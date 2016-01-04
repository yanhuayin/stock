#include "stdafx.h"
#include "TradeWnd.h"

#define ST_DIGIT_ONLY_STR           _T("1234567890")
#define ST_DEFAULT_QUANTITY_STR     _T("0")
#define ST_MAX_QUANTITY             9999999
#define ST_MIN_QUANTITY             0
#define ST_SPIN_BASE                1000

BEGIN_MESSAGE_MAP(CTradeWnd, CBCGPDialog)
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

BOOL CTradeWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if (CBCGPDialog::OnCommand(wParam, lParam))
        return TRUE;

    HWND hWndCtrl = (HWND)lParam;

    if (hWndCtrl)
    {
        WORD nCode = HIWORD(wParam);
        //WORD nId = LOWORD(wParam);

        this->GetParent()->SendMessage(nCode, wParam, lParam);

        return TRUE;
    }

    return FALSE;
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
    m_quantity.SetWindowText(ST_DEFAULT_QUANTITY_STR);

    m_qSpin.SetRange32(ST_MIN_QUANTITY, ST_MAX_QUANTITY);
    m_qSpin.SetBase(ST_SPIN_BASE);

    m_q1.DisableMask();
    m_q1.SetValidChars(ST_DIGIT_ONLY_STR);
    m_q1.SetWindowText(ST_DEFAULT_QUANTITY_STR);

    m_q2.DisableMask();
    m_q2.SetValidChars(ST_DIGIT_ONLY_STR);
    m_q2.SetWindowText(ST_DEFAULT_QUANTITY_STR);

    return TRUE;
}


