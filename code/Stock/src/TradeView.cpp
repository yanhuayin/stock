#include "stdafx.h"
#include "TradeView.h"

CTradeView::CTradeView()
    : m_tradeWnd(this)
{
}

CTradeView::~CTradeView()
{
}

BEGIN_MESSAGE_MAP(CTradeView, CBCGPDockingControlBar)
    ON_WM_CREATE()
END_MESSAGE_MAP()


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

    this->EnableDocking(CBRS_ALIGN_ANY);
}



