#include "stdafx.h"
#include <Psapi.h>
#include "resource.h"
#include "StockGlobal.h"
#include "Utils.h"
#include "StockConfig.h"
#include "StockLocateData.h"
#include "TradeControl.h"

namespace
{
    const TCHAR*    s_locate_names[] =
    {
        _T("App"),
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

        _T("query"),
        _T("delegate"),
        _T("delegateList")
    };

    const LPTSTR    s_locate_order_col_names[] =
    {
        MAKEINTRESOURCE(IDS_LOCATE_ORDER_TIME),
        MAKEINTRESOURCE(IDS_LOCATE_ORDER_CODE),
        MAKEINTRESOURCE(IDS_LOCATE_ORDER_NAME),
        MAKEINTRESOURCE(IDS_LOCATE_ORDER_TRADE_FLAG),
        MAKEINTRESOURCE(IDS_LOCATE_ORDER_PRICE),
        MAKEINTRESOURCE(IDS_LOCATE_ORDER_QUANT),
        MAKEINTRESOURCE(IDS_LOCATE_ORDER_ID),
        MAKEINTRESOURCE(IDS_LOCATE_ORDER_TURNOVER_QUANT)
    };
}

#define ST_LOC_TARGET_NAME      _T("target")
#define ST_LOC_COMPONENT_NAME   _T("component")
#define ST_LOC_INFO_NAME        _T("name")
#define ST_LOC_ID_NAME          _T("id")
#define ST_LOC_POS_NAME         _T("pos")
#define ST_LOC_X_NAME           _T("x")
#define ST_LOC_Y_NAME           _T("y")

#define ST_LOC_MAX_CLASS_NAME   256

bool CStockLocateData::Load(CString const & file)
{
    BOOL exists = WinApi::FileExists(file);

    for (int i = 0; i < LT_Num; ++i)
    {
        this->ResetLocateInfo(m_info[i], (LocateType)i);
    }

    for (int i = 0; i < SOF_Num; ++i)
    {
        this->ResetListCol(m_cancel[i], (StockOrderField)i);
        this->ResetListCol(m_delegate[i], (StockOrderField)i);
    }

    m_tSet = m_pSet = true;
    this->Withdraw();

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

                                            //POINT pos;
                                            //pos.x = x;
                                            //pos.y = y;

                                            // we need switch the window then retrive the hwnd
                                            //hwnd = TopWndFromPoint(pos);
                                            //if (!::IsWindow(hwnd))
                                            //{
                                                //posErr = true;
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
                                            if (new_id < LT_Num)
                                            {
                                                id = new_id;
                                                fixed = true;
                                                TRACE3("Warning: entry name %s does not match it's id %d, new id %d is used\n",
                                                    name.GetString(), id, new_id);
                                            }
                                            else
                                            {
                                                TRACE1("Warning: unkowned entry %s, it is ignored\n", name.GetString());
                                                continue;
                                            }
                                        }
                                    }

                                    LocateInfo &info = m_info[id];
                                    //info.name = name;
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
                case LT_Buy:
                case LT_Sell:
                case LT_Cancel:
                case LT_Query:
                    m_info[i].hwnd = this->PointToTopWnd(m_info[i].pos);
                    if (m_info[i].hwnd)
                    {
                        m_info[i].hitem = this->SearchTreeItem(m_process, m_info[i].hwnd, (LocateType)i);
                        if (m_info[i].hitem)
                        {
                            POINT pos;
                            if (WinApi::CacTreeItemCenter(m_process, m_info[i].hwnd, m_info[i].hitem, pos))
                            {
                                if (WinApi::IsTreeItemExpanded(m_info[i].hwnd, m_info[i].hitem) ||
                                    WinApi::SelectTreeItem(m_process, m_info[i].hwnd, m_info[i].hitem, pos))
                                {
                                    m_info[i].pos = pos;
                                    ++count;
                                    break;
                                }
                            }
                        }

                        this->Withdraw();
                        m_info[i].hitem = nullptr;
                        m_info[i].hwnd = nullptr;
                    }
                    break;
                case LT_Delegate:
                    if (m_info[LT_Query].hitem)
                    {
                        m_info[i].hwnd = this->PointToTopWnd(m_info[i].pos);
                        if (m_info[i].hwnd)
                        {
                            m_info[i].hitem = this->SearchTreeItem(m_process, m_info[i].hwnd, (LocateType)i, m_info[LT_Query].hitem);
                            if (m_info[i].hitem)
                            {
                                POINT pos;
                                if (WinApi::CacTreeItemCenter(m_process, m_info[i].hwnd, m_info[i].hitem, pos))
                                {
                                    if (WinApi::SelectTreeItem(m_process, m_info[i].hwnd, m_info[i].hitem, pos))
                                    {
                                        m_info[i].pos = pos;
                                        ++count;
                                        break;
                                    }
                                }
                            }

                            this->Withdraw();
                            m_info[i].hitem = nullptr;
                            m_info[i].hwnd = nullptr;
                        }
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
                            if (i == LT_CancelList)
                            {
                                if (this->GetListCol(m_process, m_info[i].hwnd, m_cancel, SOF_Num) == SOF_Num)
                                    ++count;
                                else
                                {
                                    this->Withdraw();
                                    m_info[i].hwnd = nullptr;
                                }
                            }
                            else
                            {
                                ++count;
                            }
                        }
                    }
                    break;
                case LT_DelegateList:
                    if (m_info[LT_Delegate].hitem)
                    {
                        m_info[i].hwnd = this->PointToTopWnd(m_info[i].pos);
                        if (m_info[i].hwnd)
                        {
                            if (this->GetListCol(m_process, m_info[i].hwnd, m_delegate, SOF_Num) == SOF_Num)
                                ++count;
                            else
                            {
                                this->Withdraw();
                                m_info[i].hwnd = nullptr;
                            }
                        }
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

ListViewColumn const& CStockLocateData::ListCol(LocateType listType, StockOrderField col) const
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

void CStockLocateData::SetListCol(LocateType listType, StockOrderField col, ListViewColumn const & c)
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

void CStockLocateData::SetInfo(LocateType type, LocateInfo const& i)
{
    ASSERT(m_info[type].name == i.name);

    m_info[type].pos = i.pos;
    m_info[type].hwnd = i.hwnd;
    m_info[type].hitem = i.hitem;
}

void CStockLocateData::SetReady(bool ready)
{
    m_ready = ready;
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

HTREEITEM CStockLocateData::SearchTreeItem(HandlePtr process, HWND tree, LocateType type, HTREEITEM parent) const
{

    CString target;
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
    case LT_Query:
        target.LoadString(IDS_QUERY);
        break;
    case LT_Delegate:
        target.LoadString(IDS_DELEGATE);
        break;
    default:
        break;
    }

    return WinApi::SearchTreeItem(process, tree, target, parent);
}

HWND CStockLocateData::PointToTopWnd(POINT const & pos)
{
    m_tSet = false;
    m_pSet = false;

    HWND hwnd = WinApi::TopWndFromPoint(pos); 

    if (::IsWindow(hwnd))
    {
        // if target has not been set, then set it
        if (m_target.IsEmpty())
        {
            m_process = WinApi::QueryTargetName(hwnd, m_target, m_tID);
            if (m_tID)
            {
                m_tSet = true;
                m_pSet = true;
                return hwnd;
            }

            return nullptr;
        }

        hwnd = this->ValidateTopWnd(hwnd);

        return hwnd;
    }
    
    return nullptr;
}

int CStockLocateData::GetListCol(HandlePtr process, HWND list, ListViewColumn *info, size_t cnt) const
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

        for (size_t i = 0; i < cnt; ++i)
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

    if (count != cnt)
    {
        for (size_t i = 0; i < cnt; ++i)
        {
            info[i].col = -1;
        }
    }

    return count;
}

