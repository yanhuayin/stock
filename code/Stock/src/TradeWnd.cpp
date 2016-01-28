#include "stdafx.h"
//#include <type_traits>
#include "resource.h"
#include "Utils.h"
#include "StockGlobal.h"
#include "StockConfig.h"
#include "TradeWnd.h"

#define ST_DIGIT_ONLY_STR           _T("1234567890")
#define ST_DEFAULT_QUANTITY_STR     _T("0")

#define ST_SPIN_ACELL               1000

#define ST_LIST_CTRL_MARGIN         5

#define ST_BUY_BTN_TXT              _T("+")
#define ST_SELL_BTN_TXT             _T("-")

UINT s_info_names [] =
{
    IDS_INFO_SELL_10,
    IDS_INFO_SELL_9,
    IDS_INFO_SELL_8,
    IDS_INFO_SELL_7,
    IDS_INFO_SELL_6,
    IDS_INFO_SELL_5,
    IDS_INFO_SELL_4,
    IDS_INFO_SELL_3,
    IDS_INFO_SELL_2,
    IDS_INFO_SELL_1,

    IDS_INFO_BUY_1,
    IDS_INFO_BUY_2,
    IDS_INFO_BUY_3,
    IDS_INFO_BUY_4,
    IDS_INFO_BUY_5,
    IDS_INFO_BUY_6,
    IDS_INFO_BUY_7,
    IDS_INFO_BUY_8,
    IDS_INFO_BUY_9,
    IDS_INFO_BUY_10
};

UINT s_sell_btn_ids[] =
{
    IDC_SELL_SELL_10,
    IDC_SELL_SELL_9,
    IDC_SELL_SELL_8,
    IDC_SELL_SELL_7,
    IDC_SELL_SELL_6,
    IDC_SELL_SELL_5,
    IDC_SELL_SELL_4,
    IDC_SELL_SELL_3,
    IDC_SELL_SELL_2,
    IDC_SELL_SELL_1,

    IDC_SELL_BUY_1,
    IDC_SELL_BUY_2,
    IDC_SELL_BUY_3,
    IDC_SELL_BUY_4,
    IDC_SELL_BUY_5,
    IDC_SELL_BUY_6,
    IDC_SELL_BUY_7,
    IDC_SELL_BUY_8,
    IDC_SELL_BUY_9,
    IDC_SELL_BUY_10,
};

UINT s_buy_btn_ids[] =
{
    IDC_BUY_SELL_10,
    IDC_BUY_SELL_9,
    IDC_BUY_SELL_8,
    IDC_BUY_SELL_7,
    IDC_BUY_SELL_6,
    IDC_BUY_SELL_5,
    IDC_BUY_SELL_4,
    IDC_BUY_SELL_3,
    IDC_BUY_SELL_2,
    IDC_BUY_SELL_1,

    IDC_BUY_BUY_1,
    IDC_BUY_BUY_2,
    IDC_BUY_BUY_3,
    IDC_BUY_BUY_4,
    IDC_BUY_BUY_5,
    IDC_BUY_BUY_6,
    IDC_BUY_BUY_7,
    IDC_BUY_BUY_8,
    IDC_BUY_BUY_9,
    IDC_BUY_BUY_10
};

