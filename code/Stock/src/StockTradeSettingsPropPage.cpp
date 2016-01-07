#include "stdafx.h"
#include "resource.h"
#include "StockConfig.h"
#include "StockGlobal.h"
#include "Stock.h"
#include "StockTradeSettingsPropPage.h"

#define ST_SET_VALID_STR    _T("1234567890")

IMPLEMENT_DYNCREATE(CStockTradeSettingsPropPage, CBCGPPropertyPage)

BEGIN_MESSAGE_MAP(CStockTradeSettingsPropPage, CBCGPPropertyPage)
    ON_EN_CHANGE(IDC_SETTINGS_FILE_EDIT, OnFileChange)
    ON_EN_CHANGE(IDC_SETTINGS_QUOTA_EDIT, OnQuotaChange)
END_MESSAGE_MAP()

CStockTradeSettingsPropPage::CStockTradeSettingsPropPage()
    : CBCGPPropertyPage(CStockTradeSettingsPropPage::IDD)
    , m_dirty(false)
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

    _setFile.EnableFileBrowseButton(ST_CONFIG_FILE_EXT, ST_CONFIG_FILE_FILTER);

    CTradeSettingsData &data = theApp.AppData().TradeSettingsData();
    if (data.Quota())
    {
        CString quota;
        quota.Format(_T("%u"), data.Quota());
        _quota.SetWindowText(quota);
    }

    _setFile.SetWindowText(theApp.AppData().SetFile());

    this->UpdateData(FALSE);

    return TRUE;
}

void CStockTradeSettingsPropPage::OnOK()
{
    CPropertyPage::OnOK();

    if (m_dirty)
    {
        this->UpdateData(TRUE);

        CStockAppData &app = theApp.AppData();
        CTradeSettingsData &data = app.TradeSettingsData();

        CString quotaStr;
        _quota.GetWindowText(quotaStr);
        UINT quota = _ttoi(quotaStr);

        data.Quota(quota);

        CString setFile;
        _setFile.GetWindowText(setFile);
        app.SetFile(setFile);

        if (data.Save(setFile))
        {
            if (app.Save())
            {
                this->SetModified(FALSE);
                m_dirty = true;
            }
        }
    }
}

void CStockTradeSettingsPropPage::OnFileChange()
{
    this->SetModified(TRUE);
    m_dirty = true;
}

void CStockTradeSettingsPropPage::OnQuotaChange()
{
    this->SetModified(TRUE);
    m_dirty = true;
}
