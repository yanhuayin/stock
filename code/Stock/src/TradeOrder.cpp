#include "stdafx.h"
#include "resource.h"
#include "StockGlobal.h"
#include "TradeOrder.h"
#include "Stock.h"
#include "ApiHelper.h"

#define ST_TO_F     -1

#define ST_TO_TIME_FORMAT_STR     _T("%Y%m%d")

// LVM_GETITEMCOUNT
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb761044%28v=vs.85%29.aspx
// to get the row number in a listview

// LVM_GETHEADER
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb774937%28v=vs.85%29.aspx
// to get the handle of the header of a listview

// HDM_GETITEMCOUNT
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb775337%28v=vs.85%29.aspx
// to get the column number of a listviw

int CTradeOrderManager::Trade(StockTradeOp op, CString const & code, CString const & price, CString const & quant)
{
    // we should make sure code/price/quant have valid value and loc data is ready

    CStockLocateData &loc = theApp.AppData().LocateData();

    HWND tree = nullptr;
    HTREEITEM item = nullptr;
    POINT pos;
    HWND hp = nullptr;
    HWND hc = nullptr;
    HWND hq = nullptr;
    HWND ho = nullptr;
    int key = -1;
    int cmd = -1;

    switch (op)
    {
    case STO_Buy:
        tree = loc.LocInfo(LT_Buy).hwnd;
        item = loc.LocInfo(LT_Buy).hitem;
        pos = loc.LocInfo(LT_Buy).pos;
        hp = loc.LocInfo(LT_BuyPrice).hwnd;
        hc = loc.LocInfo(LT_BuyCode).hwnd;
        hq = loc.LocInfo(LT_BuyQuant).hwnd;
        ho = loc.LocInfo(LT_BuyOrder).hwnd;
        break;
    case STO_Sell:
        tree = loc.LocInfo(LT_Sell).hwnd;
        item = loc.LocInfo(LT_Sell).hitem;
        pos = loc.LocInfo(LT_Sell).pos;
        hp = loc.LocInfo(LT_SellPrice).hwnd;
        hc = loc.LocInfo(LT_SellCode).hwnd;
        hq = loc.LocInfo(LT_SellQuant).hwnd;
        ho = loc.LocInfo(LT_SellOrder).hwnd;
        break;
    default:
        return ST_TO_F;
    }

    HandlePtr process = loc.TargetProcess();

    if (!process)
        return ST_TO_F;

    if (WinApi::SelectTreeItem(process, tree, item, pos))
    {
        ::Sleep(ST_SLEEP_T);

        if (!this->SetText(hc, code))
            return ST_TO_F;

        ::Sleep(ST_SLEEP_T);

        if (!this->SetText(hp, price))
            return ST_TO_F;

        ::Sleep(ST_SLEEP_T);

        if (!this->SetText(hq, quant))
            return ST_TO_F;

        ::Sleep(ST_SLEEP_T);

        ::SendMessage(ho, BM_CLICK, 0, 0);

        // find the delegate id
        if (WinApi::SelectTreeItem(process, tree, loc.LocInfo(LT_Delegate).hitem, loc.LocInfo(LT_Delegate).pos))
        {
            HWND dlst = loc.LocInfo(LT_DelegateList).hwnd;
            int row = ::SendMessage(dlst, LVM_GETITEMCOUNT, 0, 0);
            if (row > 0)
            {
                VirtualPtr pRowItem = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE));
                VirtualPtr pRowText = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, MEM_COMMIT, PAGE_READWRITE));

                TCHAR rtext[ST_MAX_VIRTUAL_BUF] = { 0 };

                LVITEM lvrItem = { 0 };
                lvrItem.mask = LVIF_TEXT;
                lvrItem.iSubItem = loc.ListCol(LT_DelegateList, SOF_Id).col;
                lvrItem.pszText = (LPTSTR)pRowText.get();
                lvrItem.cchTextMax = ST_MAX_VIRTUAL_BUF;
                lvrItem.iItem = row - 1;

                if (::WriteProcessMemory(process.get(), pRowItem.get(), &lvrItem, sizeof(LVITEM), nullptr))
                {
                    int count = ::SendMessage(dlst, LVM_GETITEMTEXT, row - 1, (LPARAM)(pRowItem.get()));
                    if (count > 0)
                    {
                        if (::ReadProcessMemory(process.get(), pRowText.get(), &rtext[0], sizeof(TCHAR)*count, nullptr))
                        {
                            int id = ++m_id;

                            TradeOrder &order = m_orders[id];
                            order.id = rtext;
                            order.code = code;
                            order.price = price;
                            order.quant = quant;
                            order.date = CTime::GetCurrentTime().Format(ST_TO_TIME_FORMAT_STR);

                            return id;
                        }
                    }
                }
            }
        }
    }

    return ST_TO_F;
}

StockOrderResult CTradeOrderManager::CancelOrder(int order)
{
    CStockLocateData &loc = theApp.AppData().LocateData();

    HandlePtr process = loc.TargetProcess();

    if (!process)
        return SOR_Error;

    HWND tree = loc.LocInfo(LT_Cancel).hwnd;
    HTREEITEM item = loc.LocInfo(LT_Cancel).hitem;

    if (WinApi::SelectTreeItem(process, tree, item, loc.LocInfo(LT_Cancel).pos))
    {
        auto it = m_orders.find(order);

        if (it != m_orders.end())
        {
            TradeOrder const& to = it->second;

            HWND clst = loc.LocInfo(LT_CancelList).hwnd;
            int row = ::SendMessage(clst, LVM_GETITEMCOUNT, 0, 0);
            if (row > 0)
            {
                VirtualPtr pRowItem = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE));
                VirtualPtr pRowText = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, MEM_COMMIT, PAGE_READWRITE));

                for (int i = 0; i < row; ++i)
                {
                    TCHAR rtext[ST_MAX_VIRTUAL_BUF] = { 0 };

                    LVITEM lvrItem = { 0 };
                    lvrItem.mask = LVIF_TEXT;
                    lvrItem.iSubItem = loc.ListCol(LT_CancelList, SOF_Id).col;
                    lvrItem.pszText = (LPTSTR)pRowText.get();
                    lvrItem.cchTextMax = ST_MAX_VIRTUAL_BUF;
                    lvrItem.iItem = i;

                    if (::WriteProcessMemory(process.get(), pRowItem.get(), &lvrItem, sizeof(LVITEM), nullptr))
                    {
                        int count = ::SendMessage(clst, LVM_GETITEMTEXT, i, (LPARAM)(pRowItem.get()));
                        if (count > 0)
                        {
                            if (::ReadProcessMemory(process.get(), pRowText.get(), &rtext[0], sizeof(TCHAR)*count, nullptr))
                            {
                                if (to.id == rtext)
                                {
                                    ListView_SetItemState(clst, i, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);

                                    ::SendMessage(loc.LocInfo(LT_CancelBtn).hwnd, BM_CLICK, 0, 0);

                                    return SOR_OK;
                                }
                            }
                        }
                    }
                }


            }

            return SOR_Dealed;
        }
    }



    return SOR_Error;
}

TradeOrder const & CTradeOrderManager::Order(int order) const
{
    auto it = m_orders.find(order);

    if (it == m_orders.end())
    {
        static TradeOrder errOrder = { _T("Error"), _T("Error"), _T("Error"), _T("Error"), _T("Error") };
        return errOrder;
    }

    return it->second;
}

bool CTradeOrderManager::SetText(HWND hwnd, CString const & text) const
{
    if (::SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)(text.GetString())))
        return true;

    return false;
}
