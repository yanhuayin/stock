#include "stdafx.h"
#include "resource.h"
#include "StockPropSheet.h"

IMPLEMENT_DYNAMIC(CStockPropSheet, CBCGPPropertySheet)

BEGIN_MESSAGE_MAP(CStockPropSheet, CBCGPPropertySheet)
END_MESSAGE_MAP()

CStockPropSheet::CStockPropSheet(CWnd * pParent)
    : CBCGPPropertySheet(IDS_PROP_CAPTION, pParent)
{
    this->SetLook(CBCGPPropertySheet::PropSheetLook_OutlookBar);
    this->SetIconsList(IDB_PROP_SHEET, 32);

    this->AddPage(&_locatePage);
    this->AddPage(&_settingsPage);

    this->EnableVisualManagerStyle(TRUE, TRUE);
    this->EnableDragClientArea();
    this->EnablePageTransitionEffect(CBCGPPageTransitionManager::BCGPPageTransitionSimpleSlide);
}

CStockPropSheet::~CStockPropSheet()
{
}

BOOL CStockPropSheet::OnInitDialog()
{
    BOOL bRes = CBCGPPropertySheet::OnInitDialog();

    // TODO

    return bRes;
}
