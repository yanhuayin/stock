#include "stdafx.h"
#include <Psapi.h>
#include "resource.h"
#include "StockGlobal.h"
#include "Utils.h"
#include "StockConfig.h"
#include "StockLocateData.h"

namespace
{
    const TCHAR*    s_locate_names[] =
    {
        _T("toolbar"),

        _T("buy"),
        _T("buyCode"),
        _T("buyPrice"),
        _T("buyQuant"),
        _T("buyOrder"),

        _T("sell"),
        _T("sellCode"),
        _T("sellPrice"),
        _T("sellQuant"),
        _T("sellOrder"),

        _T("cancel"),
        _T("cancelBtn"),
        _T("cancelList"),

        _T("delegate"),
        _T("delegateList")
    };

    const LPTSTR    s_locate_order_col_names[] =
    {
        MAKEINTRESOURCE(IDS_LOCATE_ORDER_NAME),
        MAKEINTRESOURCE(IDS_LOCATE_ORDER_CODE),
        MAKEINTRESOURCE(IDS_LOCATE_ORDER_ID),
        MAKEINTRESOURCE(IDS_LOCATE_ORDER_PRICE),
        MAKEINTRESOURCE(IDS_LOCATE_ORDER_QUANT)
    };
}

#define ST_LOC_TARGET_NAME      _T("target")
#define ST_LOC_COMPONENT_NAME   _T("component")
#define ST_LOC_INFO_NAME        _T("name")
#define ST_LOC_ID_NAME          _T("id")
#define ST_LOC_POS_NAME         _T("pos")
#define ST_LOC_X_NAME           _T("x")
#define ST_LOC_Y_NAME           _T("y")

#define ST_LOC_LIST_CTRL_NAME   _T("SysListView32")
#define ST_LOC_TREE_CTRL_NAME   _T("SysTreeView32")
#define ST_LOC_MAX_CLASS_NAME   256

namespace
{
    static CString s_list_ctrl_name(ST_LOC_LIST_CTRL_NAME);
    static CString s_tree_ctrl_name(ST_LOC_TREE_CTRL_NAME);

    // TODO : remove these hard code
    static int s_tb_buy_cmd = 1001;
    static int s_tb_sell_cmd = 1002;
    static int s_tb_cancel_cmd = 1003;

    static CToolbarBtn s_tbs[] =
    {
        {
            MAKEINTRESOURCE(IDS_BUY),
            LT_Buy,
            s_tb_buy_cmd
        },
        {
            MAKEINTRESOURCE(IDS_SELL),
            LT_Sell,
            s_tb_sell_cmd
        },
        {
            MAKEINTRESOURCE(IDS_CANCLE),
            LT_Cancel,
            s_tb_cancel_cmd
        }
    };
}

