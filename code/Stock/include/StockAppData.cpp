#include "stdafx.h"
#include "resource.h"
#include <Psapi.h>
#include "Utils.h"
#include "ApiHelper.h"
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
    m_load = this->LoadConfig(); //&& this->LoadWind();

    if (!m_load)
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
        RapidEncodeOutputStream eos(buff);
        RapidWriter writer(eos);

        doc.Accept(writer);

        file.Write(buff.GetString(), buff.GetSize());

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

bool CStockAppData::LoadWind()
{
    bool res = false;

    CString keyPath(MAKEINTRESOURCE(IDS_WIND_PATH));
    CString valueName(_T("BinPath"));

    HKEY windKey;
    if (::RegOpenKeyEx(HKEY_CURRENT_USER, keyPath, 0, KEY_READ, &windKey) == ERROR_SUCCESS)
    {
        DWORD type = REG_SZ;
        DWORD size = 0;

        if (::RegQueryValueEx(windKey, valueName, NULL, &type, NULL, &size) == ERROR_SUCCESS)
        {
            BYTE *path = new BYTE[size + 1];
            path[size] = '\0';

            if (::RegQueryValueEx(windKey, valueName, NULL, &type, path, &size) == ERROR_SUCCESS)
            {
                m_windFile.Format(_T("%sWindQuantLibrary.dll"), (TCHAR*)path);
                res = true;
            }

            ST_SAFE_DELETE_ARRAY(path);
        }

        ::RegCloseKey(windKey);
        ::RegCloseKey(HKEY_CURRENT_USER);
    }
    else
    {
        ::RegCloseKey(HKEY_CURRENT_USER);
    }

    return res;
}

bool CStockAppData::LoadConfig()
{
    bool res = false;

    TCHAR buffer[MAX_PATH];
    DWORD size = MAX_PATH;

    if (::QueryFullProcessImageName(::GetCurrentProcess(), 0, buffer, &size))
    {
        TRACE1("Full process image name is %s\n", buffer);

        if (::PathRemoveFileSpec(buffer) == TRUE)
        {
            m_cfgFullPath = buffer;
            m_locFile.Format(_T("%s\\%s"), buffer, m_locFile.GetString());
            m_setFile.Format(_T("%s\\%s"), buffer, m_setFile.GetString());

            if (::PathAppend(buffer, m_cfgFile) == TRUE)
            {
                BOOL exists = WinApi::FileExists(buffer);

                if (exists)
                {
                    CFile cfg;
                    CFileException  exp;

                    if (cfg.Open(buffer, CFile::modeRead, &exp))
                    {
                        cfg.SeekToBegin();

                        UINT len = (UINT)cfg.GetLength() + 1;
                        char *buf = new char[len];
                        len = cfg.Read(buf, len);

                        if (len == 0)
                        {
                            m_load = true;
                            return m_load;
                        }

                        buf[len] = '\0';

                        RapidStringStream s(buf);
                        RapidEncodeInputStream eis(s);

                        RapidDocument doc;
                        doc.ParseStream<rapidjson::kParseDefaultFlags | rapidjson::kParseStopWhenDoneFlag, RapidUTF8>(eis);

                        ST_SAFE_DELETE_ARRAY(buf);

                        if (!doc.HasParseError() && doc.IsObject())
                        {
                            res = this->ParseConfig(doc);
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
                    res = true;
                }

            }
        }
    }

    return res;
}
