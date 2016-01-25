#include "stdafx.h"
#include "TradeModel.h"
#include "TradeControl.h"

#define ST_DATE_TIME_FORMAT_STR     _T("%Y%m%d")

#define ST_REPORT_NAME              _T("SectorConstituent")
#define ST_SECTOR_ID                _T("a001010100000000")
#define ST_NAME_FIELD               _T("sec_name")
#define ST_CODE_FIELD               _T("wind_code")

#define ST_CODE_NUM_LEN             6
#define ST_CODE_60                  _T('6')

#define ST_REQ_FIELD                _T("\
rt_ask10,rt_ask9,rt_ask8,rt_ask7,rt_ask6,rt_ask5,rt_ask4,rt_ask3,rt_ask2,rt_ask1,\
rt_bid1,rt_bid2,rt_bid3,rt_bid4,rt_bid5,rt_bid6,rt_bid7,rt_bid8,rt_bid9,rt_bid10,\
rt_asize10,rt_asize9,rt_asize8,rt_asize7,rt_asize6,rt_asize5,rt_asize4,rt_asize3,rt_asize2,rt_asize1,\
rt_bsize1,rt_bsize2,rt_bsize3,rt_bsize4,rt_bsize5,rt_bsize6,rt_bsize7,rt_bsize8,rt_bsize9,rt_bsize10")

#define ST_REQ_ID_IMM               0xFFFF

#define ST_WIND_CODE_DELIMITER      _T(".")

namespace
{
    void s_show_wind_err(LONG errCode)
    {
        int size = 128;
        TCHAR buf[128];
        CWAPIWrapperCpp::getErrorMsg(errCode, eCHN, buf, size);
        buf[size - 1] = _T('\0');
        AfxMessageBox(buf);
    }

    void s_variant_to_cstring(const LPVARIANT data, String &res)
    {
        CString str;
        
        switch (data->vt & VT_BSTR_BLOB)
        {
        case VT_I4:
            str.Format(_T("%d"), data->intVal);
            break;
        case VT_I8:
            str.Format(_T("%I64d"), data->llVal);
            break;
        case VT_R4:
            str.Format(_T("%f"), data->fltVal);
            break;
        case VT_R8:
            str.Format(_T("%f"), data->dblVal);
            break;
        case VT_EMPTY:
            str.Format(_T("NaN"));
            break;
        case VT_BSTR:
            str.Format(_T("%s"), data->bstrVal);
            break;
        case VT_DATE:
            str = COleDateTime(data->date).Format();
            break;
        default:
            break;
        }

        res = str.GetString();
    }

    LPCTSTR s_code_suffix(String const& code)
    {
        static String s_sz(_T("SZ")), s_sh(_T("SH"));

        if (code.at(0) == ST_CODE_60)
        {
            return s_sh.c_str();
        }
        else
        {
            return s_sz.c_str();
        }
    }

    LONG WINAPI s_req_callback(ULONGLONG reqId, WindData const& wd)
    {
        CTradeModelManager::Instance().UpdateModel(reqId, wd);

        return 0;
    }
}

bool CTradeModelManager::Init(RequestType type)
{
    LONG errCode = CWAPIWrapperCpp::start();
    if (errCode != 0)
    {
        s_show_wind_err(errCode);
        return false;
    }

    m_date = CTime::GetCurrentTime().Format(ST_DATE_TIME_FORMAT_STR);

    CString options;
    options.Format(_T("date=%s;sectorId=%s"), m_date.c_str(), ST_SECTOR_ID);

    WindData wd;
    errCode = CWAPIWrapperCpp::wset(wd, ST_REPORT_NAME, options);
    if (errCode != 0)
    {
        s_show_wind_err(errCode);
        return false;
    }

    INT codesLen = wd.GetCodesLength();
    INT fieldsLen = wd.GetFieldsLength();

    INT nId = -1, cId = -1;
    CString secName(ST_NAME_FIELD);
    CString windCode(ST_CODE_FIELD);
    for (INT i = 0; i < fieldsLen; ++i)
    {
        if (secName == wd.GetFieldsByIndex(i))
        {
            nId = i;
        }
        else if (windCode == wd.GetFieldsByIndex(i))
        {
            cId = i;
        }
    }

    // date wind_code sec_name

    if (nId < 0 || cId < 0)
    {
        AfxMessageBox(_T("wind error!")); // TODO 
        this->Shutdown();
    }

    for (INT i = 0; i < codesLen; ++i)
    {
        TradeModelHandle h(new CTradeModel());

        h->m_code = wd.GetCodeByIndex(i);

        VARIANT var;
        wd.GetDataItem(0, i, nId, var);
        s_variant_to_cstring(&var, h->m_name);

        wd.GetDataItem(0, i, cId, var);
        s_variant_to_cstring(&var, h->m_windCode);

        CString str(h->m_windCode.c_str());
        int pos = 0;
        h->m_windCodeNum = str.Tokenize(ST_WIND_CODE_DELIMITER, pos);
        h->m_windCodeSuffix = str.Tokenize(ST_WIND_CODE_DELIMITER, pos);

        m_models[h->m_windCodeNum] = h;
    }

    m_type = type;
    // =================== TEST ===============================
    //TradeModelHandle h(new CTradeModel);
    //h->m_name = _T("Test");
    //h->m_windCode = _T("000628.SZ");
    //h->m_windCodeNum = _T("000628");
    //h->m_windCodeSuffix = _T("SZ");
    //m_models[h->m_windCodeNum] = h;
    // =================== TEST ===============================

    m_init = true;

    return m_init;
}

