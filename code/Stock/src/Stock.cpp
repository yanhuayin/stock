#include "stdafx.h"
#include "resource.h"
#include "StockConfig.h"
#include "StockGlobal.h"
#include "Utils.h"
#include "Stock.h"
#include "StockMainFrm.h"
#include "TradeModel.h"
#include "TradeControl.h"

#define ST_LOAD_APP_ERR     1
#define ST_LOAD_LOC_ERR     2
#define ST_LOAD_SET_ERR     3
#define ST_INIT_MODEL_ERR   4
#define ST_LOAD_UI_ERR      5
#define ST_LOAD_NO_ERR      0

class CStockLoadThread : public CWinThread
{
    DECLARE_DYNCREATE(CStockLoadThread)

public:
    CStockLoadThread(): m_wnd(nullptr) {}

public:
    virtual BOOL    InitInstance();
    //virtual int     ExitInstance();

public:
    void    UIWnd(CWnd *pWnd) { m_wnd = pWnd; }

protected:
    afx_msg void    OnLoad(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP();

private:
    CWnd   *m_wnd;
};

IMPLEMENT_DYNCREATE(CStockLoadThread, CWinThread)

BEGIN_MESSAGE_MAP(CStockLoadThread, CWinThread)
    ON_THREAD_MESSAGE(ST_LOAD_MSG, OnLoad)
END_MESSAGE_MAP()

BOOL CStockLoadThread::InitInstance()
{
    m_bAutoDelete = FALSE;
    return TRUE;
}

//int CStockLoadThread::ExitInstance()
//{
//    CWinThread::ExitInstance();
//}

void CStockLoadThread::OnLoad(WPARAM wParam, LPARAM lParam)
{
    CStockAppData &data = theApp.AppData();

    if (!m_wnd)
    {
        ::AfxEndThread(ST_LOAD_UI_ERR, FALSE);
        return;
    }

    UINT res = ST_LOAD_NO_ERR;

    m_wnd->PostMessage(ST_LOAD_MSG, (WPARAM)(ST_LOAD_ST_CONFIG));
    if (data.Load())
    {
        if (data.LocateData().Load(data.LocFile()) && data.LocateData().LocateWnd())
        {
            if (!data.TradeSettingsData().Load(data.SetFile()))
            {
                res = ST_LOAD_SET_ERR;
            }
        }
        else
        {
            res = ST_LOAD_LOC_ERR;
        }
    }
    else
    {
        res = ST_LOAD_APP_ERR;
    }

    if (res == ST_LOAD_NO_ERR)
    {
        m_wnd->PostMessage(ST_LOAD_MSG, (WPARAM)(ST_INIT_DATABASE));
        if (CTradeModelManager::Instance().Init())
        {
            res = ST_LOAD_NO_ERR;
        }
        else
        {
            res = ST_INIT_MODEL_ERR;
        }
    }

    m_wnd->PostMessage(ST_LOAD_MSG, (WPARAM)(ST_LOAD_ST_FINISH));

    ::AfxEndThread(res, FALSE);
}

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

    CStockLoadThread *pThread = (CStockLoadThread*)AfxBeginThread(RUNTIME_CLASS(CStockLoadThread), 0, 0, CREATE_SUSPENDED);
    pThread->UIWnd(&m_splash);
    pThread->ResumeThread();

    m_splash.Worker(pThread);
    if (m_splash.DoModal() != IDOK)
        return FALSE;

    DWORD err = ST_LOAD_APP_ERR;
    ::GetExitCodeThread(pThread->m_hThread, &err);

    ST_SAFE_DELETE(pThread);

    if (err != ST_LOAD_NO_ERR)
        return FALSE;

    // Parse command line for standard shell commands, DDE, file open
    //CCommandLineInfo cmdInfo;
    //this->ParseCommandLine(cmdInfo);

    // TODO : parse command line

    // Dispatch commands specified on the command line.  Will return FALSE if
    // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
    //if (!this->ProcessShellCommand(cmdInfo))
    //    return FALSE;

    CStockMainFrame *pMainFrame = new CStockMainFrame();

    this->EnableLoadWindowPlacement(FALSE);

    if (!pMainFrame->LoadFrame(IDR_MAIN_MENU))
        return FALSE;

    m_pMainWnd = pMainFrame;

    if (!CTradeControl::Instance().Instance().Init(pMainFrame))
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
