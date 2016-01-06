#include "stdafx.h"
#include "resource.h"
#include <Psapi.h>
#include <PathCch.h>
#include "Utils.h"
#include "StockGlobal.h"
#include "StockAppData.h"

#define ST_APP_NAME             (_T("Stock"))

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
            m_locFile.Format(_T("%s\\%s"), buffer, m_locFile.GetString());
            m_setFile.Format(_T("%s\\%s"), buffer, m_setFile.GetString());

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

                        if (!doc.HasParseError() && doc.IsObject())
                        {
                            if (this->ParseConfig(doc))
                            {
                                m_load = true;
                                return m_load;
                            } // debug output has been done by ParseConfig
                        }
                        else
                        {
                            if (doc.HasParseError())
                            {
                                TRACE1("Failed parse stock app config, error code: %d\n", doc.GetParseError());
                            }
                            else
                            {
                                TRACE0("Failed parse stock app config\n");
                            }

                            TRACE1("Warning: Default locate file %s is used\n", m_locFile.GetString());
                            TRACE1("Warning: Default settings file %s is used\n", m_setFile.GetString());
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

bool CStockAppData::Save()
{
    ASSERT(m_load); // must load before save

    CFile file;
    CFileException fexp;

    CString name;
    name.Format(_T("%s\\%s"), m_cfgFullPath, m_cfgFile);

    if (file.Open(name, CFile::modeCreate | CFile::modeReadWrite, &fexp))
    {
        RapidDocument doc;
        RapidDocument::AllocatorType &a = doc.GetAllocator();
        RapidValue &root = doc.SetObject();

        root.AddMember(RapidDocument::StringRefType(ST_CFG_APP_NAME), RapidDocument::StringRefType(m_appName.GetString()), a);
        root.AddMember(RapidDocument::StringRefType(ST_CFG_LOC_FILE), RapidDocument::StringRefType(m_locFile.GetString()), a);
        root.AddMember(RapidDocument::StringRefType(ST_CFG_SET_FILE), RapidDocument::StringRefType(m_setFile.GetString()), a);

        RapidStringBuffer buff;
        RapidWriter writer(buff);

        doc.Accept(writer);

        file.Write(doc.GetString(), doc.GetStringLength());

        return true;
    }
    else
    {
        TCHAR err[256];
        fexp.GetErrorMessage(err, 256);
        TRACE2("Can't open file %s, error is: %s\n", name.GetString(), err);
    }

    AfxMessageBox(IDS_APP_SAVE_DATA_FAILED);

    return false;
}

bool CStockAppData::ParseConfig(RapidDocument & doc)
{

    bool parsed = false;

    if (doc.HasMember(ST_CFG_APP_NAME))
    {
        RapidValue &name = doc[ST_CFG_APP_NAME];
        if (name.IsString())
        {
            if (m_appName == name.GetString())
            {
                parsed = true;
            }
        }
    }

    if (!parsed)
    {
        TRACE0("Warning: cannot parse app name, default is used\n");
    }

    parsed = false;

    if (doc.HasMember(ST_CFG_LOC_FILE))
    {
        RapidValue &locFile = doc[ST_CFG_LOC_FILE];
        if (locFile.IsString())
        {
            m_locFile = locFile.GetString();
            parsed = true;
        }
    }

    if (!parsed)
    {
        TRACE1("Warning: cannot parse locate config file path, default one % is used\n", m_locFile.GetString());
    }

    parsed = false;

    if (doc.HasMember(ST_CFG_SET_FILE))
    {
        RapidValue &setFile = doc[ST_CFG_SET_FILE];
        if (setFile.IsString())
        {
            m_setFile = setFile.GetString();
            parsed = true;
        }
    }

    if (!parsed)
    {
        TRACE1("Warning: cannot parse trade settings config file path, default one % is used\n", m_setFile.GetString());
    }

    return true;
}
