#ifndef __STOCK_LOCATE_DATA_H__
#define __STOCK_LOCATE_DATA_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "StockConfig.h"
#include "ApiHelper.h"

struct LocateInfo
{
    POINT               pos;
    CString             name;
    HWND                hwnd;
    HTREEITEM           hitem;
};

class CStockLocateData // TODO : I think we'd better move all hwnd manipulate out of this class
{
public:
    CStockLocateData(): m_load(false), m_ready(false), m_tID(0), m_process(nullptr), m_tSet(false), m_pSet(false) {}

public:
    bool    Load(CString const& file);
    bool    Save(CString const& file);
    bool    LocateWnd();

    bool    IsLoaded() const { return m_load; }
    bool    IsReady() const { return m_ready; }

    void    Clean() { m_process = nullptr; }

    LocateInfo const& LocInfo(LocateType type) const { return m_info[type]; }
    ListViewColumn const& ListCol(LocateType listType, StockOrderField col) const;

    CString const&  TargetString() const { return m_target; }
    DWORD       TargetId() const { return m_tID; }
    HandlePtr   TargetProcess() const { return m_process; }

    void        SetListCol(LocateType listType, StockOrderField col, ListViewColumn const& c);
    void        SetInfo(LocateType type, LocateInfo const& i);
    void        SetTarget(CString const& target, DWORD id, HandlePtr process) { m_target = target; m_tID = id; m_process = process; }
    void        SetReady(bool ready); // TODO : ready change should notify all views

    void        ResetListCol(ListViewColumn &col, StockOrderField field) const;
    void        ResetLocateInfo(LocateInfo &info, LocateType type) const;

    HWND        ValidateHwnd(HWND hwnd, LocateType type, CString &target, DWORD &pId, HandlePtr &process, HTREEITEM *hitem = nullptr) const;
    int         GetListCol(HandlePtr process, HWND list, ListViewColumn *info, size_t cnt) const;

private:
    int         FindIdByName(CString const& name) const;
    HTREEITEM   SearchTreeItem(HandlePtr process, HWND tree, LocateType type) const;
    HWND        PointToTopWnd(POINT const& pos);
    HWND        ValidateTopWnd(HWND hwnd);
    void        Withdraw();

private:
    bool            m_load;
    bool            m_ready;
    DWORD           m_tID;
    HandlePtr       m_process;
    CString         m_target;
    bool            m_tSet;
    bool            m_pSet;

    LocateInfo      m_info[LT_Num];

    ListViewColumn  m_cancel[SOF_Num];
    ListViewColumn  m_delegate[SOF_Num];
};






#endif // !__STOCK_LOCATE_DATA_H__