BEGIN_MESSAGE_MAP(CTradeWnd, CBCGPDialog)
    ON_BN_CLICKED(IDC_QTOP1_BTI, OnQ1PlusClicked)
    ON_BN_CLICKED(IDC_QTOP1_BTR, OnQ1MinusClicked)
    ON_BN_CLICKED(IDC_QTOP2_BTI, OnQ2PlusClicked)
    ON_BN_CLICKED(IDC_QTOP2_BTR, OnQ2MinusClicked)
    ON_BN_CLICKED(IDC_DOUBLE_BTN, OnQDoubleClicked)
    ON_BN_CLICKED(IDC_HALF_BTN, OnQHalfClicked)
    ON_NOTIFY(UDN_DELTAPOS, IDC_QUANTITY_SPIN, OnDeltPosSpinCtrl)
    ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
    ON_COMMAND(IDOK, OnEnter)
    ON_COMMAND(IDC_CANCEL_ORDER_BTN, OnCancelOrder)
    ON_COMMAND(IDC_SELL_SELL_10, OnSellSell10)
    ON_COMMAND(IDC_SELL_SELL_9, OnSellSell9)
    ON_COMMAND(IDC_SELL_SELL_8, OnSellSell8)
    ON_COMMAND(IDC_SELL_SELL_7, OnSellSell7)
    ON_COMMAND(IDC_SELL_SELL_6, OnSellSell6)
    ON_COMMAND(IDC_SELL_SELL_5, OnSellSell5)
    ON_COMMAND(IDC_SELL_SELL_4, OnSellSell4)
    ON_COMMAND(IDC_SELL_SELL_3, OnSellSell3)
    ON_COMMAND(IDC_SELL_SELL_2, OnSellSell2)
    ON_COMMAND(IDC_SELL_SELL_1, OnSellSell1)
    ON_COMMAND(IDC_SELL_BUY_1, OnSellBuy1)
    ON_COMMAND(IDC_SELL_BUY_2, OnSellBuy2)
    ON_COMMAND(IDC_SELL_BUY_3, OnSellBuy3)
    ON_COMMAND(IDC_SELL_BUY_4, OnSellBuy4)
    ON_COMMAND(IDC_SELL_BUY_5, OnSellBuy5)
    ON_COMMAND(IDC_SELL_BUY_6, OnSellBuy6)
    ON_COMMAND(IDC_SELL_BUY_7, OnSellBuy7)
    ON_COMMAND(IDC_SELL_BUY_8, OnSellBuy8)
    ON_COMMAND(IDC_SELL_BUY_9, OnSellBuy9)
    ON_COMMAND(IDC_SELL_BUY_10, OnSellBuy10)
    ON_COMMAND(IDC_BUY_SELL_10, OnBuySell10)
    ON_COMMAND(IDC_BUY_SELL_9, OnBuySell9)
    ON_COMMAND(IDC_BUY_SELL_8, OnBuySell8)
    ON_COMMAND(IDC_BUY_SELL_7, OnBuySell7)
    ON_COMMAND(IDC_BUY_SELL_6, OnBuySell6)
    ON_COMMAND(IDC_BUY_SELL_5, OnBuySell5)
    ON_COMMAND(IDC_BUY_SELL_4, OnBuySell4)
    ON_COMMAND(IDC_BUY_SELL_3, OnBuySell3)
    ON_COMMAND(IDC_BUY_SELL_2, OnBuySell2)
    ON_COMMAND(IDC_BUY_SELL_1, OnBuySell1)
    ON_COMMAND(IDC_BUY_BUY_1, OnBuyBuy1)
    ON_COMMAND(IDC_BUY_BUY_2, OnBuyBuy2)
    ON_COMMAND(IDC_BUY_BUY_3, OnBuyBuy3)
    ON_COMMAND(IDC_BUY_BUY_4, OnBuyBuy4)
    ON_COMMAND(IDC_BUY_BUY_5, OnBuyBuy5)
    ON_COMMAND(IDC_BUY_BUY_6, OnBuyBuy6)
    ON_COMMAND(IDC_BUY_BUY_7, OnBuyBuy7)
    ON_COMMAND(IDC_BUY_BUY_8, OnBuyBuy8)
    ON_COMMAND(IDC_BUY_BUY_9, OnBuyBuy9)
    ON_COMMAND(IDC_BUY_BUY_10, OnBuyBuy10)
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

CBCGPGridRow * CTradeWnd::AddOrderRow()
{
    CBCGPGridRow *pRow = _order.CreateRow(TOC_Num);
    for (int i = 0; i < TOC_Num; ++i)
        pRow->GetItem(i)->AllowEdit(FALSE);

    pRow->ReplaceItem(TOC_Cancel, new CButtonItem(ST_SELL_BTN_TXT, IDC_CANCEL_ORDER_BTN));

    CString name;
    _name.GetWindowText(name);
    pRow->GetItem(TOC_Name)->SetValue(name.GetString());

    _order.AddRow(pRow, TRUE);
    
    return pRow;
}

