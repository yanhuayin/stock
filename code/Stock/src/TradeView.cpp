#include "stdafx.h"
#include "TradeView.h"
#include "TradeControl.h"

BEGIN_MESSAGE_MAP(CTradeView, CBCGPDockingControlBar)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()


CTradeView::CTradeView()
    : m_tradeWnd(this)
{
}

CTradeView::~CTradeView()
{
}

int CTradeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_tradeWnd.Create(CTradeWnd::IDD, this))
    {
        TRACE0("Failed to create trade dialog\n");
        ASSERT(FALSE);

        return -1;
    }

    //m_tradeWnd._q1PlusClickEvent = std::bind(&CTradeView::OnQ1PlusClicked, this);
    //m_tradeWnd._q1MinusClickEvent = std::bind(&CTradeView::OnQ1MinusClicked, this);
    m_tradeWnd._enterOKEvent = std::bind(&CTradeView::OnOK, this);

    m_tradeWnd.ShowWindow(SW_SHOW);
    m_tradeWnd.UpdateWindow();

    m_tradeWnd.UpdateData(FALSE);

    this->EnableDocking(CBRS_ALIGN_TOP);

    return 0;
}

void CTradeView::OnSize(UINT nType, int cx, int cy)
{
    CBCGPDockingControlBar::OnSize(nType, cx, cy);

    this->AdjustLayout();
}

void CTradeView::AdjustLayout()
{
    if (GetSafeHwnd() == NULL)
    {
        return;
    }

    CRect rectClient;
    this->GetClientRect(rectClient);

    m_tradeWnd.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

    m_tradeWnd.Invalidate(TRUE);
    m_tradeWnd.UpdateWindow();
}

void CTradeView::SetName(CString const & name)
{
    m_tradeWnd._name.SetWindowText(name);
}

void CTradeView::SetInfo(StockInfoField field, InfoNumArrayPtr info)
{
    CTradeWnd::StockInfoCol col;
    switch (field)
    {
    case SIF_Price:
        col = CTradeWnd::SIC_Price;
        break;
    case SIF_Quant:
        col = CTradeWnd::SIC_Quant;
        break;
    default:
        return;
    }

    InfoNumArray &arr = *info;

    for (std::size_t i = 0; i < SIT_Num; ++i)
    {
        CBCGPGridRow *pRow = m_tradeWnd._info.GetRow(i);
        if (pRow)
        {
            CString val;
            val.Format(_T("%f"), arr[i]);
            pRow->GetItem(col)->SetValue(val.GetString());
        }
    }
}

void CTradeView::OnOK()
{
    // request stock info
    CString code;
    m_tradeWnd._code.GetWindowText(code);
    if (!code.IsEmpty())
    {
        CTradeControl::Instance().RequestInfo(shared_from_this(), code);
    }
}

void CTradeView::PostNcDestroy()
{
    // ok, let control remove this view
    CBCGPDockingControlBar::PostNcDestroy();

    CTradeControl &tctl = CTradeControl::Instance();
    if (!tctl.IsClose())
    {
        tctl.ViewClosed(shared_from_this());
    }
}

//void CTradeView::OnQ1PlusClicked()
//{
//    MessageBox(_T("fuck"), MB_OK);
//}
//
//void CTradeView::OnQ1MinusClicked()
//{
//    MessageBox(_T("shit"), MB_OK);
//}


