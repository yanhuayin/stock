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
    union 
    {
        HWND        hwnd;
        HTREEITEM   hitem;
    };
};

class CStockLocateData
{
public:
    CStockLocateData(): m_load(false), m_ready(false), m_tID(0) {}

public:
    bool    Load(CString const& file);
    bool    Save(CString const& file);
    bool    LocateWnd();

    bool    IsLoaded() const { return m_load; }
    bool    IsReady() const { return m_ready; }

    CLocateInfo const& LocInfo(LocateType type) const { return m_info[type]; }
    void    SetInfo(LocateType type, POINT pos, HWND hwnd);
    HTREEITEM   LocateTreeItem(HWND tree, LocateType type);
    bool        ValidateHwnd(HWND hwnd, LocateType type);

private:
    int         FindIdByName(CString const& name);
    HTREEITEM   SelectTreeItem(HWND tree, LocateType type);
    HWND        PointToWnd(POINT const& pos);

private:
    bool            m_load;
    bool            m_ready;
    DWORD           m_tID;

    CString         m_target;
    CLocateInfo     m_info[LT_Num];
};






#endif // !__STOCK_LOCATE_DATA_H__

