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

public:
    CStockLocateData&   LocateData() { return m_locate; }
    CTradeSettingsData& TradeSettingsData() { return m_tSettings; }

private:
    bool    ParseConfig(RapidDocument &doc);

private:
    CStockLocateData    m_locate;
    CTradeSettingsData  m_tSettings;

    bool                m_load;
    CString             m_cfgFile;
    CString             m_cfgFullPath;

    CString             m_appName;
    CString             m_locFile;
    CString             m_setFile;
};



#endif // !__STOCK_APP_DATA_H__

