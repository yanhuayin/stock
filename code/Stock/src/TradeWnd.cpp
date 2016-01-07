#include "stdafx.h"
//#include <type_traits>
#include "Utils.h"
#include "StockGlobal.h"
#include "TradeWnd.h"

#define ST_DIGIT_ONLY_STR           _T("1234567890")
#define ST_DEFAULT_QUANTITY_STR     _T("0")

#define ST_SPIN_ACELL               1000

#define ST_LIST_CTRL_MARGIN         5

BEGIN_MESSAGE_MAP(CTradeWnd, CBCGPDialog)
    ON_BN_CLICKED(IDC_QTOP1_BTI, OnQ1PlusClicked)
    ON_BN_CLICKED(IDC_QTOP1_BTR, OnQ1MinusClicked)
    ON_NOTIFY(UDN_DELTAPOS, IDC_QUANTITY_SPIN, OnDeltPosSpinCtrl)
    ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
    ON_COMMAND(IDOK, OnEnter)
END_MESSAGE_MAP()

CTradeWnd::CTradeWnd(CWnd * parent)
    : CBCGPDialog(CTradeWnd::IDD, parent)
{
    this->EnableVisualManagerStyle(globalData.m_bUseVisualManagerInBuiltInDialogs, TRUE);
    this->EnableLayout(TRUE);
}

CTradeWnd::~CTradeWnd()
{
}

void CTradeWnd::DoDataExchange(CDataExchange * pDx)
{
    CBCGPDialog::DoDataExchange(pDx);

    DDX_Control(pDx, IDC_CODE_EDIT, _code);
    DDX_Control(pDx, IDC_NAME_EDIT, _name);
    DDX_Control(pDx, IDC_QUOTA_EDIT, _quota);
    DDX_Control(pDx, IDC_LEFT_EDIT, _left);
    DDX_Control(pDx, IDC_QUANTITY_EDIT, _quantity);
    DDX_Control(pDx, IDC_QUANTITY_SPIN, _qSpin);
    DDX_Control(pDx, IDC_QTOP1, _q1);
    DDX_Control(pDx, IDC_QTOP2, _q2);
    DDX_Control(pDx, IDC_QTOP1_BTR, _q1Minus);
    DDX_Control(pDx, IDC_QTOP1_BTI, _q1Plus);
    DDX_Control(pDx, IDC_QTOP2_BTR, _q2Minus);
    DDX_Control(pDx, IDC_QTOP2_BTI, _q2Plus);
    DDX_Control(pDx, IDC_DOUBLE_BTN, _qDouble);
    DDX_Control(pDx, IDC_HALF_BTN, _qHalf);
    DDX_Control(pDx, IDC_INFO_RECT, m_infoRect);
    DDX_Control(pDx, IDC_ORDER_RECT, m_orderRect);
}

BOOL CTradeWnd::OnInitDialog()
{
    CBCGPDialog::OnInitDialog();

    _code.DisableMask();
    _code.SetValidChars(ST_DIGIT_ONLY_STR);

    _name.SetReadOnly(TRUE);

    _quota.SetReadOnly(TRUE);
    
    _left.SetReadOnly(TRUE);

    _quantity.DisableMask();
    _quantity.SetValidChars(ST_DIGIT_ONLY_STR);
    //m_quantity.SetWindowText(ST_DEFAULT_QUANTITY_STR);

    _qSpin.SetRange32(ST_MIN_QUANTITY, ST_MAX_QUANTITY);

    _q1.DisableMask();
    _q1.SetValidChars(ST_DIGIT_ONLY_STR);
    //m_q1.SetWindowText(ST_DEFAULT_QUANTITY_STR);

    _q2.DisableMask();
    _q2.SetValidChars(ST_DIGIT_ONLY_STR);
    //m_q2.SetWindowText(ST_DEFAULT_QUANTITY_STR);

    CRect infoRect;
    m_infoRect.GetClientRect(&infoRect);
    m_infoRect.MapWindowPoints(this, &infoRect);

    _info.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, infoRect, this, IDC_INFO_GRID);
    _info.EnableColumnAutoSize(TRUE);
    _info.SetWholeRowSel();
    _info.EnableHeader(TRUE, 0);
    _info.SetScrollBarsStyle(CBCGPScrollBar::BCGP_SBSTYLE_VISUAL_MANAGER);

    CRect orderRect;
    m_orderRect.GetClientRect(&orderRect);
    m_orderRect.MapWindowPoints(this, &orderRect);
    _order.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, orderRect, this, IDC_ORDER_GRID);
    _order.EnableColumnAutoSize(TRUE);
    _order.SetWholeRowSel();
    _order.EnableHeader(TRUE, 0);
    _order.SetScrollBarsStyle(CBCGPScrollBar::BCGP_SBSTYLE_VISUAL_MANAGER);

    return TRUE;
}

