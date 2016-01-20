#include "stdafx.h"
#include "resource.h"
#include "Utils.h"
#include "ApiHelper.h"
#include "StockGlobal.h"
#include "Stock.h"
#include "StockLocatePropPage.h"

#define ST_LOCATE_PEN_STYLE     0
#define ST_LOCATE_PEN_WIDTH     3
#define ST_LOCATE_PEN_COLOR     RGB(125, 0, 125)

#define ST_LOCATE_DRAW_TIME     400

#define ST_LOCATE_MASK_STR      _T(" A DDDDD A DDDDD ")
#define ST_LOCATE_TEMP_STR      _T("(_:_____,_:_____)")
#define ST_LOCATE_DEF_STR       _T('_')
#define ST_LOCATE_VALID_STR     _T("1234567890xXyY")

#define ST_LOCATE_POS_FORMAT_STR    _T("(x:%d,y:%d)")

IMPLEMENT_DYNCREATE(CStockLocatePropPage, CBCGPPropertyPage)

BEGIN_MESSAGE_MAP(CStockLocatePropPage, CBCGPPropertyPage)
    ON_WM_TIMER()
    ON_MESSAGE(ST_LOCATE_WND_MSG, OnTargetWnd)
    ON_BN_CLICKED(IDC_LOCATE_OP_COMBO, OnBOSChange)
    ON_EN_CHANGE(IDC_LOCATE_FILE_EDIT, OnFileChange)
    ON_BN_CLICKED(IDC_LOC_CLEAR, OnClear)
END_MESSAGE_MAP()

CStockLocatePropPage::CStockLocatePropPage()
    : CBCGPPropertyPage(CStockLocatePropPage::IDD)
    , m_bosId(ID_LOCATE_OP_BUY)
    , m_dirty(false)
{
}

CStockLocatePropPage::~CStockLocatePropPage()
{
}

void CStockLocatePropPage::DoDataExchange(CDataExchange * pDX)
{
    CBCGPPropertyPage::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_LOCATE_OP_COMBO, _tradeSelBtn);
    
    //DDX_Control(pDX, IDC_LOCATE_OP_BTN_EDIT,          m_buyOrSell);
    DDX_Control(pDX, IDC_LOCATE_CODE_EDIT,              m_bosCode);
    DDX_Control(pDX, IDC_LOCATE_OP_PRICE_EDIT,          m_bosPrice);
    DDX_Control(pDX, IDC_LOCATE_QUANTITY_EDIT,          m_bosQuantity);
    DDX_Control(pDX, IDC_LOCATE_SET_ORDER_EDIT,         m_bosOrder);

    //DDX_Control(pDX, IDC_LOCATE_OP_BTN_HANDLE,        m_hbuyOrSell);
    DDX_Control(pDX, IDC_LOCATE_CODE_HANDLE,            m_hbosCode);
    DDX_Control(pDX, IDC_LOCATE_OP_PRICE_HANDLE,        m_hbosPrice);
    DDX_Control(pDX, IDC_LOCATE_QUANTITY_HANDLE,        m_hbosQuant);
    DDX_Control(pDX, IDC_LOCATE_SET_ORDER_HANDLE,       m_hbosOrder);

    //DDX_Control(pDX, IDC_LOCATE_OP_BTN_LABEL,         m_buyOrSellLab);
    DDX_Control(pDX, IDC_LOCATE_CODE_LABEL,             m_bosCodeLab);
    DDX_Control(pDX, IDC_LOCATE_OP_PRICE_LABEL,         m_bosPriceLab);
    DDX_Control(pDX, IDC_LOCATE_QUANTITY_LABEL,         m_bosQuantityLab);
    DDX_Control(pDX, IDC_LOCATE_SET_ORDER_LABEL,        m_bosOrderLab);

    //DDX_Control(pDX, IDC_LOCATE_CANCEL_ORDER_EDIT,    m_cancel);
    DDX_Control(pDX, IDC_LOCATE_CANCEL_BTN_EDIT,        m_cancelBtn);

    DDX_Control(pDX, IDC_LOCATE_CANCEL_LIST_EDIT,       m_cancelList);
    //DDX_Control(pDX, IDC_LOCATE_DELEGATE_EDIT,        m_delegate);
    DDX_Control(pDX, IDC_LOCATE_DELEGATE_LIST_EDIT,     m_delegateList);

    //DDX_Control(pDX, IDC_LOCATE_CANCEL_ORDER_HANDLE,    m_hcancel);
    DDX_Control(pDX, IDC_LOCATE_CANCEL_BTN_HANDLE,      m_hcancelBtn);

    DDX_Control(pDX, IDC_LOCATE_CANCEL_LIST_HANDLE,     m_hcancelList);
    //DDX_Control(pDX, IDC_LOCATE_DELEGATE_HANDLE,        m_hdelegate);
    DDX_Control(pDX, IDC_LOCATE_DELEGATE_LIST_HANDLE,   m_hdelegateList);

    DDX_Control(pDX, IDC_LOCATE_TREE_EDIT,              m_tree);
    DDX_Control(pDX, IDC_LOCATE_TREE_HANDLE,            m_htree);

    DDX_Control(pDX, IDC_LOCATE_APP_EDIT,               m_app);
    DDX_Control(pDX, IDC_LOCATE_APP_HANDLE,             m_happ);

    DDX_Control(pDX, IDC_LOCATE_FILE_EDIT,              _locateFile);
}

