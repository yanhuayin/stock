#include "stdafx.h"
#include "resource.h"
#include "Utils.h"
#include "TradeView.h"
#include "StockPropSheet.h"
#include "StockMainFrm.h"

#define ST_TRADE_VIEW_WIDTH     312
#define ST_TRADE_VIEW_HEIGHT    484

#define ST_MAIN_FRAME_DEFAULT_WIDTH     577
#define ST_MAIN_FRAME_DEFAULT_HEIGHT    919

IMPLEMENT_DYNAMIC(CStockMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CStockMainFrame, CMDIFrameWnd)
    ON_WM_CREATE()
    ON_COMMAND(ID_TRADE_NEW, OnTradeNew)
    ON_COMMAND(ID_TRADE_LOCAT, OnTradeLocate)
    ON_COMMAND(ID_TRADE_SETTING, OnTradeSettings)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

CStockMainFrame::CStockMainFrame()
{
    CBCGPPopupMenu::SetForceShadow(TRUE);
}

CStockMainFrame::~CStockMainFrame()
{
}

BOOL CStockMainFrame::PreCreateWindow(CREATESTRUCT & cs)
{
    if (!CMDIFrameWnd::PreCreateWindow(cs))
        return FALSE;

    cs.cx = ST_MAIN_FRAME_DEFAULT_WIDTH;
    cs.cy = ST_MAIN_FRAME_DEFAULT_HEIGHT;

    return TRUE;
}

BOOL CStockMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd * pParentWnd, CCreateContext * pContext)
{
    if (!CMDIFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
    {
        return FALSE;
    }

    return TRUE;
}

#ifdef _DEBUG
void CStockMainFrame::AssertValid() const
{
}

void CStockMainFrame::Dump(CDumpContext & dc) const
{
}
#endif

int CStockMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    CBCGPToolbarComboBoxButton::SetFlatMode();
    CBCGPToolBar::EnableQuickCustomization();

    //create menu
    if (!m_menu.Create(this))
    {
        TRACE0("Failed to create main frame menu\n");
        ASSERT(FALSE);
        return -1;
    }

    m_menu.SetBarStyle(m_menu.GetBarStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

    CBCGPPopupMenu::SetForceMenuFocus(FALSE);

    //create main tool bar
    if (!m_toolbar.CreateEx(this, TBSTYLE_FLAT,
        WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_toolbar.LoadToolBar(IDR_MAIN_MENU))
    {
        TRACE0("Failed to create trade tool bar\n");
        ASSERT(FALSE);
        return -1;
    }

    CString toolbarTitle;
    toolbarTitle.LoadString(IDS_TRADE_TOOLBAR_TITLE);
    m_toolbar.SetWindowText(toolbarTitle);
    m_toolbar.SetPermament();

    // TODO : status bar

    // TODO : fake visual studio 201x docking
    this->EnableDocking(CBRS_ALIGN_ANY);

    // TODO : window manager and window menu

    // dock bars
    m_menu.EnableDocking(CBRS_ALIGN_TOP);
    m_toolbar.EnableDocking(CBRS_ALIGN_TOP);
    this->DockControlBar(&m_menu);
    this->DockControlBar(&m_toolbar);

    // reset tool image
    CBCGPToolBar::ResetAllImages();

    if (!m_toolbar.LoadBitmap(IDB_TRADE_TOOLBAR))
    {
        TRACE0("Failed to load toolbar image\n");
        ASSERT(false);
        return -1;
    }

    return 0;
}

void CStockMainFrame::OnTradeNew()
{
    TradeViewPtr pView(new CTradeView());

    CString title;
    title.LoadString(IDS_TRADE_WND_TITLE);

    title.AppendFormat(_T("%d"), m_views.size() + 1);

    const CRect defRect(0, 0, ST_TRADE_VIEW_WIDTH, ST_TRADE_VIEW_HEIGHT);
    if (!pView->Create(title, this, defRect, TRUE,
        ID_TRADE_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_ALIGN_TOP | CBRS_FLOAT_MULTI))
    {
        TRACE0("Failed to create tade window\n");
        ASSERT(FALSE);
        return;
    }
    m_views.push_back(pView);

    //this->DockControlBar(pView.get(), NULL, &defRect);

    CRect clientRect;
    this->GetClientRect(&clientRect);
    CPoint center((clientRect.left + clientRect.right) / 2, (clientRect.top + clientRect.bottom) / 2);

    ::ClientToScreen(m_hWnd, &center);
    
    CBCGPBaseControlBar *pTarget = this->ControlBarFromPoint(center, FALSE, TRUE, pView->GetRuntimeClass());
    if (pTarget)
    {
        pView->AttachToTabWnd(DYNAMIC_DOWNCAST(CTradeView, pTarget), BCGP_DM_SHOW);
    }
    else
    {
        this->DockControlBar(pView.get());

        if (m_menu.IsDocked())
        {
            clientRect.top += m_menu.CalcFixedLayout(FALSE, TRUE).cy;
        }

        if (m_toolbar.IsDocked())
        {
            clientRect.top += m_menu.CalcFixedLayout(FALSE, TRUE).cy;
        }

        pView->SetWindowPos(NULL, clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
    }

    pView->AdjustLayout();

    // TODO : dock bar icon
}

void CStockMainFrame::OnTradeLocate()
{
    CStockPropSheet prop;
    prop.SetActivePage(&(prop._locatePage));
    prop.DoModal();
}

void CStockMainFrame::OnTradeSettings()
{
    CStockPropSheet prop;
    prop.SetActivePage(&(prop._settingsPage));
    prop.DoModal();
}

void CStockMainFrame::OnClose()
{
    // TODO :  release views memory

    CMDIFrameWnd::OnClose();
}