void CStockLocateData::ResetListCol(ListViewColumn &col, StockOrderField field) const
{
    col.col = -1;
    col.name = CString(s_locate_order_col_names[field]);
}

void CStockLocateData::ResetLocateInfo(LocateInfo &info, LocateType type) const
{
    info.hwnd = nullptr;
    info.hitem = nullptr;
    info.name = CString(s_locate_names[type]);
    info.pos = { 0, 0 };
}

HWND CStockLocateData::ValidateTopWnd(HWND hwnd)
{
    // we can make sure m_target has a valid value or loaded from file here
    // but m_tID still can be 0

    DWORD id;
    ::GetWindowThreadProcessId(hwnd, &id);

    if (id == 0)
        return nullptr;

    // does this window belong to a valid target?
    if (m_tID != 0)
    {
        if (id != m_tID)
            hwnd = nullptr;
    }
    else
    {
        DWORD id = 0;
        CString target;
        HandlePtr process = WinApi::QueryTargetName(hwnd, target, id);
        if (id)
        {
            if (target != m_target)
                hwnd = nullptr;
            else
            {
                m_process = process;
                m_tID = id;
                m_pSet = true;
            }
        }
        else
        {
            hwnd = nullptr;
        }
    }

    return hwnd;
}

void CStockLocateData::Withdraw()
{
    if (m_tSet)
    {
        m_target.Empty();
        m_tSet = false;
    }

    if (m_pSet)
    {
        m_tID = 0;
        m_process = nullptr;
        m_pSet = false;
    }
}

HWND CStockLocateData::ValidateHwnd(HWND hwnd, LocateType type, CString &target, DWORD &pId, HandlePtr &process, HTREEITEM * hitem, HTREEITEM parent) const
{
    if (::IsWindow(hwnd))
    {
        bool isTSet = false;
        bool isPSet = false;
        if (target.IsEmpty())
        {
            process = WinApi::QueryTargetName(hwnd, target, pId);
            if (pId == 0)
                return nullptr;
            
            isTSet = true;
        }
        else
        {
            DWORD id;
            ::GetWindowThreadProcessId(hwnd, &id);

            if (pId != 0)
            {
                if (pId != id)
                    return nullptr;
            }
            else
            {
                CString name;
                HandlePtr p = WinApi::QueryTargetName(hwnd, name, id);
                if (!id)
                    return nullptr;

                if (name != target)
                    return nullptr;

                pId = id;
                process = p;
                isPSet = true;
            }
        }

        switch (type)
        {
        case LT_Buy:
        case LT_Sell:
        case LT_Cancel:
        case LT_Query:
        case LT_Delegate:
            *hitem = this->SearchTreeItem(process, hwnd, type, parent);
            if (*hitem) //though it's in valid top wnd but may not a tree ctrl
                return hwnd;
            if (isTSet)
                target.Empty();
            if (isPSet)
            {
                pId = 0;
                process = nullptr;
            }
            break;
        default:
            return hwnd;
        }
    }

    return nullptr;

}

