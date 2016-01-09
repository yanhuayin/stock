#ifndef __STOCK_APP_DATA_H__
#define __STOCK_APP_DATA_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "StockConfig.h"
#include "StockLocateData.h"
#include "TradeSettingsData.h"

class CStockAppData
{
public:
    CStockAppData();

public:
    bool    Load();
    bool    Save();
    bool    IsRead() const { return m_load; }
    bool    IsLoaded() const { return m_load; }

    CString const&  LocFile() const { return m_locFile; }
    void            LocFile(CString const& file) { m_locFile = file; }
    CString const&  SetFile() const { return m_setFile; }
    void            SetFile(CString const& file) { m_setFile = file; }
    CString const&  AppName() const { return m_appName; }

public:
    CStockLocateData&   LocateData() { return m_locate; }
    CTradeSettingsData& TradeSettingsData() { return m_tSettings; }

private:
    bool    ParseConfig(RapidDocument &doc);
    bool    LoadWind();
    bool    LoadConfig();

private:
    CStockLocateData    m_locate;
    CTradeSettingsData  m_tSettings;

    bool                m_load;
    CString             m_cfgFile;
    CString             m_cfgFullPath;

    CString             m_appName;
    CString             m_locFile;
    CString             m_setFile;
    CString             m_windFile;
};



#endif // !__STOCK_APP_DATA_H__