BOOL CStockLocatePropPage::OnInitDialog()
{
    CBCGPPropertyPage::OnInitDialog();

    m_menu.LoadMenu(IDR_LOCATE_OP_MENU);
    _tradeSelBtn.m_hMenu = m_menu.GetSubMenu(0)->GetSafeHmenu();
    _tradeSelBtn.SizeToContent();
    _tradeSelBtn.m_bOSMenu = FALSE;
    _tradeSelBtn.m_nMenuResult = m_bosId;
    _tradeSelBtn.m_bStayPressed = FALSE;
    ::CheckMenuItem(_tradeSelBtn.m_hMenu, m_bosId, MF_CHECKED);

    //m_buyOrSell.EnableMask(ST_LOCATE_MASK_STR, ST_LOCATE_TEMP_STR, ST_LOCATE_DEF_STR, ST_LOCATE_VALID_STR);
    //m_bosCode.EnableMask(ST_LOCATE_MASK_STR, ST_LOCATE_TEMP_STR, ST_LOCATE_DEF_STR, ST_LOCATE_VALID_STR);
    //m_bosPrice.EnableMask(ST_LOCATE_MASK_STR, ST_LOCATE_TEMP_STR, ST_LOCATE_DEF_STR, ST_LOCATE_VALID_STR);
    //m_bosQuantity.EnableMask(ST_LOCATE_MASK_STR, ST_LOCATE_TEMP_STR, ST_LOCATE_DEF_STR, ST_LOCATE_VALID_STR);
    //m_bosOrder.EnableMask(ST_LOCATE_MASK_STR, ST_LOCATE_TEMP_STR, ST_LOCATE_DEF_STR, ST_LOCATE_VALID_STR);
    //m_buyOrSell.DisableMask();
    //m_bosCode.DisableMask();
    //m_bosPrice.DisableMask();
    //m_bosQuantity.DisableMask();
    //m_bosOrder.DisableMask();

    _locateFile.EnableFileBrowseButton(ST_CONFIG_FILE_EXT, ST_CONFIG_FILE_FILTER);

    this->GetDlgItem(IDC_LOCATE_IMAGE)->UnsubclassWindow(); // who subclass it?

    m_pic.SubclassDlgItem(IDC_LOCATE_IMAGE, this);

    this->InitCtrls();

    this->SwitchBOS();

    return FALSE; // we set the focus
}

