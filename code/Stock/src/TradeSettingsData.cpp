#include "stdafx.h"
#include "resource.h"
#include <PathCch.h>
#include "Utils.h"
#include "StockConfig.h"
#include "TradeSettingsData.h"

#define ST_SET_QUOTA_NAME       _T("quota")

bool CTradeSettingsData::Load(CString const & file)
{
    BOOL exists = ::PathFileExists(file);

    if (exists)
    {
        CFile f;
        CFileException fexp;
        if (f.Open(file, CFile::modeRead, &fexp))
        {
            f.SeekToBegin();

            UINT len = (UINT)f.GetLength() + 1;
            TCHAR *buf = new TCHAR[len];
            len = f.Read(buf, len);

            if (len == 0)
            {
                m_load = true;
                return m_load;
            }

            RapidDocument doc;
            doc.Parse(buf);

            ST_SAFE_DELETE_ARRAY(buf);

            if (!doc.HasParseError() && doc.IsObject())
            {
                if (doc.HasMember(ST_SET_QUOTA_NAME))
                {
                    RapidValue &qualVal = doc[ST_SET_QUOTA_NAME];
                    if (qualVal.IsUint())
                    {
                        m_quota = qualVal.GetUint();
                        m_ready = true;
                    }
                    else
                    {
                        TRACE1("Warning: failed parse quota setings, default value %u is used\n", m_quota);
                    }
                }
                else
                {
                    TRACE1("Warning: cannot find quota setting, default value %u is used\n", m_quota);
                }

                m_load = true;
                return m_load;
            }
            else
            {
                if (doc.HasParseError())
                    TRACE1("Failed parse trade settings file, error code: %d\n", doc.GetParseError());
                else
                    TRACE0("Failed parse trade settings file.\n");
            }
        }
        else
        {
            TCHAR err[256];
            fexp.GetErrorMessage(err, 256);
            TRACE2("Can't open file %s, error is: %s\n", file.GetString(), err);
        }
    }
    else
    {
        m_load = true;
        return m_load;
    }

    AfxMessageBox(IDS_LOAD_SETTINGS_DATA_FAILED);

    return m_load;
}

bool CTradeSettingsData::Save(CString const & file)
{
    ASSERT(m_load);

    CFile f;
    CFileException fexp;

    if (f.Open(file, CFile::modeCreate | CFile::modeReadWrite, &fexp))
    {
        RapidDocument doc;
        RapidDocument::AllocatorType &a = doc.GetAllocator();
        RapidValue &root = doc.SetObject();

        root.AddMember(RapidDocument::StringRefType(ST_SET_QUOTA_NAME), m_quota, a);

        RapidStringBuffer buff;
        RapidWriter writer(buff);

        doc.Accept(writer);

        f.Write(buff.GetString(), buff.GetSize());

        return true;
    }
    else
    {
        TCHAR err[256];
        fexp.GetErrorMessage(err, 256);
        TRACE2("Can't open file %s, error is: %s\n", file.GetString(), err);
    }

    AfxMessageBox(IDS_SAVE_SETTINGS_DATA_FAILED);

    return false;
}
