#ifndef __TRADE_SETTINGS_DATA_H__
#define __TRADE_SETTINGS_DATA_H__

#if _MSC_VER > 1000
#pragma once
#endif

class CTradeSettingsData
{
public:
    CTradeSettingsData() {}

public:
    bool    Load(CString const& file);
};




#endif // __TRADE_SETTINGS_DATA_H__