void CStockLocatePropPage::OnOK()
{
    CPropertyPage::OnOK();

    if (m_dirty)
    {
        this->UpdateData(TRUE);

        CStockAppData &app = theApp.AppData();
        CStockLocateData &data = app.LocateData();

        data.SetTarget(m_target, m_tID, m_process);

        int count = 0;
        for (int i = 0; i < LT_Num; ++i) // overwrite the old value no matter the ctrl is valid or not
        {
            data.SetInfo((LocateType)i, _ctrls[i].i);
            if (i == LT_CancelList || i == LT_DelegateList)
            {
                ListViewColumn *p = this->GetListViewColumns((LocateType)i);
                for (int j = 0; j < SOF_Num; ++j)
                {
                    data.SetListCol((LocateType)i, (StockOrderField)j, p[j]);
                }
            }

            if (_ctrls[i].i.hwnd)
                ++count;
        }

        if (count == LT_Num)
            data.SetReady(true);
        else
            data.SetReady(false);

        CString locFile;
        _locateFile.GetWindowText(locFile);
        app.LocFile(locFile);

        if (data.Save(locFile))
        {
            if (app.Save())
            {
                this->SetModified(FALSE);
                m_dirty = false;
            }
        }

    }
}

void CStockLocatePropPage::InitCtrls()
{
    // TODO : access locate data directly, may change this later
    CStockLocateData &data = theApp.AppData().LocateData();

    for (int i = 0; i < SOF_Num; ++i)
    {
        data.ResetListCol(_cancleList[i], (StockOrderField)i);
        data.ResetListCol(_delegateList[i], (StockOrderField)i);
    }

    for (int i = 0; i < LT_Num; ++i)
    {
        LocateType type = (LocateType)i;

        _ctrls[i].label = this->FindLabelCtrl(type);
        _ctrls[i].p = this->FindMaskCtrl(type);
        _ctrls[i].h = this->FindEditCtrl(type);

        LocateInfo const& info = data.LocInfo(type);

        _ctrls[i].i = info;
        if (info.hwnd)
        {
            if (i == LT_CancelList || i == LT_DelegateList)
            {
                ListViewColumn *p = this->GetListViewColumns(type);
                for (int j = 0; j < SOF_Num; ++j)
                {
                    p[j].col = data.ListCol((LocateType)i, (StockOrderField)j).col;
                }
            }
        }

        if (i > LT_SellOrder || i == LT_App) // buy/sell text will be set in switchBOS
            this->SetCtrlText((LocateType)i);
    }

    m_target = data.TargetString();
    m_tID = data.TargetId();
    m_process = data.TargetProcess();
    
    _locateFile.SetWindowText(theApp.AppData().LocFile());
}

CBCGPMaskEdit * CStockLocatePropPage::FindMaskCtrl(LocateType type)
{
    switch (type)
    {
    case LT_App:
        return &m_app;
    case LT_Buy:
        return &m_tree;
    case LT_BuyCode:
        return &m_bosCode;
    case LT_BuyPrice:
        return &m_bosPrice;
    case LT_BuyQuant:
        return &m_bosQuantity;
    case LT_BuyOrder:
        return &m_bosOrder;
    case LT_Sell:
        return &m_tree;
    case LT_SellCode:
        return &m_bosCode;
    case LT_SellPrice:
        return &m_bosPrice;
    case LT_SellQuant:
        return &m_bosQuantity;
    case LT_SellOrder:
        return &m_bosOrder;
    case LT_Cancel:
        return &m_tree;
    case LT_CancelBtn:
        return &m_cancelBtn;
    case LT_CancelList:
        return &m_cancelList;
    case LT_Delegate:
        return &m_tree;
    case LT_DelegateList:
        return &m_delegateList;
    case LT_Num:
    default:
        ASSERT(FALSE);
        break;
    }

    return nullptr;
}

