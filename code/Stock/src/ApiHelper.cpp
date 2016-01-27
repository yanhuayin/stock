#include "stdafx.h"
#include "StockGlobal.h"
#include "Utils.h"
#include "ApiHelper.h"

#define ST_LIST_CTRL_NAME   _T("SysListView32")
#define ST_TREE_CTRL_NAME   _T("SysTreeView32")

#define ST_MAX_CLASS_NAME   256

typedef LRESULT(WINAPI *SendMessageFunc)(HWND, UINT, WPARAM, LPARAM);

struct TreeRemoteData
{
    HWND            hwnd;
    NMHDR           nmh;
    SendMessageFunc pfn;
};

struct ListRemoteData
{
    HWND            hwnd;
    NMITEMACTIVATE  nmi;
    SendMessageFunc pfn;
};

static DWORD WINAPI s_list_thread_func(LPVOID pData);
static void s_after_list_thread_func(void);

static DWORD WINAPI s_thread_fuc(LPVOID pData);
static void s_after_thread_fuc(void);

namespace
{
    static CString s_tree_ctrl_name(ST_TREE_CTRL_NAME);
    static CString s_list_ctrl_name(ST_LIST_CTRL_NAME);


    static SendMessageFunc sLoadSendMessageFunc()
    {
        static SendMessageFunc pfn = nullptr;

        if (!pfn)
        {
            HINSTANCE hUser32 = ::LoadLibrary(_T("User32.dll"));
            pfn = (SendMessageFunc)(::GetProcAddress(hUser32, "SendMessageW"));
        }

        return pfn;
    }
}

HWND WinApi::TopWndFromPoint(POINT pt)
{
    HWND unHwnd = ::WindowFromPoint(pt);

    HWND grayHwnd = ::GetWindow(unHwnd, GW_CHILD);
    RECT tempRc;
    BOOL bFind = FALSE;
    while (grayHwnd)
    {
        ::GetWindowRect(grayHwnd, &tempRc);
        if (::PtInRect(&tempRc, pt))
        {
            bFind = TRUE;
            break;
        }
        else
        {
            grayHwnd = ::GetWindow(grayHwnd, GW_HWNDNEXT);
        }
    }

    if (bFind == TRUE)
    {
        unHwnd = grayHwnd;
        bFind = FALSE;
    }

    return unHwnd;
}

HTREEITEM WinApi::SearchTreeItem(HandlePtr process, HWND tree, CString const & target, HTREEITEM parent)
{
    ASSERT(process);

    // make sure it's a real tree control
    TCHAR className[ST_MAX_CLASS_NAME];
    ::GetClassName(tree, className, ST_MAX_CLASS_NAME);
    if (s_tree_ctrl_name != className)
        return nullptr;

    VirtualPtr ttvi = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TVITEM), MEM_COMMIT, PAGE_READWRITE));
    if (!ttvi)
        return nullptr;

    VirtualPtr ttxt = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, MEM_COMMIT, PAGE_READWRITE));
    if (!ttxt)
        return nullptr;

    HTREEITEM item = TreeView_GetRoot(tree);
    if (parent)
        item = TreeView_GetChild(tree, parent);

    if (!item)
        return nullptr;

    do
    {
        TCHAR txtBuf[ST_MAX_VIRTUAL_BUF] = { 0 };

        TVITEM tvi = { 0 };
        tvi.mask = TVIF_TEXT | TVIF_HANDLE;
        tvi.pszText = (LPTSTR)ttxt.get();
        tvi.cchTextMax = ST_ARRAY_SIZE(txtBuf);
        tvi.hItem = item;

        if (!::WriteProcessMemory(process.get(), ttvi.get(), &tvi, sizeof(tvi), NULL))
        {
            item = nullptr;
            break;
        }

        if (!::SendMessage(tree, TVM_GETITEM, 0, (LPARAM)ttvi.get()))
        {
            item = nullptr;
            break;
        }

        if (!::ReadProcessMemory(process.get(), (LPCVOID)ttxt.get(), txtBuf, sizeof(txtBuf), NULL))
        {
            item = nullptr;
            break;
        }

        CString text(txtBuf);
        if (text.Find(target, 0) == 0) // find target
        {
            break;
        }

        item = TreeView_GetNextSibling(tree, item);

    } while (item);

    return item;
}

