#ifndef __STOCK_LOCATE_DATA_H__
#define __STOCK_LOCATE_DATA_H__

#if _MSC_VER > 1000
#pragma once
#endif

struct CLocateInfo
{
    POINT       pos;
    CString     name;
    int         id;
};

class CStockLocateData
{
public:
    CStockLocateData(): m_load(false), m_ready(false) {}

public:
    bool    Load(CString const& file);
    bool    Save(CString const& file);

    bool    IsLoaded() const { return m_load; }
    bool    IsReady() const { return m_ready; }

public:
    enum LocateType
    {
        LT_Buy = 0,
        LT_BuyCode,
        LT_BuyPrice,
        LT_BuyQuant,
        LT_BuyOrder,

        LT_Sell,
        LT_SellCode,
        LT_SellPrice,
        LT_SellQuant,
        LT_SellOrder,

        LT_Cancel,
        LT_CancelList,

        LT_Delegate,
        LT_DelegateList,

        LT_Num
    };

private:
    int FindIdByName(CString const& name);

private:
    bool            m_load;
    bool            m_ready;

    CString         m_target;
    CLocateInfo     m_info[LT_Num];
};






#endif // !__STOCK_LOCATE_DATA_H__

