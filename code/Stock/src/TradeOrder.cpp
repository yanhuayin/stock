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

namespace
{
    TradeDuration sTimeDelt(5); // TODO : move this to trade settings?
}

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
    UINT flag;

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
        flag = IDS_TRADE_BUY_FLAG;
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
        flag = IDS_TRADE_SELL_FLAG;
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

        int id = ++m_id;

        TradeOrder &order = m_orders[id];

        order.local = TradeClock::now();
        order.time = order.local;
        order.code = code;
        order.price = price;
        order.quant = quant;
        order.flag.LoadString(flag);
        order.turnover = quant;
        order.deal = true;

        m_total += (_ttoi(quant));

        LocateInfo const& cancel = loc.LocInfo(LT_Cancel);
        if (WinApi::SelectTreeItem(process, tree, cancel.hitem, cancel.pos))
        {
            ::Sleep(ST_SLEEP_T);

            HWND clst = loc.LocInfo(LT_CancelList).hwnd;

            VirtualPtr pItem = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE));
            VirtualPtr pText = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TCHAR) * ST_ORDER_COL_LEN, MEM_COMMIT, PAGE_READWRITE));

            int row = ::SendMessage(clst, LVM_GETITEMCOUNT, 0, 0);
            if (row > 0)
            {
                int tcol = loc.ListCol(LT_CancelList, SOF_Time).col;
                for (int i = 0; i < row; ++i)
                {
                    TCHAR timeT[ST_ORDER_COL_LEN] = { 0 };

                    if (WinApi::QueryListItemText(process, clst, i, tcol, &timeT[0], sizeof(TCHAR) * ST_ORDER_COL_LEN, pItem, pText))
                    {
                        TradeTimePoint time = StrToTime(timeT);

                        if ((CalcTimeDiff(time, order.local)) <= sTimeDelt)
                        {
                            int codeCol = loc.ListCol(LT_CancelList, SOF_Code).col;
                            int priceCol = loc.ListCol(LT_CancelList, SOF_Price).col;
                            int quantCol = loc.ListCol(LT_CancelList, SOF_Quant).col;

                            TCHAR codeT[ST_ORDER_COL_LEN] = { 0 };
                            TCHAR priceT[ST_ORDER_COL_LEN] = { 0 };
                            TCHAR quantT[ST_ORDER_COL_LEN] = { 0 };

                            if (WinApi::QueryListItemText(process, clst, i, codeCol, &codeT[0], sizeof(TCHAR) * ST_ORDER_COL_LEN, pItem, pText) &&
                                WinApi::QueryListItemText(process, clst, i, priceCol, &priceT[0], sizeof(TCHAR) * ST_ORDER_COL_LEN, pItem, pText) &&
                                WinApi::QueryListItemText(process, clst, i, quantCol, &quantT[0], sizeof(TCHAR) * ST_ORDER_COL_LEN, pItem, pText))
                            {
                                if ((code == codeT) && (price == priceT) && (quant == quantT))
                                {
                                    order.deal = false;

                                    order.time = time;

                                    int idCol = loc.ListCol(LT_CancelList, SOF_Id).col;
                                    int turnoverCol = loc.ListCol(LT_CancelList, SOF_Turnover).col;

                                    TCHAR idT[ST_ORDER_COL_LEN] = { 0 };
                                    TCHAR turnoverT[ST_ORDER_COL_LEN] = { 0 };

                                    if (WinApi::QueryListItemText(process, clst, i, idCol, &idT[0], sizeof(TCHAR) * ST_ORDER_COL_LEN, pItem, pText) &&
                                        WinApi::QueryListItemText(process, clst, i, turnoverCol, &turnoverT[0], sizeof(TCHAR) * ST_ORDER_COL_LEN, pItem, pText))
                                    {
                                        order.id = idT;
                                        order.turnover = turnoverT;
                                    }

                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        return id;
    }

    return ST_TO_F;
}

StockOrderResult CTradeOrderManager::CancelOrder(int order)
{
    auto it = m_orders.find(order);

    if (it == m_orders.end())
        return SOR_Error;

    TradeOrder &to = it->second;

    if (to.deal)
        return SOR_OK;

    if (to.id.IsEmpty())
        return SOR_Error;

    CStockLocateData &loc = theApp.AppData().LocateData();

    HandlePtr process = loc.TargetProcess();

    if (!process)
        return SOR_Error;

    HWND tree = loc.LocInfo(LT_Cancel).hwnd;
    HTREEITEM item = loc.LocInfo(LT_Cancel).hitem;

    if (WinApi::SelectTreeItem(process, tree, item, loc.LocInfo(LT_Cancel).pos))
    {
        ::Sleep(ST_SLEEP_T * 2);

        HWND clst = loc.LocInfo(LT_CancelList).hwnd;
        int row = ::SendMessage(clst, LVM_GETITEMCOUNT, 0, 0);
        if (row > 0)
        {
            VirtualPtr pRowItem = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE));
            VirtualPtr pRowText = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TCHAR) * ST_ORDER_COL_LEN, MEM_COMMIT, PAGE_READWRITE));

            int col = loc.ListCol(LT_CancelList, SOF_Id).col;
            for (int i = 0; i < row; ++i)
            {
                TCHAR rtext[ST_ORDER_COL_LEN] = { 0 };

                if (WinApi::QueryListItemText(process, clst, i, col, &rtext[0], sizeof(TCHAR) * ST_ORDER_COL_LEN, pRowItem, pRowText))
                {
                    if (to.id == rtext)
                    {
                        col = loc.ListCol(LT_CancelList, SOF_Turnover).col;
                        TCHAR ttext[ST_ORDER_COL_LEN] = { 0 };

                        if (WinApi::QueryListItemText(process, clst, i, col, &ttext[0], sizeof(TCHAR) * ST_ORDER_COL_LEN, pRowItem, pRowText))
                        {
                            to.turnover = ttext;
                        }

                        ListView_SetItemState(clst, i, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);

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

                        return SOR_LeftOK;
                    }
                }
            }
        }

        to.turnover = to.quant;
        to.deal = true;

        return SOR_Dealed;
    }

    return SOR_Error;
}

TradeOrder const & CTradeOrderManager::Order(int order) const
{
    auto it = m_orders.find(order);

    if (it == m_orders.end())
    {
        static TradeOrder errOrder = 
        {
            TradeClock::now(), 
            TradeClock::now(), 
            _T("Error"), 
            _T("Error"),
            _T("Error"), 
            _T("Error"), 
            _T("Error"),
            _T("Error"),
            false
        };
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
