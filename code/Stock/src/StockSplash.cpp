#include "stdafx.h"
#include "resource.h"
#include "StockGlobal.h"
#include "StockSplash.h"

#define ST_SPLASH_TIMER     1

BEGIN_MESSAGE_MAP(CStockSplash, CDialog)
    ON_MESSAGE(ST_LOAD_MSG, OnLoading)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

CStockSplash::CStockSplash()
    : CDialog(CStockSplash::IDD)
    , m_worker(nullptr)
{
}

CStockSplash::~CStockSplash()
{
}

void CStockSplash::DoDataExchange(CDataExchange * pDX)
{
    DDX_Text(pDX, IDC_LOG_STATUS, m_status);
}

BOOL CStockSplash::OnInitDialog()
{
    if (!CDialog::OnInitDialog())
        return FALSE;

    if (!m_worker)
        return FALSE;

    m_worker->PostThreadMessage(ST_LOAD_MSG, 0, 0);

    return TRUE;
}

LRESULT CStockSplash::OnLoading(WPARAM wParam, LPARAM lParam)
{
    UINT code = (UINT)wParam;

    switch (code)
    {
    case ST_LOAD_ST_CONFIG:
        m_status = _T("loading...");
        break;
    case ST_INIT_DATABASE:
        m_status = _T("initializing...");
        break;
    case ST_LOAD_ST_FINISH:
        this->EndDialog(IDOK);
        return 0;
    default:
        return 0;
    }

    this->UpdateData(FALSE);

    return 0;
}

HBRUSH CStockSplash::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT ctlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, ctlColor);

    if (pWnd->GetDlgCtrlID() == IDC_LOG_STATUS)
    {
        pDC->SetBkMode(TRANSPARENT);
        return HBRUSH(::GetStockObject(HOLLOW_BRUSH));
    }

    return hbr;
}