CBCGPStatic * CStockLocatePropPage::FindLabelCtrl(LocateType type)
{
    switch (type)
    {
    case LT_BuyCode:
        return &m_bosCodeLab;
    case LT_BuyPrice:
        return &m_bosPriceLab;
    case LT_BuyQuant:
        return &m_bosQuantityLab;
    case LT_BuyOrder:
        return &m_bosOrderLab;
    case LT_SellCode:
        return &m_bosCodeLab;
    case LT_SellPrice:
        return &m_bosPriceLab;
    case LT_SellQuant:
        return &m_bosQuantityLab;
    case LT_SellOrder:
        return &m_bosOrderLab;
    case LT_App:
    case LT_Buy:
    case LT_Sell:
    case LT_Cancel:
    case LT_CancelBtn:
    case LT_CancelList:
    case LT_Delegate:
    case LT_DelegateList:
        return nullptr;
    case LT_Num:
    default:
        ASSERT(FALSE);
        break;
    }

    return nullptr;
}

CBCGPEdit * CStockLocatePropPage::FindEditCtrl(LocateType type)
{
    switch (type)
    {
    case LT_App:
        return &m_happ;
    case LT_Buy:
        return &m_htree;
    case LT_BuyCode:
        return &m_hbosCode;
    case LT_BuyPrice:
        return &m_hbosPrice;
    case LT_BuyQuant:
        return &m_hbosQuant;
    case LT_BuyOrder:
        return &m_hbosOrder;
    case LT_Sell:
        return &m_htree;
    case LT_SellCode:
        return &m_hbosCode;
    case LT_SellPrice:
        return &m_hbosPrice;
    case LT_SellQuant:
        return &m_hbosQuant;
    case LT_SellOrder:
        return &m_hbosOrder;
    case LT_Cancel:
        return &m_htree;
    case LT_CancelBtn:
        return &m_hcancelBtn;
    case LT_CancelList:
        return &m_hcancelList;
    case LT_Delegate:
        return &m_htree;
    case LT_DelegateList:
        return &m_hdelegateList;
    case LT_Num:
    default:
        ASSERT(FALSE);
        break;
    }

    return nullptr;
}

void CStockLocatePropPage::SwitchBOS()
{
    if (m_bosId == ID_LOCATE_OP_BUY)
    {
        //_ctrls[LT_Buy].label->SetWindowText(CString(MAKEINTRESOURCE(IDS_BUY)));
        _ctrls[LT_BuyPrice].label->SetWindowText(CString(MAKEINTRESOURCE(IDS_BUY_PRICE)));
        _ctrls[LT_BuyQuant].label->SetWindowText(CString(MAKEINTRESOURCE(IDS_BUY_QUANTITY)));
        _ctrls[LT_BuyOrder].label->SetWindowText(CString(MAKEINTRESOURCE(IDS_BUY_ORDER)));

        //this->SetCtrlText(LT_Buy);
        this->SetCtrlText(LT_BuyCode);
        this->SetCtrlText(LT_BuyPrice);
        this->SetCtrlText(LT_BuyQuant);
        this->SetCtrlText(LT_BuyOrder);
    }
    else if (m_bosId == ID_LOCATE_OP_SELL)
    {
        //_ctrls[LT_Sell].label->SetWindowText(CString(MAKEINTRESOURCE(IDS_SELL)));
        _ctrls[LT_SellPrice].label->SetWindowText(CString(MAKEINTRESOURCE(IDS_SELL_PRICE)));
        _ctrls[LT_SellQuant].label->SetWindowText(CString(MAKEINTRESOURCE(IDS_SELL_QUANTITY)));
        _ctrls[LT_SellOrder].label->SetWindowText(CString(MAKEINTRESOURCE(IDS_SELL_ORDER)));

        //this->SetCtrlText(LT_Sell);
        this->SetCtrlText(LT_SellCode);
        this->SetCtrlText(LT_SellPrice);
        this->SetCtrlText(LT_SellQuant);
        this->SetCtrlText(LT_SellOrder);
    }

    this->SetCtrlFocus();

    this->UpdateData(FALSE);
}

