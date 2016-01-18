#ifndef __STOCK_LOCATE_DATA_H__
#define __STOCK_LOCATE_DATA_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "StockConfig.h"
#include "ApiHelper.h"

struct CLocateInfo
{
    POINT               pos;
    CString             name;
    HWND                hwnd;
    HTREEITEM           hitem;
    int                 cmd; // for toolbar
    int                 key;
};

struct CToolbarBtn
{
    LPTSTR  name;
    int     cId;
    int     cmd;
};

struct CListCol
{
    CString name;
    int     col;
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
    CListCol const& ListCol(LocateType listType, StockOrderField col) const;
    const LPTSTR    ListColName(StockOrderField col) const;
    void        SetListCol(LocateType listType, StockOrderField col, CListCol const& c);
    void        SetInfo(LocateType type, POINT const& pos, HWND hwnd, HTREEITEM hitem, int cmd);
    void        SetTarget(CString const& target, DWORD id) { m_target = target; m_tID = id; }
    void        SetReady(bool ready); // TODO : ready change should notify all views
    bool        ValidateHwnd(HWND hwnd, LocateType type, CString &target, DWORD &pId, HTREEITEM *hitem = nullptr) const;
    bool        OpenTradePage(HandlePtr process, HWND tree, HTREEITEM item, int key = -1, int cmd = -1) const;
    int         GetToolbarBtn(HWND hwnd, DWORD pId, CString const& pTarget, CToolbarBtn *info, size_t cnt) const; // TODO : use process handle instead id
    int         GetListCol(HandlePtr process, HWND list, CListCol *info, size_t cnt) const;
    void        ResetListCol(CListCol *cols, size_t size) const;
    void        ResetLocateInfo(CLocateInfo *info, size_t size) const;

private:
    int         FindIdByName(CString const& name) const;
    HTREEITEM   SelectTreeItem(HWND tree, LocateType type, DWORD pId, bool open = true) const; // TODO : use process handle instead id
    HWND        PointToTopWnd(POINT const& pos);
    HWND        ValidateTopWnd(HWND hwnd, CString const& t, DWORD pId) const; // TODO : use process handle instead id
    DWORD       QueryTargetName(HWND hwnd, CString & outName, DWORD pId) const; // TODO : use process handle instead id


private:
    bool            m_load;
    bool            m_ready;
    DWORD           m_tID;

    CString         m_target;
    CLocateInfo     m_info[LT_Num];

    CListCol        m_cancel[SOF_Num];
    CListCol        m_delegate[SOF_Num];
};






#endif // !__STOCK_LOCATE_DATA_H__