bool CStockLocateData::Load(CString const & file)
{
    BOOL exists = FileExists(file);

    // clear info data everytime when loading
    this->ResetLocateInfo(m_info, LT_Num);
    this->ResetListCol(m_cancel, SOF_Num);

    if (exists)
    {
        CFile f;
        CFileException fexp;
        if (f.Open(file, CFile::modeRead, &fexp))
        {
            f.SeekToBegin();

            UINT len = (UINT)f.GetLength() + 1;
            char *buf = new char[len];
            len = f.Read(buf, len);

            if (len == 0)
            {
                m_load = true; // empty file
                return m_load;
            }

            buf[len] = '\0';

            RapidStringStream s(buf);
            RapidEncodeInputStream eis(s);

            RapidDocument doc;
            doc.ParseStream<rapidjson::kParseDefaultFlags | rapidjson::kParseStopWhenDoneFlag, RapidUTF8>(eis);

            ST_SAFE_DELETE_ARRAY(buf);

            if (!doc.HasParseError() && doc.IsObject())
            {
                // TODO : use rapidjson pointer instead
                if (doc.HasMember(ST_LOC_TARGET_NAME))
                {
                    RapidValue &targetVal = doc[ST_LOC_TARGET_NAME];
                    if (targetVal.IsString())
                    {
                        m_target = targetVal.GetString();
                    }
                    else
                    {
                        TRACE1("Warning: unrecognized field %s when parsing locate config\n", ST_LOC_TARGET_NAME);
                    }
                }

                if (doc.HasMember(ST_LOC_COMPONENT_NAME))
                {
                    RapidValue &compVal = doc[ST_LOC_COMPONENT_NAME];
                    if (compVal.IsArray())
                    {
                        size_t size = compVal.Size();
                        if (size > LT_Num)
                        {
                            size = LT_Num;
                            TRACE1("Warning: %s's entry num are more than expected, some entries will be ignored\n", ST_LOC_COMPONENT_NAME);
                        }

                        //int count = 0;
                        for (size_t i = 0; i < size; ++i)
                        {
                            RapidValue &infoVal = compVal[i];
                            if (infoVal.IsObject() &&
                                infoVal.HasMember(ST_LOC_INFO_NAME) &&
                                infoVal.HasMember(ST_LOC_ID_NAME) &&
                                infoVal.HasMember(ST_LOC_POS_NAME))
                            {
                                RapidValue &nameVal = infoVal[ST_LOC_INFO_NAME];
                                RapidValue &idVal = infoVal[ST_LOC_ID_NAME];
                                RapidValue &posVal = infoVal[ST_LOC_POS_NAME];

                                if (nameVal.IsString() &&
                                    idVal.IsInt() &&
                                    posVal.IsObject())
                                {
                                    CString name = nameVal.GetString();
                                    int id = idVal.GetInt();

                                    bool posErr = false;
                                    //HWND hwnd = nullptr;
                                    int x, y;
                                    if (posVal.HasMember(ST_LOC_X_NAME) &&
                                        posVal.HasMember(ST_LOC_Y_NAME))
                                    {
                                        RapidValue &xVal = posVal[ST_LOC_X_NAME];
                                        RapidValue &yVal = posVal[ST_LOC_Y_NAME];
                                        if (xVal.IsInt() && yVal.IsInt())
                                        {
                                            x = xVal.GetInt();
                                            y = yVal.GetInt();

                                            POINT pos;
                                            pos.x = x;
                                            pos.y = y;

                                            // we need switch the window then retrive the hwnd
                                            //hwnd = TopWndFromPoint(pos);
                                            //if (!::IsWindow(hwnd))
                                            //{
                                                posErr = true;
                                                //TRACE2("Warning: can not locate a window under the pos (x:%d,y%d)\n", x, y);
                                            //}
                                        }
                                        else
                                            posErr = true;
                                    }
                                    else
                                        posErr = true;

                                    if (posErr)
                                    {
                                        TRACE1("Warning: entry %s does not have valid pos info, it is ignored\n", name.GetString());
                                        continue;
                                    }

                                    bool fixed = false;
                                    if (id >= LT_Num)
                                    {
                                        // This should not happen, try to fix it
                                        id = this->FindIdByName(name);
                                        if (id >= LT_Num)
                                        {
                                            TRACE1("Warning: unrecorgnized entry %s, it is ignored\n", name.GetString());
                                            continue;
                                        }
                                        fixed = true;
                                    }

                                    if (!fixed)
                                    {
                                        // maybe use the wrong id?
                                        if (name != s_locate_names[id])
                                        {
                                            int new_id = this->FindIdByName(name);
                                            if (new_id < LT_Num && m_info[new_id].name.IsEmpty())
                                            {
                                                id = new_id;
                                                fixed = true;
                                                TRACE3("Warning: entry name %s does not match it's id %d, new id %d is used\n",
                                                    name.GetString(), id, new_id);
                                            }
                                            else
                                            {
                                                if (new_id >= LT_Num)
                                                {
                                                    TRACE1("Warning: unkowned entry %s, it is ignored\n", name.GetString());
                                                }
                                                else
                                                {
                                                    TRACE2("Warning: duplicated entry %s is found, entry with id %d is ignored\n",
                                                        name.GetString(),
                                                        id);
                                                }
                                                continue;
                                            }
                                        }
                                    }

                                    CLocateInfo &info = m_info[id];
                                    info.name = name;
                                    info.pos.x = x;
                                    info.pos.y = y;
                                    //info.hwnd = hwnd;
                                    //++count;
                                }
                                else
                                {
                                    TRACE2("Warning: unrecognized field %d when parsing field %s, entry is ignored\n", i, ST_LOC_COMPONENT_NAME);
                                }
                            }
                            else
                            {
                                TRACE2("Warning: unrecognized field %d when parsing field %s, entry is ignored\n", i, ST_LOC_COMPONENT_NAME);
                            }
                        }

                        //if (count == LT_Num)
                        //    m_ready = true;
                    }
                    else
                    {
                        TRACE1("Warning: unrecognized field %s when parsing locate config\n", ST_LOC_COMPONENT_NAME);
                    }
                }

                m_load = true;
                return m_load;
            }
            else
            {
                if (doc.HasParseError())
                    TRACE1("Failed parse locate file, error code: %d\n", doc.GetParseError());
                else
                    TRACE0("Failed parse locate file.\n");
            }

        }
        else
        {
            TCHAR err[256];
            fexp.GetErrorMessage(err, 256);
            TRACE2("Can't open file %s, error is: %s\n", file.GetString(), err);
        }
    }
    else
    {
        m_load = true;
        return m_load;
    }

    AfxMessageBox(IDS_LOAD_LOCATE_DATA_FAILED);

    return m_load;
}

