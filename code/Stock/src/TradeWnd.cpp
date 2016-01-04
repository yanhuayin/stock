#include "stdafx.h"
#include <type_traits>
#include "Utils.h"
#include "TradeWnd.h"

#define ST_DIGIT_ONLY_STR           _T("1234567890")
#define ST_DEFAULT_QUANTITY_STR     _T("0")
#define ST_MAX_QUANTITY             9999999
#define ST_MIN_QUANTITY             0

#define ST_SPIN_ACELL               1000

#define ST_LIST_CTRL_MARGIN         5

BEGIN_MESSAGE_MAP(CTradeWnd, CBCGPDialog)
    ON_BN_CLICKED(IDC_QTOP1_BTI, OnQ1PlusClicked)
    ON_BN_CLICKED(IDC_QTOP1_BTR, OnQ1MinusClicked)
    ON_NOTIFY(UDN_DELTAPOS, IDC_QUANTITY_SPIN, OnDeltPosSpinCtrl)
END_MESSAGE_MAP()

CTradeWnd::CTradeWnd(CWnd * parent)
    : CBCGPDialog(CTradeWnd::IDD, parent)
{
    this->EnableVisualManagerStyle(globalData.m_bUseVisualManagerInBuiltInDialogs, TRUE);
}

CTradeWnd::~CTradeWnd()
{
}

void CTradeWnd::DoDataExchange(CDataExchange * pDx)
{
    CBCGPDialog::DoDataExchange(pDx);

    DDX_Control(pDx, IDC_CODE_EDIT, m_code);
    DDX_Control(pDx, IDC_NAME_EDIT, m_name);
    DDX_Control(pDx, IDC_QUOTA_EDIT, m_quota);
    DDX_Control(pDx, IDC_LEFT_EDIT, m_left);
    DDX_Control(pDx, IDC_QUANTITY_EDIT, m_quantity);
    DDX_Control(pDx, IDC_QUANTITY_SPIN, m_qSpin);
    DDX_Control(pDx, IDC_QTOP1, m_q1);
    DDX_Control(pDx, IDC_QTOP2, m_q2);
    DDX_Control(pDx, IDC_QTOP1_BTR, m_q1Minus);
    DDX_Control(pDx, IDC_QTOP1_BTI, m_q1Plus);
    DDX_Control(pDx, IDC_QTOP2_BTR, m_q2Minus);
    DDX_Control(pDx, IDC_QTOP2_BTI, m_q2Plus);
    DDX_Control(pDx, IDC_DOUBLE_BTN, m_qDouble);
    DDX_Control(pDx, IDC_DIVIDE_BTN, m_qHalf);

}

template <typename T>
void TakeMargin(T &min, T &max, T margin)
{
    static_assert(std::is_integral<T>::value, "integral type required");

    if ((max - min) > (2 * margin))
    {
        min += margin;
        max -= margin;
    }
    else if (max > (min + 2))
    {
        min += 1;
        max -= 1;
    }
}

BOOL CTradeWnd::OnInitDialog()
{
    CBCGPDialog::OnInitDialog();

    m_code.DisableMask();
    m_code.SetValidChars(ST_DIGIT_ONLY_STR);

    m_name.SetReadOnly(TRUE);

    m_quota.SetReadOnly(TRUE);
    
    m_left.SetReadOnly(TRUE);

    m_quantity.DisableMask();
    m_quantity.SetValidChars(ST_DIGIT_ONLY_STR);
    //m_quantity.SetWindowText(ST_DEFAULT_QUANTITY_STR);

    m_qSpin.SetRange32(ST_MIN_QUANTITY, ST_MAX_QUANTITY);

    m_q1.DisableMask();
    m_q1.SetValidChars(ST_DIGIT_ONLY_STR);
    //m_q1.SetWindowText(ST_DEFAULT_QUANTITY_STR);

    m_q2.DisableMask();
    m_q2.SetValidChars(ST_DIGIT_ONLY_STR);
    //m_q2.SetWindowText(ST_DEFAULT_QUANTITY_STR);

    CRect infoRect;
    this->GetDlgItem(IDC_INFO_GROUP)->GetWindowRect(&infoRect);
    this->ScreenToClient(&infoRect);
    TakeMargin(infoRect.left, infoRect.right, (LONG)ST_LIST_CTRL_MARGIN);
    TakeMargin(infoRect.top, infoRect.bottom, (LONG)ST_LIST_CTRL_MARGIN);

    if (!m_info.Create(WS_CHILD, infoRect, this, IDC_INFO_GRID))
    {
        TRACE0("Failed create list ctrl\n");
        ASSERT(FALSE);
        return FALSE;
    }

    m_info.SetScrollBarsStyle(CBCGPScrollBar::BCGP_SBSTYLE_VISUAL_MANAGER);
    m_info.ShowWindow(SW_SHOW);

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
        m_quantity.GetWindowText(quantityStr);
        int quantity = _ttoi(quantityStr);

        quantity = clamp(quantity - ST_SPIN_ACELL, ST_MIN_QUANTITY, ST_MAX_QUANTITY);

        quantityStr.Format(_T("%d"), quantity);

        m_quantity.SetWindowText(quantityStr);
    }
    else if (pNMUpDown->iDelta == 1) // up
    {
        CString quantityStr;
        m_quantity.GetWindowText(quantityStr);
        int quantity = _ttoi(quantityStr);

        quantity = clamp(quantity + ST_SPIN_ACELL, ST_MIN_QUANTITY, ST_MAX_QUANTITY);

        quantityStr.Format(_T("%d"), quantity);

        m_quantity.SetWindowText(quantityStr);
    }

    *pResult = 0;
}