void CStockLocatePropPage::SetCtrlText(LocateType type)
{
    if (_ctrls[type].i.hwnd)
    {
        CString pos;
        pos.Format(ST_LOCATE_POS_FORMAT_STR, _ctrls[type].i.pos.x, _ctrls[type].i.pos.y);
        _ctrls[type].p->SetWindowText(pos);

        CString hwnd;
        hwnd.Format(_T("%x"), _ctrls[type].i.hwnd);
        _ctrls[type].h->SetWindowText(hwnd);
    }
    else
    {
        _ctrls[type].p->SetWindowText(_T(""));
        _ctrls[type].h->SetWindowText(_T(""));
    }

}

void CStockLocatePropPage::SetCtrlFocus()
{
    for (int i = 0; i < LT_Num; ++i)
    {
        if (m_bosId == ID_LOCATE_OP_BUY && (i > LT_Sell && i <= LT_SellOrder))
            continue;
        if (m_bosId == ID_LOCATE_OP_SELL && (i > LT_Buy && i <= LT_BuyOrder))
            continue;

        if (_ctrls[i].i.hwnd == nullptr)
        {
            _ctrls[i].p->SetFocus();
            return;
        }
    }
}

LocateType CStockLocatePropPage::GetCtrlFocus()
{
    CWnd *pWnd = this->GetFocus();

    if (pWnd)
    {
        for (int i = 0; i < LT_Num; ++i)
        {
            if (m_bosId == ID_LOCATE_OP_BUY && (i > LT_Sell && i <= LT_SellOrder))
                continue;
            if (m_bosId == ID_LOCATE_OP_SELL && (i > LT_Buy && i <= LT_BuyOrder))
                continue;

            if (pWnd == _ctrls[i].p ||
                pWnd == _ctrls[i].h)
            {
                return (LocateType)i;
            }
        }
    }

    return LT_Num;
}

void CStockLocatePropPage::OnTimer(UINT nIDEvent)
{
    POINT pnt;

    RECT rc;
    HWND deskHwnd = ::GetDesktopWindow();
    HDC deskDC = ::GetWindowDC(deskHwnd);

    int oldRop2 = SetROP2(deskDC, 10);
    ::GetCursorPos(&pnt);

    HWND unHwnd = WinApi::TopWndFromPoint(pnt);

    ::GetWindowRect(unHwnd, &rc);
    if (rc.top < 0) rc.top = 0;
    if (rc.left < 0) rc.left = 0;

    HPEN newPen = ::CreatePen(ST_LOCATE_PEN_STYLE, ST_LOCATE_PEN_WIDTH, ST_LOCATE_PEN_COLOR);
    HGDIOBJ oldPen = ::SelectObject(deskDC, newPen);
    ::Rectangle(deskDC, rc.left, rc.top, rc.right, rc.bottom);

    Sleep(ST_LOCATE_DRAW_TIME);

    ::Rectangle(deskDC, rc.left, rc.top, rc.right, rc.bottom);

    ::SetROP2(deskDC, oldRop2);
    ::SelectObject(deskDC, oldPen);
    ::DeleteObject(newPen);
    ::ReleaseDC(deskHwnd, deskDC);

    deskDC = NULL;

    CBCGPPropertyPage::OnTimer(nIDEvent);
}

void CStockLocatePropPage::OnBOSChange()
{
    if (m_bosId != _tradeSelBtn.m_nMenuResult)
    {
        ::CheckMenuItem(_tradeSelBtn.m_hMenu, m_bosId, MF_UNCHECKED);
        m_bosId = _tradeSelBtn.m_nMenuResult;
        ::CheckMenuItem(_tradeSelBtn.m_hMenu, m_bosId, MF_CHECKED);
        

        this->SwitchBOS();
    }

}

void CStockLocatePropPage::OnFileChange()
{
    this->SetModified(TRUE);
    m_dirty = true;
}

