#include "stdafx.h"
#include "resource.h"
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
    //this->SetRegistryKey(ST_APP_REGISTRY_KEY);

    //this->LoadStdProfileSettings();

    //this->SetRegistryBase(ST_APP_REGISTRY_SECTION);

    // load data
    if (!m_data.Load())
        return FALSE;

    if (!m_data.LocateData().Load(m_data.LocFile()))
        return FALSE;

    if (!m_data.TradeSettingsData().Load(m_data.SetFile()))
        return FALSE;

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

    if (!m_data.LocateData().IsReady())
    {
        pMainFrame->PostMessage(WM_COMMAND, MAKEWPARAM(ID_TRADE_LOCAT, 0), 0);
    }
    else if (!m_data.TradeSettingsData().IsRead())
    {
        pMainFrame->PostMessage(WM_COMMAND, MAKEWPARAM(ID_TRADE_SETTING, 0), 0);
    }


    return TRUE;
}

BOOL CStockApp::LoadState(LPCTSTR lpszSectionName, CBCGPFrameImpl * pFrameImpl)
{
    // override this function to avoid some crash when load app
    return TRUE;
}

BOOL CStockApp::SaveState(LPCTSTR lpszSectionName, CBCGPFrameImpl * pFrameImpl)
{
    // override this function because we overrided loadstate function
    return TRUE;
}

//BOOL CStockApp::LoadWindowPlacement(CRect & rectNormalPosition, int & nFflags, int & nShowCmd)
//{
//    // override this to disable save/load state to client regedit
//    return TRUE;
//}
//
//BOOL CStockApp::StoreWindowPlacement(const CRect & rectNormalPosition, int nFflags, int nShowCmd)
//{
//    // override this to disable save/load state to client regedit
//    return TRUE;
//}
//
//BOOL CStockApp::ReloadWindowPlacement(CFrameWnd * pFrame)
//{
//    // override this to disable save/load state to client regedit
//    return TRUE;
//}

void CStockApp::OnAppAbout()
{
}



