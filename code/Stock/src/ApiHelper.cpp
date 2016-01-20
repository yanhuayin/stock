#include "stdafx.h"
#include "StockGlobal.h"
#include "Utils.h"
#include "ApiHelper.h"

#define ST_LIST_CTRL_NAME   _T("SysListView32")
#define ST_TREE_CTRL_NAME   _T("SysTreeView32")

#define ST_MAX_CLASS_NAME   256

typedef LRESULT(WINAPI *SendMessageFunc)(HWND, UINT, WPARAM, LPARAM);

struct RemoteData
{
    HWND            hwnd;
    NMHDR           nmh;
    SendMessageFunc pfn;
};

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

HTREEITEM WinApi::SearchTreeItem(HandlePtr process, HWND tree, CString const & target, CString const & parent)
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

    bool serParent = !parent.IsEmpty();

    HTREEITEM item = TreeView_GetRoot(tree);
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

        if (serParent)
        {
            if (parent == txtBuf) // find parent
            {
                item = TreeView_GetChild(tree, item);
                serParent = false;
                continue;
            }

            item = TreeView_GetNextSibling(tree, item);
            continue;
        }

        if (target == txtBuf) // find target
        {
            break;
        }

        item = TreeView_GetNextSibling(tree, item);

    } while (item);

    return item;
}

bool WinApi::SelectTreeItem(HandlePtr process, HWND tree, HTREEITEM item, POINT const& center)
{
    POINT cursor;
    if (!::GetCursorPos(&cursor))
        return false;

    if (!::SetCursorPos(center.x, center.y))
        return false;

    //::ShowCursor(FALSE);
    ::BlockInput(TRUE);

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

//bool WinApi::CacTreeItemCenter(HandlePtr process, HWND tree, HTREEITEM item, POINT & pos)
bool WinApi::CacTreeItemCenter(HWND tree, HTREEITEM item, POINT & pos)
{
    // TODO : do we have to alloc memory on the specified process?
    //VirtualPtr pRect = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(RECT), MEM_COMMIT, PAGE_READWRITE));
    //if (!pRect)
    //    return false;

    //if (TreeView_GetItemRect(tree, item, pRect.get(), TRUE))
    //{
    //    RECT rect;
    //    if (::ReadProcessMemory(process.get(), pRect.get(), &rect, sizeof(RECT), nullptr))
    //    {
    //        pos.x = (rect.left + rect.right) / 2;
    //        pos.y = (rect.top + rect.bottom) / 2;

    //        ::ClientToScreen(tree, &pos);

    //        return true;
    //    }
    //}

    RECT rect;
    if (TreeView_GetItemRect(tree, item, &rect, TRUE))
    {
        pos.x = (rect.left + rect.right) / 2;
        pos.y = (rect.top + rect.bottom) / 2;

        if (::ClientToScreen(tree, &pos))
        {
            return true;
        }
    }

    return false;
}

bool WinApi::NotifyTreeParent(HandlePtr process, HWND tree, UINT message)
{
    VirtualPtr pRemote = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(RemoteData), MEM_COMMIT, PAGE_READWRITE));
    if (!pRemote)
        return false;

    RemoteData remote;
    remote.hwnd = ::GetParent(tree);
    remote.pfn = sLoadSendMessageFunc();
    remote.nmh.code = message;
    remote.nmh.idFrom = ::GetDlgCtrlID(tree);
    remote.nmh.hwndFrom = tree;

    if (!::WriteProcessMemory(process.get(), pRemote.get(), &remote, sizeof(RemoteData), nullptr))
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
static DWORD WINAPI s_thread_fuc(LPVOID pData)
{
    RemoteData *p = (RemoteData*)pData;

    p->pfn(p->hwnd, WM_NOTIFY, (WPARAM)p->nmh.idFrom, (LPARAM)&(p->nmh));

    return 0;
}

static void s_after_thread_fuc(void)
{

}