bool WinApi::SelectTreeItem(HandlePtr process, HWND tree, HTREEITEM item, POINT const& center)
{
    ::Sleep(ST_SLEEP_T);

    if (!::BlockInput(TRUE))
        return false;

    //::ShowCursor(FALSE);

    POINT cursor;
    if (!::GetCursorPos(&cursor))
        return false;

    if (!::SetCursorPos(center.x, center.y))
        return false;

    //HTREEITEM curr = TreeView_GetSelection(tree);

    //TreeView_SelectItem(tree, item);

    bool res = WinApi::NotifyTreeParent(process, tree, NM_CLICK);

    ::SetCursorPos(cursor.x, cursor.y);

    //::ShowCursor(TRUE);

    ::BlockInput(FALSE);

    //if (!res)
    //{
    //    if (curr)
    //        TreeView_SelectItem(tree, curr);
    //}


    return res;
}

bool WinApi::SelectListItem(HandlePtr process, HWND list, int item, int subItem, POINT const & pos)
{
    //::Sleep(ST_SLEEP_T);

    if (!::BlockInput(TRUE))
        return false;

    POINT cursor;
    if (!::GetCursorPos(&cursor))
        return false;

    if (!::SetCursorPos(pos.x, pos.y))
        return false;

    // doesn't work
    //bool res = WinApi::NotifyListParent(process, list, NM_CLICK, item, subItem, pos);
    POINT client = pos;
    if (!::ScreenToClient(list, &client))
        return false;

    ::PostMessage(list, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(client.x, client.y));
    ::Sleep(200);
    ::PostMessage(list, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(client.x, client.y));

    ::SetCursorPos(cursor.x, cursor.y);

    ::BlockInput(FALSE);

    return true;
}

bool WinApi::CacTreeItemCenter(HandlePtr process, HWND tree, HTREEITEM item, POINT & pos)
{
    VirtualPtr pRect = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(RECT), MEM_COMMIT, PAGE_READWRITE));
    if (!pRect)
        return false;

    RECT rect;
    ::ZeroMemory(&rect, sizeof(RECT));
    *(HTREEITEM*)&rect = item;

    if (!::WriteProcessMemory(process.get(), pRect.get(), &rect, sizeof(RECT), nullptr))
        return false;

    ::SendMessage(tree, TVM_GETITEMRECT, TRUE, (LPARAM)(pRect.get()));

    if (!::ReadProcessMemory(process.get(), pRect.get(), &rect, sizeof(RECT), nullptr))
        return false;

    pos.x = (rect.left + rect.right) / 2;
    pos.y = (rect.top + rect.bottom) / 2;

    ::ClientToScreen(tree, &pos);

    return true;
}

bool WinApi::CacListItemCenter(HandlePtr process, HWND list, int item, int subItem, int flag, POINT & pos)
{
    VirtualPtr pRect = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(RECT), MEM_COMMIT, PAGE_READWRITE));
    if (!pRect)
        return false;

    RECT rect;
    ::ZeroMemory(&rect, sizeof(RECT));
    rect.top = subItem + 1;
    rect.left = flag;

    if (!::WriteProcessMemory(process.get(), pRect.get(), &rect, sizeof(RECT), nullptr))
        return false;

    ::SendMessage(list, LVM_GETSUBITEMRECT, (WPARAM)item, (LPARAM)(pRect.get()));

    if (!::ReadProcessMemory(process.get(), pRect.get(), &rect, sizeof(RECT), nullptr))
        return false;

    pos.x = (rect.left + rect.right) / 2;
    pos.y = (rect.top + rect.bottom) / 2;

    ::ClientToScreen(list, &pos);

    return true;
}

bool WinApi::IsTreeItemExpanded(HWND tree, HTREEITEM item)
{
    if (TreeView_GetItemState(tree, item, TVIS_EXPANDED) & TVIS_EXPANDED)
        return true;

    return false;
}

bool WinApi::QueryListItemText(HandlePtr process, HWND list, int item, int subItem, LPTSTR lpBuff, SIZE_T bufSize, VirtualPtr pItem, VirtualPtr pText)
{
    if (!pItem)
        pItem = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE));
    if (!pText)
        pText = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, bufSize, MEM_COMMIT, PAGE_READWRITE));

    if (!pItem || !pText)
        return false;

    LVITEM lvrItem = { 0 };
    lvrItem.mask = LVIF_TEXT;
    lvrItem.iSubItem = subItem;
    lvrItem.pszText = (LPTSTR)pText.get();
    lvrItem.cchTextMax = bufSize;
    lvrItem.iItem = item;

    if (::WriteProcessMemory(process.get(), pItem.get(), &lvrItem, sizeof(LVITEM), nullptr))
    {
        int count = ::SendMessage(list, LVM_GETITEMTEXT, item, (LPARAM)(pItem.get()));
        if (count > 0)
        {
            if (::ReadProcessMemory(process.get(), pText.get(), lpBuff, sizeof(TCHAR)*count, nullptr))
            {
                return true;
            }
        }
    }
    
    return false;
}

