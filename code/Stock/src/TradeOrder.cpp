#include "stdafx.h"
#include "resource.h"
#include "StockGlobal.h"
#include "TradeOrder.h"
#include "Stock.h"

#define ST_TO_F     -1


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
    HWND hp = nullptr;
    HWND hc = nullptr;
    HWND hq = nullptr;
    HWND ho = nullptr;

    switch (op)
    {
    case STO_Buy:
        tree = loc.LocInfo(LT_Buy).hwnd;
        item = loc.LocInfo(LT_Buy).hitem;
        hp = loc.LocInfo(LT_BuyPrice).hwnd;
        hc = loc.LocInfo(LT_BuyCode).hwnd;
        hq = loc.LocInfo(LT_BuyQuant).hwnd;
        ho = loc.LocInfo(LT_BuyOrder).hwnd;
        break;
    case STO_Sell:
        tree = loc.LocInfo(LT_Sell).hwnd;
        item = loc.LocInfo(LT_Sell).hitem;
        hp = loc.LocInfo(LT_SellPrice).hwnd;
        hc = loc.LocInfo(LT_SellCode).hwnd;
        hq = loc.LocInfo(LT_SellQuant).hwnd;
        ho = loc.LocInfo(LT_SellOrder).hwnd;
        break;
    default:
        return ST_TO_F;
    }

    CString target;
    DWORD id = loc.Target(target);

    HandlePtr process = MakeHandlePtr(::OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, id));
    if (!process)
        return ST_TO_F;

    if (loc.OpenTradePage(process, tree, item))
    {
        if (!this->SetText(process, hc, code))
            return ST_TO_F;

        if (!this->SetText(process, hp, price))
            return ST_TO_F;

        if (!this->SetText(process, hq, quant))
            return ST_TO_F;

        ::SendMessage(ho, BM_CLICK, 0, 0);

        // find the order info in delegate list ctrl
        HTREEITEM dlitem = loc.LocInfo(LT_Delegate).hitem;
        HWND dlst = loc.LocInfo(LT_DelegateList).hwnd;

        if (loc.OpenTradePage(process, tree, dlitem))
        {
            CString deleId(MAKEINTRESOURCE(IDS_DELE_ID));

            int row = (int)::SendMessage(dlst, LVM_GETITEMCOUNT, 0, 0);
            if (row > 0)
            {
                HWND header = (HWND)::SendMessage(dlst, LVM_GETHEADER, 0, 0);

                // TODO : move this to locate data, we don't have to find delegate id col every time
                int col = (int)::SendMessage(header, HDM_GETITEMCOUNT, 0, 0);
                if (col > 0)
                {
                    VirtualPtr pColItem = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(LVCOLUMN), MEM_COMMIT, PAGE_READWRITE));
                    VirtualPtr pColText = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, MEM_COMMIT, PAGE_READWRITE));
                    if (!pColItem || !pColText)
                        return ST_TO_F;

                    int c = 0;
                    for (; c < col; ++c)
                    {
                        TCHAR ctext[ST_MAX_VIRTUAL_BUF] = { 0 };

                        LVCOLUMN lvcItem = { 0 };
                        lvcItem.mask = LVCF_TEXT;
                        lvcItem.iSubItem = 0;
                        lvcItem.iOrder = c;
                        lvcItem.pszText = (LPTSTR)pColText.get();
                        lvcItem.cchTextMax = sizeof(ctext);

                        if (::WriteProcessMemory(process.get(), pColItem.get(), &lvcItem, sizeof(LVCOLUMN), nullptr))
                        {
                            if (::SendMessage(dlst, LVM_GETCOLUMN, c, (LPARAM)(pColItem.get())))
                            {
                                if (::ReadProcessMemory(process.get(), pColText.get(), &ctext[0], sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, nullptr))
                                {
                                    if (deleId == ctext)
                                        break;
                                }
                            }
                        }
                    }

                    if (c < col) // find the col id
                    {
                        VirtualPtr pRowItem = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE));
                        VirtualPtr pRowText = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, MEM_COMMIT, PAGE_READWRITE));
                        if (!pRowItem || !pRowText)
                            return ST_TO_F;

                        TCHAR rtext[ST_MAX_VIRTUAL_BUF] = { 0 };

                        LVITEM lvrItem = { 0 };
                        lvrItem.mask = LVIF_TEXT;
                        lvrItem.iSubItem = c;
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

                                }
                            }
                        }
                    }
                }
            }
        }

    }

    return ST_TO_F;
}

bool CTradeOrderManager::SetText(HandlePtr process, HWND hwnd, CString const & text) const
{
    bool res = false;

    if (!process)
        return res;

    SIZE_T size = sizeof(TCHAR) * (text.GetLength() + 1);
    VirtualPtr _t = MakeVirtualPtr(::VirtualAllocEx(process.get(), NULL, size, MEM_COMMIT, PAGE_READWRITE));
    if (!_t)
        return res;

    if (::WriteProcessMemory(process.get(), _t.get(), text.GetString(), size, NULL))
    {
        if (::SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)(_t.get())) == TRUE)
        {
            res = true;
        }
    }

    return res;
}