bool CStockLocateData::Save(CString const & file)
{
    ASSERT(m_load);

    CFile f;
    CFileException fexp;

    if (f.Open(file, CFile::modeCreate | CFile::modeReadWrite, &fexp))
    {
        RapidDocument doc;
        RapidDocument::AllocatorType &a = doc.GetAllocator();
        RapidValue &root = doc.SetObject();

        if (!m_target.IsEmpty())
        {
            root.AddMember(RapidDocument::StringRefType(ST_LOC_TARGET_NAME), RapidDocument::StringRefType(m_target.GetString()), a);
        }

        RapidValue comps(rapidjson::kArrayType);
        for (int i = 0; i < LT_Num; ++i)
        {
            if (m_info[i].hwnd)
            {
                RapidValue info(rapidjson::kObjectType);
                info.AddMember(RapidDocument::StringRefType(ST_LOC_INFO_NAME), RapidDocument::StringRefType(m_info[i].name.GetString()), a);
                RapidValue pos(rapidjson::kObjectType);
                pos.AddMember(RapidDocument::StringRefType(ST_LOC_X_NAME), m_info[i].pos.x, a);
                pos.AddMember(RapidDocument::StringRefType(ST_LOC_Y_NAME), m_info[i].pos.y, a);
                info.AddMember(RapidDocument::StringRefType(ST_LOC_POS_NAME), pos, a);
                info.AddMember(RapidDocument::StringRefType(ST_LOC_ID_NAME), i, a);

                comps.PushBack(info, a);
            }
        }

        if (comps.Size())
        {
            root.AddMember(RapidDocument::StringRefType(ST_LOC_COMPONENT_NAME), comps, a);
        }

        RapidStringBuffer buff;
        RapidEncodeOutputStream eos(buff);
        RapidWriter writer(eos);

        doc.Accept(writer);

        f.Write(buff.GetString(), buff.GetSize());

        return true;
    }
    else
    {
        TCHAR err[256];
        fexp.GetErrorMessage(err, 256);
        TRACE2("Can't open file %s, error is: %s\n", file.GetString(), err);
    }

    AfxMessageBox(IDS_SAVE_LOCATE_DATA_FAILED);

    return false;
}