void CTradeWnd::GetPrice(StockInfoType info, CString & outPrice) const
{
    CBCGPGridRow *pRow = _info.GetRow(info);
    if (pRow)
    {
        outPrice = pRow->GetItem(SIC_Price)->GetValue();
    }
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
    _info.EnableMarkSortedColumn(FALSE);
    _info.EnableRowHeader(TRUE);
    _info.SetSingleSel(TRUE);

    _info.InsertColumn(SIC_OpBuy, _T(""), 20);
    _info.InsertColumn(SIC_Name, CString(MAKEINTRESOURCE(IDS_INFO_NAME_COL)), 30);
    _info.InsertColumn(SIC_Price, CString(MAKEINTRESOURCE(IDS_INFO_PRICE_COL)), 75);
    _info.InsertColumn(SIC_Quant, CString(MAKEINTRESOURCE(IDS_INFO_SELL_QUANT_COL)), 75);
    _info.InsertColumn(SIC_OpSell, _T(""), 20);

    _info.SetHeaderAlign(SIC_Name, HDF_CENTER);
    _info.SetColumnAlign(SIC_Name, HDF_CENTER);
    _info.SetHeaderAlign(SIC_Price, HDF_RIGHT);
    _info.SetColumnAlign(SIC_Price, HDF_RIGHT);
    _info.SetHeaderAlign(SIC_Quant, HDF_RIGHT);
    _info.SetColumnAlign(SIC_Quant, HDF_RIGHT);

    //int col = _info.GetColumnCount();
    for (int i = 0; i < SIT_Num; ++i)
    {
        CBCGPGridRow *pRow = _info.CreateRow(SIC_Num);
        for (int j = 0; j < SIC_Num; ++j)
            pRow->GetItem(j)->AllowEdit(FALSE);

        pRow->ReplaceItem(SIC_OpBuy, new CButtonItem(ST_BUY_BTN_TXT, s_buy_btn_ids[i]));
        pRow->GetItem(SIC_Name)->SetValue(CString(MAKEINTRESOURCE(s_info_names[i])).GetString());
        pRow->ReplaceItem(SIC_OpSell, new CButtonItem(ST_SELL_BTN_TXT, s_sell_btn_ids[i]));

        _info.AddRow(pRow, FALSE);
    }

    CRect orderRect;
    m_orderRect.GetClientRect(&orderRect);
    m_orderRect.MapWindowPoints(this, &orderRect);
    _order.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, orderRect, this, IDC_ORDER_GRID);
    _order.EnableColumnAutoSize(TRUE);
    _order.SetWholeRowSel();
    _order.EnableHeader(TRUE, 0);
    _order.SetScrollBarsStyle(CBCGPScrollBar::BCGP_SBSTYLE_VISUAL_MANAGER);
    _order.EnableMarkSortedColumn(FALSE);
    _order.EnableRowHeader(TRUE);
    _order.SetSingleSel(TRUE);

    _order.InsertColumn(TOC_Cancel, _T(""), 10);
    _order.InsertColumn(TOC_Local, CString(MAKEINTRESOURCE(IDS_ORDER_LOCAL_TIME_COL)), 30);
    _order.InsertColumn(TOC_Time, CString(MAKEINTRESOURCE(IDS_ORDER_TIME_COL)), 30);
    _order.InsertColumn(TOC_Code, CString(MAKEINTRESOURCE(IDS_ORDER_CODE_COL)), 20);
    _order.InsertColumn(TOC_Name, CString(MAKEINTRESOURCE(IDS_ORDER_NAME_COL)), 20);
    _order.InsertColumn(TOC_Flag, CString(MAKEINTRESOURCE(IDS_ORDER_FLAG_COL)), 20);
    _order.InsertColumn(TOC_Price, CString(MAKEINTRESOURCE(IDS_ORDER_PRICE_COL)), 30);
    _order.InsertColumn(TOC_Quant, CString(MAKEINTRESOURCE(IDS_ORDER_QUANT_COL)), 30);
    _order.InsertColumn(TOC_Id, CString(MAKEINTRESOURCE(IDS_ORDER_ID_COL)), 20);
    _order.InsertColumn(TOC_Turnover, CString(MAKEINTRESOURCE(IDS_ORDER_TURNOVER_COL)), 30);

    return TRUE;
}

void CTradeWnd::OnQ1PlusClicked()
{
    CString q1Str;
    _q1.GetWindowText(q1Str);
    if (!q1Str.IsEmpty())
    {
        int q1 = _ttoi(q1Str);

        CString quantityStr;
        _quantity.GetWindowText(quantityStr);
        int quantity = 0;
        if (!quantityStr.IsEmpty())
            quantity = _ttoi(quantityStr);

        quantity = clamp(quantity + q1, ST_MIN_QUANTITY, ST_MAX_QUANTITY);

        quantityStr.Format(_T("%d"), quantity);

        _quantity.SetWindowText(quantityStr);
    }
}