bool WinApi::NotifyListParent(HandlePtr process, HWND list, UINT message, int item, int subItem, POINT const & pos)
{
    VirtualPtr pRemote = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(ListRemoteData), MEM_COMMIT, PAGE_READWRITE));
    if (!pRemote)
        return false;

    ListRemoteData remote;
    remote.hwnd = ::GetParent(list);
    remote.pfn = sLoadSendMessageFunc();
    remote.nmi.hdr.code = message;
    remote.nmi.hdr.idFrom = ::GetDlgCtrlID(list);
    remote.nmi.hdr.hwndFrom = list;
    remote.nmi.iItem = item;
    remote.nmi.iSubItem = subItem;
    remote.nmi.ptAction = pos;

    if (!::WriteProcessMemory(process.get(), pRemote.get(), &remote, sizeof(ListRemoteData), nullptr))
        return false;

    const SIZE_T codeSize = ((LPBYTE)s_after_list_thread_func - (LPBYTE)s_list_thread_func);
    ASSERT(codeSize == 48);

    VirtualPtr pThread = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, codeSize, MEM_COMMIT, PAGE_READWRITE));
    if (!pThread)
        return false;

    if (!::WriteProcessMemory(process.get(), pThread.get(), s_list_thread_func, codeSize, nullptr))
        return false;

    HANDLE hThread = ::CreateRemoteThread(process.get(), nullptr, 0, (LPTHREAD_START_ROUTINE)(pThread.get()), pRemote.get(), 0, nullptr);
    if (::WaitForSingleObject(hThread, ST_MAX_THREAD_TIME) != WAIT_OBJECT_0)
        return false;

    return true;
}

bool WinApi::NotifyTreeParent(HandlePtr process, HWND tree, UINT message)
{
    VirtualPtr pRemote = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TreeRemoteData), MEM_COMMIT, PAGE_READWRITE));
    if (!pRemote)
        return false;

    TreeRemoteData remote;
    remote.hwnd = ::GetParent(tree);
    remote.pfn = sLoadSendMessageFunc();
    remote.nmh.code = message;
    remote.nmh.idFrom = ::GetDlgCtrlID(tree);
    remote.nmh.hwndFrom = tree;

    if (!::WriteProcessMemory(process.get(), pRemote.get(), &remote, sizeof(TreeRemoteData), nullptr))
        return false;

    const SIZE_T codeSize = ((LPBYTE)s_after_thread_fuc - (LPBYTE)s_thread_fuc);
    ASSERT(codeSize == 48);

    VirtualPtr pThread = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, codeSize, MEM_COMMIT, PAGE_READWRITE));
    if (!pThread)
        return false;

    if (!::WriteProcessMemory(process.get(), pThread.get(), s_thread_fuc, codeSize, nullptr))
        return false;

    HANDLE hThread = ::CreateRemoteThread(process.get(), nullptr, 0, (LPTHREAD_START_ROUTINE)(pThread.get()), pRemote.get(), 0, nullptr);
    if (::WaitForSingleObject(hThread, ST_MAX_THREAD_TIME) != WAIT_OBJECT_0)
        return false;

    return true;
}

HandlePtr WinApi::QueryTargetName(HWND hwnd, CString & outName, DWORD & outId)
{
    DWORD id = outId;
    if (id == 0)
        ::GetWindowThreadProcessId(hwnd, &id);

    HandlePtr process = nullptr;
    if (id)
    {
        process = MakeHandlePtr(::OpenProcess(PROCESS_ALL_ACCESS, FALSE, id));
        if (process)
        {
            TCHAR buffer[MAX_PATH];
            DWORD size = MAX_PATH;
            if (::QueryFullProcessImageName(process.get(), 0, buffer, &size))
            {
                outName = buffer;
            }
            else
            {
                id = 0;
            }
        }
        else
        {
            id = 0;
        }
    }

    outId = id;
    return process;
}

// both thread function must be static or we need to disable incremental linking
// 
static DWORD WINAPI s_list_thread_func(LPVOID pData)
{
    ListRemoteData *p = (ListRemoteData*)pData;

    p->pfn(p->hwnd, WM_NOTIFY, (WPARAM)p->nmi.hdr.idFrom, (LPARAM)&(p->nmi));

    return 0;
}

static void s_after_list_thread_func(void)
{

}

static DWORD WINAPI s_thread_fuc(LPVOID pData)
{
    TreeRemoteData *p = (TreeRemoteData*)pData;

    p->pfn(p->hwnd, WM_NOTIFY, (WPARAM)p->nmh.idFrom, (LPARAM)&(p->nmh));

    return 0;
}

static void s_after_thread_fuc(void)
{

}

