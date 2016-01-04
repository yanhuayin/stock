#include "stdafx.h"
#include "TradeView.h"

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

    m_tradeWnd._q1PlusClickEvent = std::bind(&CTradeView::OnQ1PlusClicked, this);
    m_tradeWnd._q1MinusClickEvent = std::bind(&CTradeView::OnQ1MinusClicked, this);

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

void CTradeView::OnQ1PlusClicked()
{
    MessageBox(_T("fuck"), MB_OK);
}

void CTradeView::OnQ1MinusClicked()
{
    MessageBox(_T("shit"), MB_OK);
}