void CTradeWnd::OnQ1PlusClicked()
{
    if (_q1PlusClickEvent)
    {
        _q1PlusClickEvent();
    }
}

void CTradeWnd::OnQ1MinusClicked()
{
    if (_q1MinusClickEvent)
    {
        _q1MinusClickEvent();
    }
}

void CTradeWnd::OnDeltPosSpinCtrl(NMHDR * pNMHDR, LRESULT * pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    if (pNMUpDown->iDelta == -1) // down
    {
        CString quantityStr;
        _quantity.GetWindowText(quantityStr);
        int quantity = _ttoi(quantityStr);

        quantity = clamp(quantity - ST_SPIN_ACELL, ST_MIN_QUANTITY, ST_MAX_QUANTITY);

        quantityStr.Format(_T("%d"), quantity);

        _quantity.SetWindowText(quantityStr);
    }
    else if (pNMUpDown->iDelta == 1) // up
    {
        CString quantityStr;
        _quantity.GetWindowText(quantityStr);
        int quantity = _ttoi(quantityStr);

        quantity = clamp(quantity + ST_SPIN_ACELL, ST_MIN_QUANTITY, ST_MAX_QUANTITY);

        quantityStr.Format(_T("%d"), quantity);

        _quantity.SetWindowText(quantityStr);
    }

    *pResult = 0;
}

void CTradeWnd::OnEnter()
{

}

LRESULT CTradeWnd::HandleInitDialog(WPARAM wPram, LPARAM lParam)
{
#ifndef _AFX_NO_OCC_SUPPORT
    LRESULT lRes = CBCGPDialog::HandleInitDialog(wPram, lParam);
#else
    LRESULT lRes = 0;
#endif // !_AFX_NO_OCC_SUPPORT

    CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
    if (pLayout)
    {
        pLayout->AddAnchor(IDC_STOCK_GROUP, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeHorz, CPoint(0, 0), CPoint(100, 0));
        pLayout->AddAnchor(IDC_QUANTITY_GROUP, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeHorz, CPoint(0, 0), CPoint(100, 0));

        pLayout->AddAnchor(IDC_NAME_LABEL, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_CODE_LABEL, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_QUOTA_LABEL, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_LEFT_LABEL, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));

        pLayout->AddAnchor(IDC_NAME_EDIT, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_CODE_EDIT, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_QUOTA_EDIT, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_LEFT_EDIT, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));

        pLayout->AddAnchor(IDC_QUANTITY_EDIT, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_QUANTITY_SPIN, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_DOUBLE_BTN, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_HALF_BTN, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_QTOP1, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_QTOP1_BTI, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_QTOP1_BTR, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_QTOP2, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_QTOP2_BTI, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));
        pLayout->AddAnchor(IDC_QTOP2_BTR, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone, CPoint(50, 0));

        pLayout->AddAnchor(IDC_INFO_GRID, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeHorz, CPoint(0, 0), CPoint(100, 0));
        pLayout->AddAnchor(IDC_ORDER_GRID, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth, CPoint(0, 0), CPoint(100, 100));
    }

    return lRes;
}


