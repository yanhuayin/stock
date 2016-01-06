#include "stdafx.h"
#include "resource.h"
#include <Psapi.h>
#include <PathCch.h>
#include "Utils.h"
#include "StockGlobal.h"
#include "StockAppData.h"

#define ST_CFG_APP_NAME (_T("app_name"))
#define ST_CFG_LOC_FILE (_T("locate_file"))
#define ST_CFG_SET_FILE (_T("seetings_file"))

CStockAppData::CStockAppData()
    : m_load(false)
    , m_cfgFile(_T("StockApp.json"))
    , m_locFile(_T("Locate.json"))
    , m_setFile(_T("Settings.json"))
    , m_appName(ST_APP_NAME)
{
}

bool CStockAppData::Load()
{
    TCHAR buffer[MAX_PATH];
    DWORD size = MAX_PATH;

    if (::QueryFullProcessImageName(::GetCurrentProcess(), 0, buffer, &size))
    {
        TRACE1("Full process image name is %s\n", buffer);

        if (::PathCchRemoveFileSpec(buffer, MAX_PATH) == S_OK)
        {
            m_cfgFullPath = buffer;

            if (::PathCchAppendEx(buffer, MAX_PATH, m_cfgFile, 0) == S_OK)
            {
                BOOL exists = ::PathFileExists(buffer);

                if (exists)
                {
                    CFile cfg;
                    CFileException  exp;

                    if (cfg.Open(buffer, CFile::modeRead, &exp))
                    {
                        cfg.SeekToBegin();

                        UINT len = (UINT)cfg.GetLength() + 1;
                        TCHAR *buf = new TCHAR[len];
                        len = cfg.Read(buf, len);

                        RapidDocument doc;
                        doc.Parse(buf);

                        ST_SAFE_DELETE_ARRAY(buf);

                        if (!doc.HasParseError())
                        {

                        }
                        else
                        {
                            TRACE1("Failed parse stock app config, error code: %d\n", m_doc.GetParseError());
                        }
                    }
                    else
                    {
                        TCHAR err[256];
                        exp.GetErrorMessage(err, 256);
                        TRACE2("Can't open file %s, error is: %s\n", buffer, err);
                    }
                }
                else
                {
                    m_load = true;
                    return m_load;
                }
                
            }
        }
    }

    AfxMessageBox(IDS_APP_LOAD_DATA_FAILED);

    return m_load;
}

bool CStockAppData::ParseConfig(RapidDocument & doc)
{
    if (doc.IsObject())
    {

    }
    else
    {
        TRACE0("Warning: Failed parse config doc\n");
        TRACE0("Warning: ")
    }

    return true;

    RapidDocument::AllocatorType &a = doc.GetAllocator();

    bool parseErr = false;
    bool showErr = false;

    if (m_doc.HasMember(ST_CFG_APP_NAME))
    {
        RapidValue &name = m_doc[ST_CFG_APP_NAME];
        if (!name.IsString())
        {
            parseErr = true;
            showErr = true;
        }
        else
        {
            if (m_appName != name.GetString())
            {
                TRACE2("Warning: app name is wrong. Expect: %s, Actual: %s\n",
                    m_appName.GetString(), name.GetString());
            }
        }
    }
    else
    {
        m_doc.AddMember(RapidDocument::StringRefType(ST_CFG_APP_NAME), RapidDocument::StringRefType(m_appName.GetString()), a);
    }

    if (m_doc.HasMember(ST_CFG_LOC_FILE))
    {
        RapidValue &locFile = m_doc[ST_CFG_LOC_FILE];
        if (locFile.IsString())
        {
            if (!m_locate.Load(locFile.GetString()))
                parseErr = true;
        }
        else
        {
            parseErr = true;
            showErr = true;
        }
    }

    if (m_doc.HasMember(ST_CFG_SET_FILE))
    {
        RapidValue &setFile = m_doc[ST_CFG_SET_FILE];
        if (setFile.IsString())
        {
            if (!m_tSettings.Load(setFile.GetString()))
                parseErr = true;
        }
        else
        {
            parseErr = true;
            showErr = true;
        }
    }

    if (!parseErr)
    {
        m_load = true;
        return m_load;
    }

    if (!showErr)
        return m_load;
}
