#include "stdafx.h"
#include "TradeView.h"
#include "TradeControl.h"

IMPLEMENT_DYNCREATE(CTradeView, CBCGPDockingControlBar)

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
    m_tradeWnd._tradeEvent = std::bind(&CTradeView::OnTrade, this, std::placeholders::_1, std::placeholders::_2);
    m_tradeWnd._cancelOrderEvent = std::bind(&CTradeView::OnCancelOrder, this, std::placeholders::_1);

    UINT quota = CTradeControl::Instance().Quota();
    CString quotaStr;
    quotaStr.Format(_T("%u"), quota);
    m_tradeWnd._quota.SetWindowText(quotaStr);

    UINT left = CTradeControl::Instance().Left();
    CString leftStr;
    leftStr.Format(_T("%u"), left);
    m_tradeWnd._left.SetWindowText(leftStr);

    m_tradeWnd.ShowWindow(SW_SHOW);
    m_tradeWnd.UpdateWindow();

    m_tradeWnd.UpdateData(FALSE);

    //this->EnableDocking(CBRS_ALIGN_TOP);

    return 0;
}

void CTradeView::OnSize(UINT nType, int cx, int cy)
{
    CBCGPDockingControlBar::OnSize(nType, cx, cy);

    if (m_tradeWnd.GetSafeHwnd())
    {
        m_tradeWnd.SetWindowPos(nullptr, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        m_tradeWnd.Invalidate();
        m_tradeWnd.UpdateWindow();
    }
}

void CTradeView::SetName(CString const & name)
{
    m_tradeWnd._name.SetWindowText(name);
}

void CTradeView::SetInfo(StockInfoField field, InfoNumArray const& info)
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

    for (std::size_t i = 0; i < SIT_Num; ++i)
    {
        CBCGPGridRow *pRow = m_tradeWnd._info.GetRow(i);
        if (pRow)
        {
            CString val;
            val.Format(_T("%f"), info[i]);
            pRow->GetItem(col)->SetValue(val.GetString());
        }
    }
}

void CTradeView::SetQuota(CString const & quota)
{
    m_tradeWnd._quota.SetWindowText(quota);
}

void CTradeView::SetLeft(CString const & left)
{
    m_tradeWnd._left.SetWindowText(left);
}

void CTradeView::ViewOrder(int order, ViewOrderData const & data)
{
    CBCGPGridRow *pRow = m_tradeWnd.AddOrderRow();

    pRow->GetItem(CTradeWnd::TOC_Local)->SetValue(data.local.GetString());
    pRow->GetItem(CTradeWnd::TOC_Time)->SetValue(data.time.GetString());
    pRow->GetItem(CTradeWnd::TOC_Code)->SetValue(data.code.GetString());
    pRow->GetItem(CTradeWnd::TOC_Flag)->SetValue(data.flag.GetString());
    pRow->GetItem(CTradeWnd::TOC_Price)->SetValue(data.price.GetString());
    pRow->GetItem(CTradeWnd::TOC_Quant)->SetValue(data.quant.GetString());
    pRow->GetItem(CTradeWnd::TOC_Id)->SetValue(data.id.GetString());
    pRow->GetItem(CTradeWnd::TOC_Turnover)->SetValue(data.turnover.GetString());

    pRow->SetData(order);
}

//void CTradeView::GetCode(CString & outCode) const
//{
//    m_tradeWnd._code.GetWindowText(outCode);
//}

void CTradeView::GetQuant(CString & outQuant) const
{
    m_tradeWnd._quantity.GetWindowText(outQuant);
}

void CTradeView::GetPrice(StockInfoType info, CString & outPrice) const
{
    m_tradeWnd.GetPrice(info, outPrice);
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

void CTradeView::OnTrade(StockInfoType info, StockTradeOp op)
{
    CTradeControl::Instance().Trade(shared_from_this(), info, op);
}

void CTradeView::OnCancelOrder(CBCGPGridRow *pRow)
{
    StockOrderResult res = CTradeControl::Instance().CancelOrder(shared_from_this(), pRow->GetData());

    if (res == SOR_OK || res == SOR_Dealed || res == SOR_LeftOK)
    {
        m_tradeWnd._order.RemoveRow(pRow->GetRowId());

        if (res == SOR_Dealed)
        {
            AfxMessageBox(IDS_ORDER_DEALED);
        }

        return;
    }

    AfxMessageBox(IDS_ORDER_ERROR);
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

IMPLEMENT_DYNCREATE(CStockMDIChild, CBCGPMDIChildWnd)

BEGIN_MESSAGE_MAP(CStockMDIChild, CBCGPMDIChildWnd)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


void CStockMDIChild::OnDestroy()
{
    if (m_pTabbedControlBar != NULL && CWnd::FromHandlePermanent(m_pTabbedControlBar->GetSafeHwnd()) != NULL)
    {
        CWnd* pParent = m_pTabbedControlBar->GetParent();

        if (pParent == this && m_pMDIFrame != NULL && !m_pMDIFrame->IsClosing())
        {
            // destory embedded control bar
            ::DestroyWindow(m_pTabbedControlBar->GetSafeHwnd());
        }
        m_pTabbedControlBar = NULL;
    }

    CBCGPMDIChildWnd::OnDestroy();
}