bool CStockLocateData::LocateWnd()
{
    if (m_load)
    {
        int count = 0;
        for (int i = 0; i < LT_Num; ++i)
        {
            if (!m_info[i].name.IsEmpty())
            {
                switch ((LocateType)i) // TODO : check the window type and process
                {
                case LT_App:
                    m_info[i].hwnd = this->PointToTopWnd(m_info[i].pos);
                    if (m_info[i].hwnd)
                        ++count;
                    break;
                case LT_Toolbar:
                    m_info[i].hwnd = this->PointToTopWnd(m_info[i].pos);
                    if (m_info[i].hwnd)
                    {
                        size_t size = ST_ARRAY_SIZE(s_tbs);
                        this->GetToolbarBtn(m_info[i].hwnd, m_tID, m_target, s_tbs, size);
                        for (int i = 0; i < size; ++i)
                        {
                            if (s_tbs[i].cmd != -1)
                            {
                                m_info[s_tbs[i].cId].cmd = s_tbs[i].cmd;
                            }
                        }

                        ++count;
                    }
                    break;
                case LT_Buy:
                case LT_Sell:
                case LT_Cancel:
                case LT_Delegate:
                    m_info[i].hwnd = this->PointToTopWnd(m_info[i].pos);
                    if (m_info[i].hwnd)
                    {
                        m_info[i].hitem = this->SelectTreeItem(m_info[i].hwnd, (LocateType)i, m_tID);
                        if (m_info[i].hitem)
                            ++count;
                        else
                            m_info[i].hwnd = nullptr;
                    }
                    break;
                case LT_BuyCode:
                case LT_BuyPrice:
                case LT_BuyQuant:
                case LT_BuyOrder:
                    if (m_info[LT_Buy].hitem)
                    {
                        m_info[i].hwnd = this->PointToTopWnd(m_info[i].pos);
                        if (m_info[i].hwnd)
                            ++count;
                    }
                    break;
                case LT_SellCode:
                case LT_SellPrice:
                case LT_SellQuant:
                case LT_SellOrder:
                    if (m_info[LT_Sell].hitem)
                    {
                        m_info[i].hwnd = this->PointToTopWnd(m_info[i].pos);
                        if (m_info[i].hwnd)
                            ++count;
                    }
                    break;
                case LT_CancelBtn:
                case LT_CancelList:
                    if (m_info[LT_Cancel].hitem)
                    {
                        m_info[i].hwnd = this->PointToTopWnd(m_info[i].pos);
                        if (m_info[i].hwnd)
                        {
                            for (int i = 0; i < SOF_Num; ++i)
                            {
                                m_cancel[i].name = this->ListColName((StockOrderField)i);
                            }

                            HandlePtr process = MakeHandlePtr(::OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, m_tID));
                            if (process)
                            {
                                if (this->GetListCol(process, m_info[i].hwnd, m_cancel, SOF_Num) == SOF_Num)
                                    ++count;
                                else
                                    m_info[i].hwnd = nullptr;
                            }
                            else
                                m_info[i].hwnd = nullptr;
                        }
                    }
                    break;
                case LT_DelegateList:
                    if (m_info[LT_Delegate].hitem)
                    {
                        // ignore delegate list here, because we have no way to open the delegate page
                        //m_info[i].hwnd = this->PointToTopWnd(m_info[i].pos);
                        //if (m_info[i].hwnd)
                            ++count;
                    }
                    break;
                default:
                    break;
                }
            }
        }

        if (count == LT_Num)
            m_ready = true;

        return true;
    }

    return false;
}

CListCol const & CStockLocateData::ListCol(LocateType listType, StockOrderField col) const
{
    ASSERT(listType == LT_CancelList || listType == LT_DelegateList);

    if (listType == LT_CancelList)
    {
        return m_cancel[col];
    }
    else
    {
        return m_delegate[col];
    }
}

