#include "stdafx.h"
#include "StockGlobal.h"
#include "Utils.h"
#include "ApiHelper.h"

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
    static CString s_tree_ctrl_name(ST_LOC_TREE_CTRL_NAME);

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

HTREEITEM WinApi::SelectTreeItem(HandlePtr process, HWND tree, bool click, CString const & target, CString const & parent)
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

    if (item && click)
    {
        VirtualPtr pRemote = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(RemoteData), MEM_COMMIT, PAGE_READWRITE));
        if (pRemote)
            return nullptr;

        RemoteData remote;
        remote.hwnd = ::GetParent(tree);
        remote.pfn = sLoadSendMessageFunc();
        remote.nmh.code = NM_CLICK;
        remote.nmh.idFrom = ::GetDlgCtrlID(tree);
        remote.nmh.hwndFrom = tree;

        if (!::WriteProcessMemory(process.get(), pRemote.get(), &remote, sizeof(RemoteData), nullptr))
            return nullptr;

        const SIZE_T codeSize = ((LPBYTE)s_after_thread_fuc - (LPBYTE)s_thread_fuc);
        // TODO : check the size

        VirtualPtr pThread = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, codeSize, MEM_COMMIT, PAGE_READWRITE));
        if (!pThread)
            return nullptr;

        if (!::WriteProcessMemory(process.get(), pThread.get(), s_thread_fuc, codeSize, nullptr))
            return nullptr;

        HANDLE hThread = ::CreateRemoteThread(process.get(), nullptr, 0, (LPTHREAD_START_ROUTINE)(pThread.get()), pRemote.get(), 0, nullptr);
        if (::WaitForSingleObject(hThread, ST_MAX_THREAD_TIME) != WAIT_OBJECT_0)
            return nullptr;
    }

    return item;
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

