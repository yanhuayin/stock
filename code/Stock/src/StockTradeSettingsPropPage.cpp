#include "stdafx.h"
#include "resource.h"
#include "StockTradeSettingsPropPage.h"

#define ST_SET_VALID_STR    _T("1234567890")

IMPLEMENT_DYNCREATE(CStockTradeSettingsPropPage, CBCGPPropertyPage)

BEGIN_MESSAGE_MAP(CStockTradeSettingsPropPage, CBCGPPropertyPage)
END_MESSAGE_MAP()

CStockTradeSettingsPropPage::CStockTradeSettingsPropPage()
    : CBCGPPropertyPage(CStockTradeSettingsPropPage::IDD)
{
}

CStockTradeSettingsPropPage::~CStockTradeSettingsPropPage()
{
}

void CStockTradeSettingsPropPage::DoDataExchange(CDataExchange * pDx)
{
    CBCGPPropertyPage::DoDataExchange(pDx);

    DDX_Control(pDx, IDC_SETTINGS_QUOTA_EDIT, _quota);
    DDX_Control(pDx, IDC_SETTINGS_FILE_EDIT, _setFile);
}

BOOL CStockTradeSettingsPropPage::OnInitDialog()
{
    CBCGPPropertyPage::OnInitDialog();

    _quota.DisableMask();
    _quota.SetValidChars(ST_SET_VALID_STR);

    return TRUE;
}
