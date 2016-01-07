#ifndef __STOCK_TRADE_SETTINGS_PROP_PAGE_H__
#define __STOCK_TRADE_SETTINGS_PROP_PAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif

class CStockTradeSettingsPropPage : public CBCGPPropertyPage
{
    DECLARE_DYNCREATE(CStockTradeSettingsPropPage)

public:
    CStockTradeSettingsPropPage();
   ~CStockTradeSettingsPropPage();

public:
    enum { IDD = IDD_SETTINGS_PAGE };

    CBCGPMaskEdit           _quota;
    CBCGPEdit               _setFile;

protected:
    virtual void    DoDataExchange(CDataExchange *pDx);
    virtual BOOL    OnInitDialog();
    virtual void    OnOK();

protected:
    afx_msg void    OnFileChange();
    afx_msg void    OnQuotaChange();
    DECLARE_MESSAGE_MAP()

private:
    bool            m_dirty;
};



#endif // !__STOCK_TRADE_SETTINGS_PROP_PAGE_H__

