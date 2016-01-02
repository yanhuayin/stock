#include "stdafx.h"
#include "resource.h"
#include "StockMainFrm.h"

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
    return 0;
}

void CStockMainFrame::OnTradeNew()
{
}

void CStockMainFrame::OnTradeLocate()
{
}

void CStockMainFrame::OnTradeSettings()
{
}

void CStockMainFrame::OnClose()
{
}

BOOL CStockMainFrame::CreateDockingBars()
{
    return 0;
}

void CStockMainFrame::SetDockingBarsIcons(BOOL bHiColorIcons)
{
}