bool CTradeModelManager::Shutdown()
{
    if (m_init)
    {
        if (m_type == RT_MultiThread)
            CWAPIWrapperCpp::cancelAllRequest();

        m_init = false;
    }

    //LONG errCode = CWAPIWrapperCpp::stop();
    //if (errCode != 0)
    //{
    //    s_show_wind_err(errCode);
    //    return false;
    //}

    return true;
}

void CTradeModelManager::FreeModel(TradeModelHandle h)
{
    if (m_init && h)
    {
        if (m_type == RT_MultiThread && h->m_reqId)
        {
            // TODO : multi thread safety
            CWAPIWrapperCpp::cancelRequest(h->m_reqId);

            m_reqs.erase(h->m_reqId);
            h->m_reqId = 0;
        }
        else if (m_type == RT_Immediate && h->m_reqId == ST_REQ_ID_IMM)
        {
            h->m_reqId = 0;
        }

        //h->m_price = nullptr;
        //h->m_quant = nullptr;
    }
}

bool CTradeModelManager::RequestModel(TradeModelHandle h, bool force)
{
    if (m_init && h)
    {
        if (m_type == RT_Immediate)
        {
            if (force || h->m_reqId != ST_REQ_ID_IMM)
            {
                WindData wd;
                LONG errCode = CWAPIWrapperCpp::wsq(wd, h->m_windCode.c_str(), ST_REQ_FIELD, nullptr);

                if (errCode != 0)
                {
                    s_show_wind_err(errCode);
                    return false;
                }

                h->m_reqId = ST_REQ_ID_IMM;
                this->UpdateModel(h, wd); // immediate request will update model at once

                // =================== TEST ===============================
                //if (!h->m_price)
                //{
                //    h->m_price = CTradeModel::InfoNumArrayPtr(new CTradeModel::InfoNumArray());
                //}

                //CTradeModel::InfoNumArray &pa = *(h->m_price);

                //if (!h->m_quant)
                //{
                //    h->m_quant = CTradeModel::InfoNumArrayPtr(new CTradeModel::InfoNumArray());
                //}

                //CTradeModel::InfoNumArray &qa = *(h->m_quant);

                //for (int i = 0; i < SIT_Num; ++i)
                //{
                //    pa[i] = 10.9 + i * 0.001;
                //    qa[i] = 100.0;
                //}
                // =================== TEST ===============================

                return true;
            }

            return true; // requested already
        }
        else
        {
            if (h->m_reqId == 0)
            {
                LONG errCode = CWAPIWrapperCpp::wsq(h->m_reqId, h->m_windCode.c_str(), ST_REQ_FIELD, s_req_callback, nullptr, TRUE);

                if (errCode != 0)
                {
                    h->m_reqId = 0;
                    s_show_wind_err(errCode);
                    return false;
                }

                m_reqs[h->m_reqId] = h;

                return true;
            }

            return true; // requested already
        }
    }

    return false;
}

void CTradeModelManager::UpdateModel(ULONGLONG reqId, WindData const & wd)
{
    ASSERT(m_type == RT_MultiThread);
    // TODO : multi thread safty
    if (m_init)
    {
        auto it = m_reqs.find(reqId);

        if (it != m_reqs.end())
        {
            TradeModelHandle h = it->second;

            this->UpdateModel(h, wd);

            CTradeControl::Instance().RefreshViewsInfo(it->second); // for multi thread safety, this should be changed
        }
        else
        {
            CWAPIWrapperCpp::cancelRequest(reqId);
        }
    }
}

TradeModelHandle CTradeModelManager::FindModel(String const & code, CandidatesList * c)
{
    if (m_init)
    {
        size_t len = code.length();
        if (len == ST_CODE_NUM_LEN)
        {
            //CString fullCode;
            //fullCode.Format(_T("%s.%s"), code.c_str(), s_code_suffix(code));

            //String _tmpCode = fullCode.GetString();

            auto it = m_models.find(code);
            if (it != m_models.end())
            {
                return it->second;
            }

            return nullptr;
        }
        else if (len < ST_CODE_NUM_LEN)
        {
            // TODO :
            return nullptr;
        }
    }

    return nullptr;
}

void CTradeModelManager::UpdateModel(TradeModelHandle h, WindData const & wd)
{
    // TODO : multithread should support not update all
    INT fields = wd.GetFieldsLength();
    if (fields == SIF_Num * SIT_Num) // 40 fields now
    {
        if (!h->m_price)
        {
            h->m_price = CTradeModel::InfoNumArrayPtr(new CTradeModel::InfoNumArray());
        }

        CTradeModel::InfoNumArray &pa = *(h->m_price);

        if (!h->m_quant)
        {
            h->m_quant = CTradeModel::InfoNumArrayPtr(new CTradeModel::InfoNumArray());
        }

        CTradeModel::InfoNumArray &qa = *(h->m_quant);

        for (int i = 0; i < SIF_Num; ++i)
        {
            for (int j = 0; j < SIT_Num; ++j)
            {
                int id = SIT_Num * i + j;

                VARIANT var;
                wd.GetDataItem(0, 0, id, var);

                double val = 0.0;
                auto flag = var.vt & VT_BSTR_BLOB;
                switch (flag)
                {
                case VT_R8:
                    val = var.dblVal;
                    break;
                default:
                    break;
                }

                if (i == SIF_Price)
                {
                    pa[j] = val;
                }
                else
                {
                    qa[j] = val;
                }
            }
        }
    }
}

CTradeModel::InfoNumArrayPtr CTradeModel::NumInfo(StockInfoField field)
{
    switch (field)
    {
    case SIF_Price:
        return m_price;
    case SIF_Quant:
        return m_quant;
    default:
        break;
    }

    return nullptr;
}
