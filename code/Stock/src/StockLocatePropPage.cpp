#include "stdafx.h"
#include "resource.h"
#include "StockGlobal.h"
#include "StockLocatePropPage.h"

#define ST_LOCATE_PEN_STYLE     0
#define ST_LOCATE_PEN_WIDTH     3
#define ST_LOCATE_PEN_COLOR     RGB(125, 0, 125)

#define ST_LOCATE_DRAW_TIME     400

#define ST_LOCATE_MASK_STR      _T(" A DDDDD A DDDDD")
#define ST_LOCATE_TEMP_STR      _T("(_:_____,_:_____)")
#define ST_LOCATE_DEF_STR       _T('_')
#define ST_LOCATE_VALID_STR     _T("1234567890xXyY")

BEGIN_MESSAGE_MAP(CStockLocatePropPage, CBCGPPropertyPage)
    ON_WM_TIMER()
    ON_MESSAGE(ST_LOCATE_WND_MSG, OnTargetWnd)
    ON_BN_CLICKED(IDC_LOCATE_OP_COMBO, OnBOSChange)
END_MESSAGE_MAP()

CStockLocatePropPage::CStockLocatePropPage()
    : m_bosId(ID_LOCATE_OP_BUY)
    , m_hbuy(NULL)
    , m_hsell(NULL)
    , m_hbuyCode(NULL)
    , m_hsellCode(NULL)
    , m_hbuyPrice(NULL)
    , m_hsellPrice(NULL)
    , m_hbuyQuant(NULL)
    , m_hsellQuant(NULL)
    , m_hbuyOrder(NULL)
    , m_hsellOrder(NULL)
{
}

CStockLocatePropPage::~CStockLocatePropPage()
{
}

void CStockLocatePropPage::DoDataExchange(CDataExchange * pDX)
{
    CBCGPPropertyPage::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_LOCATE_OP_COMBO, _tradeSelBtn);
    
    DDX_Control(pDX, IDC_LOCATE_OP_BTN_EDIT,        _buyOrSell);
    DDX_Control(pDX, IDC_LOCATE_CODE_EDIT,          _bosCode);
    DDX_Control(pDX, IDC_LOCATE_OP_PRICE_EDIT,      _bosPrice);
    DDX_Control(pDX, IDC_LOCATE_QUANTITY_EDIT,      _bosQuantity);
    DDX_Control(pDX, IDC_LOCATE_SET_ORDER_EDIT,     _bosOrder);

    DDX_Control(pDX, IDC_LOCATE_OP_BTN_HANDLE,      m_hbuyOrSell);
    DDX_Control(pDX, IDC_LOCATE_CODE_HANDLE,        m_hbosCode);
    DDX_Control(pDX, IDC_LOCATE_OP_PRICE_HANDLE,    m_hbosPrice);
    DDX_Control(pDX, IDC_LOCATE_QUANTITY_HANDLE,    m_hbosQuant);
    DDX_Control(pDX, IDC_LOCATE_SET_ORDER_HANDLE,   m_hbosOrder);

    DDX_Control(pDX, IDC_LOCATE_OP_BTN_LABEL,       _buyOrSellLab);
    DDX_Control(pDX, IDC_LOCATE_CODE_LABEL,         _bosCodeLab);
    DDX_Control(pDX, IDC_LOCATE_OP_PRICE_LABEL,     _bosPriceLab);
    DDX_Control(pDX, IDC_LOCATE_QUANTITY_LABEL,     _bosQuantityLab);
    DDX_Control(pDX, IDC_LOCATE_SET_ORDER_LABEL,    _bosOrderLab);

    DDX_Control(pDX, IDC_LOCATE_CANCEL_ORDER_EDIT,  _cancel);
    DDX_Control(pDX, IDC_LOCATE_CANCEL_LIST_EDIT,   _cancelList);
    DDX_Control(pDX, IDC_LOCATE_DELEGATE_EDIT,      _delegate);
    DDX_Control(pDX, IDC_LOCATE_DELEGATE_LIST_EDIT, _delegateList);

    DDX_Control(pDX, IDC_LOCATE_FILE_EDIT, _locateFile);
}

BOOL CStockLocatePropPage::OnInitDialog()
{
    CBCGPPropertyPage::OnInitDialog();

    m_menu.LoadMenu(IDR_LOCATE_OP_MENU);
    _tradeSelBtn.m_hMenu = m_menu.GetSubMenu(0)->GetSafeHmenu();
    _tradeSelBtn.SizeToContent();
    _tradeSelBtn.m_bOSMenu = FALSE;
    _tradeSelBtn.m_nMenuResult = m_bosId;

    _buyOrSell.EnableMask(ST_LOCATE_MASK_STR, ST_LOCATE_TEMP_STR, ST_LOCATE_DEF_STR, ST_LOCATE_VALID_STR);
    _bosCode.EnableMask(ST_LOCATE_MASK_STR, ST_LOCATE_TEMP_STR, ST_LOCATE_DEF_STR, ST_LOCATE_VALID_STR);
    _bosPrice.EnableMask(ST_LOCATE_MASK_STR, ST_LOCATE_TEMP_STR, ST_LOCATE_DEF_STR, ST_LOCATE_VALID_STR);
    _bosQuantity.EnableMask(ST_LOCATE_MASK_STR, ST_LOCATE_TEMP_STR, ST_LOCATE_DEF_STR, ST_LOCATE_VALID_STR);
    _bosOrder.EnableMask(ST_LOCATE_MASK_STR, ST_LOCATE_TEMP_STR, ST_LOCATE_DEF_STR, ST_LOCATE_VALID_STR);

    _locateFile.EnableFileBrowseButton(ST_CONFIG_FILE_EXT, ST_CONFIG_FILE_FILTER);

    m_pic.SubclassDlgItem(IDC_LOCATE_IMAGE, this);

    return TRUE;
}

void CStockLocatePropPage::OnTimer(UINT nIDEvent)
{
    POINT pnt;

    RECT rc;
    HWND deskHwnd = ::GetDesktopWindow();
    HDC deskDC = ::GetWindowDC(deskHwnd);

    int oldRop2 = SetROP2(deskDC, 10);
    ::GetCursorPos(&pnt);

    HWND unHwnd = ::WindowFromPoint(pnt);

    HWND grayHwnd = ::GetWindow(unHwnd, GW_CHILD);
    RECT tempRc;
    BOOL bFind = FALSE;
    while (grayHwnd)
    {
        ::GetWindowRect(grayHwnd, &tempRc);
        if (::PtInRect(&tempRc, pnt))
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
        // TODO : switch to b or s

    }

}

LRESULT CStockLocatePropPage::OnTargetWnd(WPARAM wParam, LPARAM lParam)
{
    return LRESULT();
}