void CTradeWnd::OnQ1MinusClicked()
{
    CString q1Str;
    _q1.GetWindowText(q1Str);
    if (!q1Str.IsEmpty())
    {
        int q1 = _ttoi(q1Str);

        CString quantityStr;
        _quantity.GetWindowText(quantityStr);
        int quantity = 0;
        if (!quantityStr.IsEmpty())
            quantity = _ttoi(quantityStr);

        quantity = clamp(quantity - q1, ST_MIN_QUANTITY, ST_MAX_QUANTITY);

        quantityStr.Format(_T("%d"), quantity);

        _quantity.SetWindowText(quantityStr);
    }
}

void CTradeWnd::OnQ2PlusClicked()
{
    CString q2Str;
    _q2.GetWindowText(q2Str);
    if (!q2Str.IsEmpty())
    {
        int q2 = _ttoi(q2Str);

        CString quantityStr;
        _quantity.GetWindowText(quantityStr);
        int quantity = 0;
        if (!quantityStr.IsEmpty())
            quantity = _ttoi(quantityStr);

        quantity = clamp(quantity + q2, ST_MIN_QUANTITY, ST_MAX_QUANTITY);

        quantityStr.Format(_T("%d"), quantity);

        _quantity.SetWindowText(quantityStr);
    }
}

void CTradeWnd::OnQ2MinusClicked()
{
    CString q2Str;
    _q2.GetWindowText(q2Str);
    if (!q2Str.IsEmpty())
    {
        int q2 = _ttoi(q2Str);

        CString quantityStr;
        _quantity.GetWindowText(quantityStr);
        int quantity = 0;
        if (!quantityStr.IsEmpty())
            quantity = _ttoi(quantityStr);

        quantity = clamp(quantity - q2, ST_MIN_QUANTITY, ST_MAX_QUANTITY);

        quantityStr.Format(_T("%d"), quantity);

        _quantity.SetWindowText(quantityStr);
    }
}

void CTradeWnd::OnQDoubleClicked()
{
    CString quantityStr;
    _quantity.GetWindowText(quantityStr);
    if (!quantityStr.IsEmpty())
    {
        int quantity = _ttoi(quantityStr);
        quantity = clamp(quantity * 2, ST_MIN_QUANTITY, ST_MAX_QUANTITY);
        quantityStr.Format(_T("%d"), quantity);

        _quantity.SetWindowText(quantityStr);
    }
}

void CTradeWnd::OnQHalfClicked()
{
    CString quantityStr;
    _quantity.GetWindowText(quantityStr);
    if (!quantityStr.IsEmpty())
    {
        int quantity = _ttoi(quantityStr);
        quantity = clamp(quantity / 2, ST_MIN_QUANTITY, ST_MAX_QUANTITY);
        quantityStr.Format(_T("%d"), quantity);

        _quantity.SetWindowText(quantityStr);
    }
}

void CTradeWnd::OnDeltPosSpinCtrl(NMHDR * pNMHDR, LRESULT * pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    if (pNMUpDown->iDelta == -1) // down
    {
        CString quantityStr;
        _quantity.GetWindowText(quantityStr);
        int quantity = 0;
        if (!quantityStr.IsEmpty())
            quantity = _ttoi(quantityStr);

        quantity = clamp(quantity - ST_SPIN_ACELL, ST_MIN_QUANTITY, ST_MAX_QUANTITY);

        quantityStr.Format(_T("%d"), quantity);

        _quantity.SetWindowText(quantityStr);
    }
    else if (pNMUpDown->iDelta == 1) // up
    {
        CString quantityStr;
        _quantity.GetWindowText(quantityStr);
        int quantity = 0;
        if (!quantityStr.IsEmpty())
            quantity = _ttoi(quantityStr);

        quantity = clamp(quantity + ST_SPIN_ACELL, ST_MIN_QUANTITY, ST_MAX_QUANTITY);

        quantityStr.Format(_T("%d"), quantity);

        _quantity.SetWindowText(quantityStr);
    }

    *pResult = 0;
}

void CTradeWnd::OnEnter()
{
    this->UpdateData();

    if (_enterOKEvent)
    {
        _enterOKEvent();
    }
}

void CTradeWnd::OnCancelOrder()
{
    CBCGPGridRow *pRow = _order.GetCurSel();
    if (pRow)
    {
        if (_cancelOrderEvent)
        {
            _cancelOrderEvent(pRow);
        }
    }
}

void CTradeWnd::OnSellSell10()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell10, STO_Sell);
    }
}

void CTradeWnd::OnSellSell9()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell9, STO_Sell);
    }
}

void CTradeWnd::OnSellSell8()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell8, STO_Sell);
    }
}

void CTradeWnd::OnSellSell7()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell7, STO_Sell);
    }
}

