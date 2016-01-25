#include "stdafx.h"
#include "resource.h"
#include "StockGlobal.h"
#include "TradeOrder.h"
#include "Stock.h"
#include "ApiHelper.h"

#define ST_TO_F     -1

#define ST_TO_TIME_FORMAT_STR       _T("%Y%m%d")

#define ST_ORDER_COL_LEN            32

#define ST_ORDER_DEL_NUM            3

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
    UINT capId;

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
        capId = IDS_BUY_CONFIRM;
        break;
    case STO_Sell:
        tree = loc.LocInfo(LT_Sell).hwnd;
        item = loc.LocInfo(LT_Sell).hitem;
        pos = loc.LocInfo(LT_Sell).pos;
        hp = loc.LocInfo(LT_SellPrice).hwnd;
        hc = loc.LocInfo(LT_SellCode).hwnd;
        hq = loc.LocInfo(LT_SellQuant).hwnd;
        ho = loc.LocInfo(LT_SellOrder).hwnd;
        capId = IDS_SELL_CONFIRM;
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

        ::PostMessage(ho, BM_CLICK, 0, 0); // Must use post

        ::Sleep(ST_SLEEP_T);

        HWND hconfirm = ::FindWindow(nullptr, CString(MAKEINTRESOURCE(capId)));

        if (!hconfirm)
            return ST_TO_F;

        ::SendMessage(hconfirm, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);

        ::Sleep(ST_SLEEP_T);

        HWND hhint = ::FindWindow(nullptr, CString(MAKEINTRESOURCE(IDS_HINT)));

        if (hhint)
        {
            ::SendMessage(hhint, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
        }

        // find the delegate id
        ::Sleep(ST_SLEEP_T);

        LocateInfo const& query = loc.LocInfo(LT_Query);

        if (!WinApi::IsTreeItemExpanded(tree, query.hitem))
        {
            if (!WinApi::SelectTreeItem(process, tree, query.hitem, query.pos))
                return ST_TO_F;
        }

        LocateInfo const& del = loc.LocInfo(LT_Delegate);
        if (WinApi::SelectTreeItem(process, tree, del.hitem, del.pos))
        {
            ::Sleep(ST_SLEEP_T * 3); // the delegate list seems will be refreshed only when it is opened

            //double dPrice = _tstof(price.GetString());
            //CString compPrice;
            //compPrice.Format(_T("%.3f"), dPrice); // delegate price only have 3 decimal precision

            VirtualPtr pItem = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE));
            VirtualPtr pText = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TCHAR) * ST_ORDER_COL_LEN, MEM_COMMIT, PAGE_READWRITE));

            int idCol = loc.ListCol(LT_DelegateList, SOF_Id).col;
            int codeCol = loc.ListCol(LT_DelegateList, SOF_Code).col;
            int priceCol = loc.ListCol(LT_DelegateList, SOF_Price).col;
            int quantCol = loc.ListCol(LT_DelegateList, SOF_Quant).col;

            HWND dlst = loc.LocInfo(LT_DelegateList).hwnd;

            int count = 0;
            do
            {
                int row = ::SendMessage(dlst, LVM_GETITEMCOUNT, 0, 0);
                if (row > 0)
                {
                    row -= 1;

                    TCHAR idT[ST_ORDER_COL_LEN] = { 0 };
                    TCHAR codeT[ST_ORDER_COL_LEN] = { 0 };
                    TCHAR priceT[ST_ORDER_COL_LEN] = { 0 };
                    TCHAR quantT[ST_ORDER_COL_LEN] = { 0 };

                    if (WinApi::QueryListItemText(process, dlst, row, codeCol, &codeT[0], sizeof(TCHAR) * ST_ORDER_COL_LEN, pItem, pText) &&
                        WinApi::QueryListItemText(process, dlst, row, priceCol, &priceT[0], sizeof(TCHAR) * ST_ORDER_COL_LEN, pItem, pText) &&
                        WinApi::QueryListItemText(process, dlst, row, quantCol, &quantT[0], sizeof(TCHAR) * ST_ORDER_COL_LEN, pItem, pText) &&
                        WinApi::QueryListItemText(process, dlst, row, idCol, &idT[0], sizeof(TCHAR) * ST_ORDER_COL_LEN, pItem, pText))
                    {
                        if ((code == codeT) && (price == priceT) && (quant == quantT))
                        {
                            int id = ++m_id;

                            TradeOrder &order = m_orders[id];
                            order.id = idT;
                            order.code = code;
                            order.price = price;
                            order.quant = quant;

                            m_total += (_ttoi(quant));

                            return id;
                        }
                    }
                }

                ::Sleep(ST_SLEEP_T);

                ++count;

            } while (count < ST_ORDER_DEL_NUM);

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
            ::Sleep(ST_SLEEP_T * 2);

            TradeOrder const& to = it->second;

            HWND clst = loc.LocInfo(LT_CancelList).hwnd;
            int row = ::SendMessage(clst, LVM_GETITEMCOUNT, 0, 0);
            if (row > 0)
            {
                VirtualPtr pRowItem = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE));
                VirtualPtr pRowText = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, MEM_COMMIT, PAGE_READWRITE));

                int col = loc.ListCol(LT_CancelList, SOF_Id).col;
                for (int i = 0; i < row; ++i)
                {
                    TCHAR rtext[ST_MAX_VIRTUAL_BUF] = { 0 };

                    if (WinApi::QueryListItemText(process, clst, i, col, &rtext[0], sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, pRowItem, pRowText))
                    {
                        if (to.id == rtext)
                        {
                            ListView_SetItemState(clst, i, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);

                            ::SendMessage(::GetParent(clst), (WM_USER + 20820), (WPARAM)i, 0);

                            POINT pos;
                            if (!WinApi::CacListItemCenter(process, clst, i, 0, LVIR_LABEL, pos))
                                return SOR_Error;

                            if (!WinApi::SelectListItem(process, clst, i, 0, pos))
                                return SOR_Error;

                            ::Sleep(ST_SLEEP_T);

                            ::PostMessage(loc.LocInfo(LT_CancelBtn).hwnd, BM_CLICK, 0, 0);

                            ::Sleep(ST_SLEEP_T);

                            CString cap(MAKEINTRESOURCE(IDS_HINT));

                            HWND hhint = ::FindWindow(nullptr, cap);

                            if (hhint)
                            {
                                ::PostMessage(hhint, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);

                                ::Sleep(ST_SLEEP_T);

                                hhint = ::FindWindow(nullptr, cap);

                                if (hhint)
                                {
                                    ::PostMessage(hhint, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
                                }
                            }
                            else
                            {
                                hhint = ::FindWindow(nullptr, CString(MAKEINTRESOURCE(IDS_CANCEL_HINT)));
                                if (hhint)
                                {
                                    ::PostMessage(hhint, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
                                    return SOR_Error;
                                }
                            }

                            return SOR_OK;
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
