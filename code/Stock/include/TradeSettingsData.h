#ifndef __TRADE_SETTINGS_DATA_H__
#define __TRADE_SETTINGS_DATA_H__

#if _MSC_VER > 1000
#pragma once
#endif

class CTradeSettingsData
{
public:
    CTradeSettingsData()
        : m_load(false)
        , m_ready(false)
        , m_quota(0)
        {
        }

public:
    bool    Load(CString const& file);
    bool    Save(CString const& file);
    bool    IsRead() const { return m_ready; }
    bool    IsLoaded() const { return m_load; }
    UINT    Quota() const { return m_quota; }
    void    Quota(UINT value);

private:
    bool    m_load;
    bool    m_ready;

    UINT    m_quota;
};




#endif // __TRADE_SETTINGS_DATA_H__

