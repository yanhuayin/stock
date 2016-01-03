#include "stdafx.h"
#include "StockConfig.h"
#include "Stock.h"
#include "StockMainFrm.h"


BEGIN_MESSAGE_MAP(CStockApp, CBCGPWinApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
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

    AfxEnableControlContainer();

    // this will under HKEY_CURRENT_USER\\Software
    this->SetRegistryKey(ST_APP_REGISTRY_KEY);

    this->LoadStdProfileSettings();

    this->SetRegistryBase(ST_APP_REGISTRY_SECTION);

    CStockMainFrame *pMainFrame = new CStockMainFrame();

    this->EnableLoadWindowPlacement(FALSE);

    if (!pMainFrame->LoadFrame(IDR_MAIN_MENU))
        return FALSE;

    m_pMainWnd = pMainFrame;

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    this->ParseCommandLine(cmdInfo);

    // TODO : parse command line

    // Dispatch commands specified on the command line.  Will return FALSE if
    // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
    if (!this->ProcessShellCommand(cmdInfo))
        return FALSE;

    if (!this->ReloadWindowPlacement(pMainFrame))
    {
        pMainFrame->ShowWindow(m_nCmdShow);
        pMainFrame->UpdateWindow();
    }

    pMainFrame->SendMessageToDescendants(WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0, TRUE, TRUE);

    return TRUE;
}

void CStockApp::PreLoadState()
{
    // TODO : add context menu
}

void CStockApp::OnAppAbout()
{
}



