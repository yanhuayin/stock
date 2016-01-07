#ifndef __STOCK_PROP_SHEET_H__
#define __STOCK_PROP_SHEET_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "StockLocatePropPage.h"
#include "StockTradeSettingsPropPage.h"

class CStockPropSheet : public CBCGPPropertySheet
{
    DECLARE_DYNAMIC(CStockPropSheet)

public:
    CStockPropSheet(CWnd *pParent = NULL);
   ~CStockPropSheet();

public:
    CStockLocatePropPage        _locatePage;
    CStockTradeSettingsPropPage _settingsPage;

public:
    virtual BOOL    OnInitDialog();

protected:
    DECLARE_MESSAGE_MAP()
};

#endif // !__STOCK_PROP_SHEET_H__
