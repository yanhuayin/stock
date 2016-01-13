#ifndef __STOCK_LOCATE_DATA_H__
#define __STOCK_LOCATE_DATA_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "StockConfig.h"
#include "ApiHelper.h"

struct CLocateInfo
{
    POINT       pos;
    CString     name;
    HWND        hwnd;
    HTREEITEM   hitem;
};

class CStockLocateData // TODO : I think we'd better move all hwnd manipulate out of this class
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
    DWORD       Target(CString & outTarget) const { outTarget = m_target; return m_tID; }
    void        SetInfo(LocateType type, POINT const& pos, HWND hwnd, HTREEITEM hitem);
    void        SetTarget(CString const& target, DWORD id) { m_target = target; m_tID = id; }
    void        SetReady(bool ready); // TODO : ready change should notify all views
    bool        ValidateHwnd(HWND hwnd, LocateType type, CString &target, DWORD &pId, HTREEITEM *hitem = nullptr) const;
    bool        OpenTradePage(HWND tree, HTREEITEM item) const;

private:
    int         FindIdByName(CString const& name) const;
    HTREEITEM   SelectTreeItem(HWND tree, LocateType type, DWORD pId) const;
    HWND        PointToTopWnd(POINT const& pos);
    HWND        ValidateTopWnd(HWND hwnd, CString const& t, DWORD pId) const;
    DWORD       QueryTargetName(HWND hwnd, CString & outName, DWORD pId) const;

private:
    bool            m_load;
    bool            m_ready;
    DWORD           m_tID;

    CString         m_target;
    CLocateInfo     m_info[LT_Num];
};






#endif // !__STOCK_LOCATE_DATA_H__