const LPTSTR CStockLocateData::ListColName(StockOrderField col) const
{
    return s_locate_order_col_names[col];
}

void CStockLocateData::SetListCol(LocateType listType, StockOrderField col, CListCol const & c)
{
    ASSERT(listType == LT_CancelList || listType == LT_DelegateList);
    if (listType == LT_CancelList)
    {
        m_cancel[col] = c;
    }
    else
    {
        m_delegate[col] = c;
    }
}

void CStockLocateData::SetInfo(LocateType type, POINT const& pos, HWND hwnd, HTREEITEM hitem, int cmd)
{
    // Note: you must call ValidateWnd before you set info
    m_info[type].pos = pos;
    m_info[type].hwnd = hwnd;
    m_info[type].hitem = hitem;
    m_info[type].cmd = cmd;

    if (m_info[type].name.IsEmpty())
    {
        m_info[type].name = s_locate_names[type];
    }
}

int CStockLocateData::FindIdByName(CString const & name) const
{
    int i = 0;
    for (; i < LT_Num; ++i)
    {
        if (name == s_locate_names[i])
            return i;
    }

    return LT_Num;
}

HTREEITEM CStockLocateData::SelectTreeItem(HWND tree, LocateType type, DWORD pId, bool open) const
{
    //process id must be a valid value

    // make sure it's a real tree control
    TCHAR className[MAX_CLASS_NAME];
    ::GetClassName(tree, className, MAX_CLASS_NAME);
    if (s_tree_ctrl_name != className)
        return nullptr;

    HandlePtr process = MakeHandlePtr(::OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pId));

    if (!process)
        return nullptr;


    VirtualPtr ttvi = MakeVirtualPtr(::VirtualAllocEx(process.get(), NULL, sizeof(TVITEM), MEM_COMMIT, PAGE_READWRITE));
    if (!ttvi)
        return nullptr;

    VirtualPtr ttxt = MakeVirtualPtr(::VirtualAllocEx(process.get(), NULL, sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, MEM_COMMIT, PAGE_READWRITE));
    if (!ttxt)
        return nullptr;

    CString target;
    CString parent;
    switch (type)
    {
    case LT_Buy:
        target.LoadString(IDS_BUY);
        break;
    case LT_Sell:
        target.LoadString(IDS_SELL);
        break;
    case LT_Cancel:
        target.LoadString(IDS_CANCLE);
        break;
    case LT_Delegate:
        parent.LoadString(IDS_QUERY);
        target.LoadString(IDS_DELEGATE);
        break;
    default:
        break;
    }

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

        if (!parent.IsEmpty())
        {
            if (parent == txtBuf) // find parent
            {
                item = TreeView_GetChild(tree, item);
                parent.Empty();
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

    if (item && open)
    {
        if (!this->OpenTradePage(process, tree, item, m_info[type].key, m_info[type].cmd))
            item = nullptr;
    }

    return item;
}

HWND CStockLocateData::PointToTopWnd(POINT const & pos)
{
    HWND hwnd = ::TopWndFromPoint(pos); 

    if (::IsWindow(hwnd))
    {
        // if target has not been set, then set it
        if (m_target.IsEmpty())
        {
            m_tID = this->QueryTargetName(hwnd, m_target, 0);
            if (m_tID)
                return hwnd;

            return nullptr;
        }

        hwnd = this->ValidateTopWnd(hwnd, m_target, m_tID);

        if (m_tID == 0 && hwnd) // this happpens when load target from config file
        {
            ::GetWindowThreadProcessId(hwnd, &m_tID);
        } // if non control loaded from config file match the loaded target then m_tID still can be 0
    }
    
    return nullptr;
}

DWORD CStockLocateData::QueryTargetName(HWND hwnd, CString & outName, DWORD pId) const
{
    // hwnd must be valid
    DWORD id = pId;
    if (id == 0)
        ::GetWindowThreadProcessId(hwnd, &id);

    if (id)
    {
        HANDLE process = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, id);
        if (process)
        {
            TCHAR buffer[MAX_PATH];
            DWORD size = MAX_PATH;
            if (::QueryFullProcessImageName(process, 0, buffer, &size))
            {
                outName = buffer;
            }
            else
            {
                id = 0;
            }

            ::CloseHandle(process);
        }
        else
        {
            id = 0;
        }
    }

    return id;
}

