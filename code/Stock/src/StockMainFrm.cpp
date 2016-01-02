#include "stdafx.h"
#include "resource.h"
#include "Utils.h"
#include "TradeView.h"
#include "StockMainFrm.h"

#define ST_TRADE_VIEW_WIDTH     312
#define ST_TRADE_VIEW_HEIGHT    484

UINT s_trade_toolbar_img[] =
{
    IDI_NEW,
    IDI_LOCATE,
    IDI_SETTINGS
};

UINT s_trade_toolbar_cmd[] =
{
    ID_TRADE_NEW,
    ID_TRADE_LOCAT,
    ID_TRADE_SETTING
};

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
    return CMDIFrameWnd::PreCreateWindow(cs);
}

BOOL CStockMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd * pParentWnd, CCreateContext * pContext)
{
    return 0;
}

BOOL CStockMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
    return 0;
}

void CStockMainFrame::AssertValid() const
{
}

void CStockMainFrame::Dump(CDumpContext & dc) const
{
}

int CStockMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    CBCGPToolbarComboBoxButton::SetFlatMode();

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
    CImageList imageList;
    imageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);

    CBitmap img;
    for (size_t i = 0; i < ST_ARRAY_SIZE(s_trade_toolbar_img); ++i)
    {
        img.LoadBitmap(s_trade_toolbar_img[i]);
        imageList.Add(&img, RGB(255, 255, 255));
        img.DeleteObject();
    }

    if (!m_toolbar.CreateEx(this, TBSTYLE_FLAT,
        WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_toolbar.LoadToolBar(IDR_TRADE_TOOLBAR))
    {
        TRACE0("Failed to create trade tool bar\n");
        ASSERT(FALSE);
        return -1;
    }

    m_toolbar.LoadBitmap(IDB_TRADE_TOOLBAR);

    CString toolbarTitle;
    toolbarTitle.LoadString(IDS_TRADE_TOOLBAR_TITLE);
    m_toolbar.SetWindowText(toolbarTitle);

    // TODO : status bar

    // TODO : fake visual studio 201x docking
    this->EnableDocking(CBRS_ALIGN_ANY);

    // TODO : window manager and window menu

    return 0;
}

void CStockMainFrame::OnTradeNew()
{
    CTradeView *pView = new CTradeView();


    CString title;
    title.LoadString(IDS_TRADE_WND_TITLE);

    title.AppendFormat(_T("%d"), m_views.size() + 1);

    if (!pView->Create(title, this, CRect(0, 0, ST_TRADE_VIEW_WIDTH, ST_TRADE_VIEW_HEIGHT), TRUE,
        ID_TRADE_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
    {
        TRACE0("Failed to create tade window\n");
        ASSERT(FALSE);
        
        // TODO :
        ST_SAFE_DELETE(pView);
        return;
    }

    m_views.push_back(pView);

    pView->EnableDocking(CBRS_ALIGN_ANY);

    this->DockControlBar(pView);

    // TODO : dock bar icon
}

void CStockMainFrame::OnTradeLocate()
{
}

void CStockMainFrame::OnTradeSettings()
{
}

void CStockMainFrame::OnClose()
{
    // TODO :  release views memory

    CMDIFrameWnd::OnClose();
}