void CTradeWnd::OnSellSell6()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell6, STO_Sell);
    }
}

void CTradeWnd::OnSellSell5()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell5, STO_Sell);
    }
}

void CTradeWnd::OnSellSell4()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell4, STO_Sell);
    }
}

void CTradeWnd::OnSellSell3()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell3, STO_Sell);
    }
}

void CTradeWnd::OnSellSell2()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell2, STO_Sell);
    }
}

void CTradeWnd::OnSellSell1()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell1, STO_Sell);
    }
}

void CTradeWnd::OnSellBuy1()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy1, STO_Sell);
    }
}

void CTradeWnd::OnSellBuy2()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy2, STO_Sell);
    }
}

void CTradeWnd::OnSellBuy3()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy3, STO_Sell);
    }
}

void CTradeWnd::OnSellBuy4()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy4, STO_Sell);
    }
}

void CTradeWnd::OnSellBuy5()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy5, STO_Sell);
    }
}

void CTradeWnd::OnSellBuy6()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy6, STO_Sell);
    }
}

void CTradeWnd::OnSellBuy7()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy7, STO_Sell);
    }
}

void CTradeWnd::OnSellBuy8()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy8, STO_Sell);
    }
}

void CTradeWnd::OnSellBuy9()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy9, STO_Sell);
    }
}

void CTradeWnd::OnSellBuy10()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy10, STO_Sell);
    }
}

void CTradeWnd::OnBuySell10()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell10, STO_Buy);
    }
}

void CTradeWnd::OnBuySell9()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell9, STO_Buy);
    }
}

void CTradeWnd::OnBuySell8()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell8, STO_Buy);
    }
}

void CTradeWnd::OnBuySell7()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell7, STO_Buy);
    }
}

void CTradeWnd::OnBuySell6()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell6, STO_Buy);
    }
}

void CTradeWnd::OnBuySell5()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell5, STO_Buy);
    }
}

void CTradeWnd::OnBuySell4()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell4, STO_Buy);
    }
}

void CTradeWnd::OnBuySell3()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell3, STO_Buy);
    }
}

void CTradeWnd::OnBuySell2()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell2, STO_Buy);
    }
}

void CTradeWnd::OnBuySell1()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Sell1, STO_Buy);
    }
}

void CTradeWnd::OnBuyBuy1()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy1, STO_Buy);
    }
}

void CTradeWnd::OnBuyBuy2()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy2, STO_Buy);
    }
}

void CTradeWnd::OnBuyBuy3()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy3, STO_Buy);
    }
}

void CTradeWnd::OnBuyBuy4()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy4, STO_Buy);
    }
}

void CTradeWnd::OnBuyBuy5()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy5, STO_Buy);
    }
}

void CTradeWnd::OnBuyBuy6()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy6, STO_Buy);
    }
}

void CTradeWnd::OnBuyBuy7()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy7, STO_Buy);
    }
}

void CTradeWnd::OnBuyBuy8()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy8, STO_Buy);
    }
}

void CTradeWnd::OnBuyBuy9()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy9, STO_Buy);
    }
}

void CTradeWnd::OnBuyBuy10()
{
    this->UpdateData();
    if (_tradeEvent)
    {
        _tradeEvent(SIT_Buy10, STO_Buy);
    }
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

CButtonItem::CButtonItem(LPCTSTR lpszText, UINT id)
    : CBCGPGridItem(lpszText)
    , m_id(id)
{
}

void CButtonItem::OnDrawValue(CDC * pDC, CRect rect)
{
    ASSERT_VALID(pDC);

    this->OnFillBackground(pDC, rect);

    rect.DeflateRect(1, 1);

    COLORREF clrText = globalData.clrBarText;

    CBCGPVisualManager::GetInstance()->OnDrawPushButton(pDC, rect, NULL, clrText);


    COLORREF clrTextOld = pDC->SetTextColor(clrText);

    CString strText = (LPCTSTR)(_bstr_t)m_varValue;
    pDC->DrawText(strText, rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

    pDC->SetTextColor(clrTextOld);
}

BOOL CButtonItem::OnClickValue(UINT uiMsg, CPoint point)
{
    if (uiMsg != WM_LBUTTONDOWN)
    {
        return CBCGPGridItem::OnClickValue(uiMsg, point);
    }

    CBCGPGridCtrl *pGridCtrl = this->GetOwnerList();
    ASSERT_VALID(pGridCtrl);

    pGridCtrl->GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(m_id, BN_CLICKED));

    return TRUE;
}