void CStockLocatePropPage::OnClear()
{
    m_process = nullptr;
    m_target.Empty();
    m_tID = 0;

    for (int i = 0; i < LT_Num; ++i)
    {
        _ctrls[i].i.hwnd = nullptr;
        _ctrls[i].i.hitem = nullptr;
        this->SetCtrlText((LocateType)i);
    }
}

LRESULT CStockLocatePropPage::OnTargetWnd(WPARAM wParam, LPARAM lParam)
{
    HWND hwnd = m_pic.TargetHWnd();
    POINT pos = m_pic.TargetPos();

    LocateType type = this->GetCtrlFocus();

    if (type < LT_Num)
    {
        bool tSet = m_target.IsEmpty();
        bool pSet = (m_tID == 0);

        CStockLocateData & data = theApp.AppData().LocateData();
        hwnd = data.ValidateHwnd(hwnd, type, m_target, m_tID, m_process, &(_ctrls[type].i.hitem));
        if (hwnd)
        {
            LocateType t1, t2, t3;

            bool share = true;
            switch (type)
            {
            case LT_Buy:
                t1 = LT_Sell;
                t2 = LT_Cancel;
                t3 = LT_Delegate;
                break;
            case LT_Sell:
                t1 = LT_Buy;
                t2 = LT_Cancel;
                t3 = LT_Delegate;
                break;
            case LT_Cancel:
                t1 = LT_Sell;
                t2 = LT_Buy;
                t3 = LT_Delegate;
                break;
            case LT_Delegate:
                t1 = LT_Sell;
                t2 = LT_Cancel;
                t3 = LT_Buy;
                break;
            case LT_CancelList:
            case LT_DelegateList:
            {
                ListViewColumn *p = this->GetListViewColumns(type);
                if (data.GetListCol(m_process, hwnd, p, SOF_Num) != SOF_Num)
                {
                    this->Withdraw(tSet, pSet);
                    return 0;
                }

            }
            default:
                share = false;
                break;
            }

            if (share)
            {
                bool valid = false;
                if (data.ValidateHwnd(hwnd, t1, m_target, m_tID, m_process, &(_ctrls[t1].i.hitem)) &&
                    data.ValidateHwnd(hwnd, t2, m_target, m_tID, m_process, &(_ctrls[t2].i.hitem)) &&
                    data.ValidateHwnd(hwnd, t3, m_target, m_tID, m_process, &(_ctrls[t3].i.hitem)))
                {
                    POINT p1, p2, p3;
                    if (WinApi::CacTreeItemCenter(hwnd, _ctrls[type].i.hitem, pos) &&
                        WinApi::CacTreeItemCenter(hwnd, _ctrls[t1].i.hitem, p1) &&
                        WinApi::CacTreeItemCenter(hwnd, _ctrls[t2].i.hitem, p2) &&
                        WinApi::CacTreeItemCenter(hwnd, _ctrls[t3].i.hitem, p3))
                    {
                        // tree ctrl is valid
                        _ctrls[t1].i.hwnd = _ctrls[t2].i.hwnd = _ctrls[t3].i.hwnd = hwnd;
                        _ctrls[t1].i.pos = p1;
                        _ctrls[t2].i.pos = p2;
                        _ctrls[t3].i.pos = p3;

                        valid = true;
                    }
                }

                if (!valid)
                {
                    _ctrls[type].i.hitem = _ctrls[t1].i.hitem = _ctrls[t2].i.hitem = _ctrls[t3].i.hitem = nullptr;

                    this->Withdraw(tSet, pSet);

                    return 0;
                }
            }

            _ctrls[type].i.hwnd = hwnd;
            _ctrls[type].i.pos = pos;

            this->SetCtrlText(type);

            this->SetModified(TRUE);
            m_dirty = true;

            this->UpdateData(FALSE);

            this->SetCtrlFocus();

        }
        else
        {
            // TODO:
        }
    }

    return 0;
}
