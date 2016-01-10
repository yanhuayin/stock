#include "stdafx.h"
#include "TradeModel.h"
#include "wind\WAPIWrapperCpp.h"

#define ST_DATE_TIME_FORMAT_STR     _T("%Y%m%d")

#define ST_REPORT_NAME              _T("SectorConstituent")
#define ST_SECTOR_ID                _T("a001010100000000")
#define ST_NAME_FILED               _T("sec_name")

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

    void s_variant_to_cstring(const LPVARIANT data, CString &str)
    {
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
            str.Format(_T("%NaN"));
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
    }
}


bool CTradeModelManager::Init()
{
    LONG errCode = CWAPIWrapperCpp::start();
    if (errCode != 0)
    {
        s_show_wind_err(errCode);
        return false;
    }

    m_date = CTime::GetCurrentTime().Format(ST_DATE_TIME_FORMAT_STR);

    CString options;
    options.Format(_T("date=%s;sectorId=%s"), m_date.GetString(), ST_SECTOR_ID);

    WindData wd;
    errCode = CWAPIWrapperCpp::wset(wd, ST_REPORT_NAME, options);
    if (errCode != 0)
    {
        s_show_wind_err(errCode);
        return false;
    }

    INT codesLen = wd.GetCodesLength();
    INT fieldsLen = wd.GetFieldsLength();

    INT j = 0;
    CString secName(ST_NAME_FILED);
    for (; j < fieldsLen; ++j)
    {
        if (secName == wd.GetFieldsByIndex(j))
        {
            break;
        }
    }

    if (j == fieldsLen)
    {
        AfxMessageBox(_T("wind error!")); // TODO 
        this->Shutdown();
    }

    for (INT i = 0; i < codesLen; ++i)
    {
        TradeModelHandle h(new CTradeModel());

        h->m_code = wd.GetCodeByIndex(i);

        VARIANT var;
        wd.GetDataItem(0, i, j, var);
        s_variant_to_cstring(&var, h->m_name);

        m_models[h->m_code] = h;
    }

    return true;
}

bool CTradeModelManager::Shutdown()
{
    LONG errCode = CWAPIWrapperCpp::stop();
    if (errCode != 0)
    {
        s_show_wind_err(errCode);
        return false;
    }

    return true;
}

void CTradeModelManager::FreeModel(TradeModelHandle h)
{
}
