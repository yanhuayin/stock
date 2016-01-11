#ifndef __STOCK_LOCATE_DATA_H__
#define __STOCK_LOCATE_DATA_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "StockConfig.h"

struct CLocateInfo
{
    POINT       pos;
    CString     name;
    HWND        hwnd;
};

class CStockLocateData
{
public:
    CStockLocateData(): m_load(false), m_ready(false) {}

public:
    bool    Load(CString const& file);
    bool    Save(CString const& file);
    bool    LocateWnd();

    bool    IsLoaded() const { return m_load; }
    bool    IsReady() const { return m_ready; }

    CLocateInfo const& LocInfo(LocateType type) const { return m_info[type]; }
    void    SetInfo(LocateType type, POINT pos, HWND hwnd);

private:
    int     FindIdByName(CString const& name);
    HWND    SelectDelegateTreeItem(HWND tree);

private:
    bool            m_load;
    bool            m_ready;

    CString         m_target;
    CLocateInfo     m_info[LT_Num];
};






#endif // !__STOCK_LOCATE_DATA_H__