int CStockLocateData::GetToolbarBtn(HWND hwnd, DWORD pId, CString const& pTarget, CToolbarBtn * info, size_t cnt) const
{
    HandlePtr process = MakeHandlePtr(::OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pId));
    if (!process)
        return 0;

    HMODULE hModule = ::GetModuleHandle(pTarget);
    if (!hModule)
        return 0;

    VirtualPtr pName = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, MEM_COMMIT, PAGE_READWRITE));
    VirtualPtr pBtn = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TBBUTTON), MEM_COMMIT, PAGE_READWRITE));

    if (!pName || !pBtn)
        return 0;

    int count = 0;
    size_t num = ::SendMessage(hwnd, TB_BUTTONCOUNT, 0, 0);
    for (size_t i = 0; i < num; ++i)
    {
        TCHAR txtBuf[ST_MAX_VIRTUAL_BUF] = { 0 };
        TBBUTTON btn = { 0 };
        if (!::WriteProcessMemory(process.get(), pBtn.get(), &btn, sizeof(TBBUTTON), nullptr))
            continue;
        if (::SendMessage(hwnd, TB_GETBUTTON, i, (LPARAM)(pBtn.get()) == FALSE))
            continue;
        if (!::ReadProcessMemory(process.get(), pBtn.get(), &btn, sizeof(TBBUTTON), nullptr))
            continue;

        if (IS_INTRESOURCE(btn.iString))
        {
            if (::LoadString(hModule, btn.iString, (LPTSTR)pName.get(), ST_MAX_VIRTUAL_BUF))
            {
                if (!::ReadProcessMemory(process.get(), pName.get(), txtBuf, sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, nullptr))
                    continue;
            }
            else
                continue;
        }
        else
        {
            if (!::ReadProcessMemory(process.get(), (LPVOID)btn.iString, txtBuf, sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, nullptr))
                continue;
        }

        for (int j = 0; j < cnt; ++j)
        {
            if (CString(info[j].name) == txtBuf)
            {
                info[j].cmd = btn.idCommand;
                ++count;
                break;
            }
        }
    }

    return count;
}

