#include "stdafx.h"
#include "resource.h"
#include "Stock.h"


BEGIN_MESSAGE_MAP(CStockApp, CBCGPWinApp)
END_MESSAGE_MAP()

CStockApp theApp;

CStockApp::CStockApp()
{
    this->SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2013_DARK_GRAY);
    this->EnableMouseWheelInInactiveWindow(TRUE);
}

CStockApp::~CStockApp()
{
}

BOOL CStockApp::InitInstance()
{
    CBCGPWinApp::InitInstance();

    globalData.m_bUseVisualManagerInBuiltInDialogs = TRUE;

    if (!AfxOleInit())
    {
        AfxMessageBox(IDS_OLE_INIT_FAILED);
        return FALSE;
    }


}