int CStockLocateData::GetListCol(HandlePtr process, HWND list, CListCol *info, size_t cnt) const
{
    int count = 0;

    HWND header = (HWND)::SendMessage(list, LVM_GETHEADER, 0, 0);

    int col = (int)::SendMessage(header, HDM_GETITEMCOUNT, 0, 0);
    if (col > 0)
    {
        VirtualPtr pColItem = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(LVCOLUMN), MEM_COMMIT, PAGE_READWRITE));
        VirtualPtr pColText = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, MEM_COMMIT, PAGE_READWRITE));
        if (!pColItem || !pColText)
            return 0;

        for (int i = 0; i < cnt; ++i)
        {
            for (int c = 0; c < col; ++c)
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
                    if (::SendMessage(list, LVM_GETCOLUMN, c, (LPARAM)(pColItem.get())))
                    {
                        if (::ReadProcessMemory(process.get(), pColText.get(), &ctext[0], sizeof(TCHAR) * ST_MAX_VIRTUAL_BUF, nullptr))
                        {
                            if (info[i].name == ctext)
                            {
                                info[i].col = c;
                                ++count;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    return count;
}

void CStockLocateData::ResetListCol(CListCol *col, size_t size) const
{
    for (size_t i = 0; i < size; ++i)
    {
        col[i].col = -1;
    }
}

void CStockLocateData::ResetLocateInfo(CLocateInfo * info, size_t size) const
{
    for (int i = 0; i < LT_Num; ++i)
    {
        info[i].hwnd = nullptr;
        info[i].hitem = nullptr;
        info[i].cmd = -1;
        switch (i)
        {
        case LT_Buy:
            info[i].key = VK_F1; // hard code here, I don't think there is any common way to do this
            break;
        case LT_Sell:
            info[i].key = VK_F2;
            break;
        case LT_Cancel:
            info[i].key = VK_F3;
            break;
        default:
            info[i].key = -1;
            break;
        }
    }
}

HWND CStockLocateData::ValidateTopWnd(HWND hwnd, CString const& t, DWORD pId) const
{
    // we can make sure t has a valid value or loaded from file here
    // but pId still can be 0

    DWORD id;
    ::GetWindowThreadProcessId(hwnd, &id);

    if (id == 0)
        return nullptr;

    // does this window belong to a valid target?
    if (pId != 0)
    {
        if (id != pId)
            hwnd = nullptr;
    }
    else
    {
        CString target;
        this->QueryTargetName(hwnd, target, id);

        if (!target.IsEmpty())
        {
            if (target != t)
                hwnd = nullptr;
        }
        else
        {
            hwnd = nullptr;
        }
    }


    return hwnd;
}

bool CStockLocateData::ValidateHwnd(HWND hwnd, LocateType type, CString &target, DWORD &pId, HTREEITEM * hitem) const
{
    if (::IsWindow(hwnd))
    {
        bool isTSet = false;
        if (target.IsEmpty())
        {
            pId = this->QueryTargetName(hwnd, target, 0);
            if (pId == 0)
                return false;
            
            isTSet = true;
        }
        else
        {
            hwnd = this->ValidateTopWnd(hwnd, target, pId);
            if (!hwnd)
                return false;

            if (pId == 0)
            {
                ::GetWindowThreadProcessId(hwnd, &pId);
            }
        }

        switch (type)
        {
        case LT_Buy:
        case LT_Sell:
        case LT_Cancel:
        case LT_Delegate:
            *hitem = this->SelectTreeItem(hwnd, type, pId, false);
            if (*hitem) //though it's in valid top wnd but may not a tree ctrl
                return true;
            if (isTSet)
                target.Empty();
        default:
            return true;
        }
    }

    return false;

}

// all these are fucking bullshit. At first they said use the tree to open the trade page
// but turn out we cannot send the notify message cross the process
// then try to use the toolbar instead of the tree to open the page
// finally they choose to use a different version of the software which support F1/F2/F3 as the shotcut to open the tade page
// I dont have time to clean up these fucking code 
bool CStockLocateData::OpenTradePage(HandlePtr process, HWND tree, HTREEITEM item, int key, int cmd) const 
{

    // TreeView_SelectItem(tree, item);

    // fuck, we still need to send a click message to the tree ctrl
    //RECT rc;
    //::ZeroMemory(&rc, sizeof(rc));
    //*(HTREEITEM*)&rc = item;

    //VirtualPtr _trc = MakeVirtualPtr(::VirtualAllocEx(process.get(), NULL, sizeof(rc), MEM_COMMIT, PAGE_READWRITE));
    //// TODO : handle error
    //::WriteProcessMemory(process.get(), _trc.get(), &rc, sizeof(rc), NULL);
    //// TODO : handle error
    //::SendMessage(tree, TVM_GETITEMRECT, TRUE, (LPARAM)_trc.get());
    //// TODO : handle error
    //::ReadProcessMemory(process.get(), _trc.get(), &rc, sizeof(rc), NULL);
    //// TODO : handle error

    //::PostMessage(tree, WM_LBUTTONDOWN, 0, MAKELPARAM(rc.left, rc.top));
    //::PostMessage(tree, WM_LBUTTONUP, 0, MAKELPARAM(rc.left, rc.top));

    //POINT pt;
    //pt.x = rc.left;
    //pt.y = rc.top;

    //::ClientToScreen(tree, &pt);

    //POINT cpt;
    //::GetCursorPos(&cpt);

    //mouse_event(MOUSEEVENTF_MOVE, (pt.x - cpt.x), (pt.y - cpt.y), 0, 0);
    //mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

    // try this, SendNotifyMessage to tree's parent (check with spy) with NM_CLICK
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms644953%28v=vs.85%29.aspx
    // https://msdn.microsoft.com/en-us/library/windows/desktop/bb773466%28v=vs.85%29.aspx
    // https://msdn.microsoft.com/en-us/library/windows/desktop/bb775583%28v=vs.85%29.aspx
    // https://msdn.microsoft.com/en-us/library/windows/desktop/bb775583%28v=vs.85%29.aspx
    // looks like
    //NMHDR nmhdr;
    //
    //memset(&nmhdr, 0, sizeof(NMHDR));
    //nmhdr.hwndFrom = tree;
    //nmhdr.idFrom = ::GetDlgCtrlId(tree);
    //nmhdr.code = NM_CLICK;
    //SendMessage(???, WM_NOTIFY, (WPARAM)::GetDlgCtrlId(tree), (LPARAM)&nmhdr);

    //return false;

    //HWND target = ::GetParent(tree);
    //if (!target)
    //    return false;

    //VirtualPtr pNMHDR = MakeVirtualPtr(::VirtualAllocEx(process.get(), nullptr, sizeof(NMHDR), MEM_COMMIT, PAGE_READWRITE));

    //if (!pNMHDR)
    //    return false;

    //NMHDR nmh = { 0 };
    //nmh.code = NM_CLICK;
    //nmh.idFrom = ::GetDlgCtrlID(tree);
    //nmh.hwndFrom = tree;

    //if (::WriteProcessMemory(process.get(), pNMHDR.get(), &nmh, sizeof(NMHDR), nullptr))
    //{
    //    ::SendMessage(target, WM_NOTIFY, nmh.idFrom, (LPARAM)(pNMHDR.get()));
    //    return true;
    //}


    //HWND t = (HWND)(0x10518);
    //HWND t = (HWND)(0x10512);
    //HWND t2 = (HWND)(0x10518);
    //if (::IsWindow(t))
    //{
        //::SendMessage(t, WM_COMMAND, 10001, 0);

        //::SendMessage(t, (WM_USER + 0), 0, 0);


        //::SendMessage(t2, (WM_USER + 5101), 0, 0);
        //::SendMessage(t2, (WM_USER + 5103), 0, 0);
        //::SendMessage(t2, (WM_USER + 5104), 0, 0);

        //::SendMessage(t, (WM_USER + 5103), 0, 0);


        //::SendMessage(t, (WM_USER + 5101), 0, 0);
        //DWORD err = ::GetLastError();
        //TRACE1("%u", err);
        
        //::SendMessage(t, (WM_USER + 5103), 0, 0);


        //TRACE1("%u", err);
        
        //::SendMessage(t, (WM_USER + 5104), 0, 0);
        //TRACE1("%u", err);
    //}

    if (m_info[LT_App].hwnd && key != -1)
    {
        ::SendMessage(m_info[LT_App].hwnd, WM_KEYDOWN, key, 0);
    }
    else if (m_info[LT_Toolbar].hwnd && cmd != -1)
    {
        // TODO : could we just send to toolbar?
        HWND p1 = ::GetParent(m_info[LT_Toolbar].hwnd);
        HWND p2 = ::GetParent(p1);

        ::SendMessage(p2, WM_COMMAND, cmd, 0);

        return true;
    }

    
    return false;
}
