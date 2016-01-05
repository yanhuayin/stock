#ifndef __STOCK_LOCATE_PROP_PAGE_H__
#define __STOCK_LOCATE_PROP_PAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "TargetLocationPic.h"

class CStockLocatePropPage : public CBCGPPropertyPage
{
    DECLARE_DYNCREATE(CStockLocatePropPage)

public:
    CStockLocatePropPage();
   ~CStockLocatePropPage();

public:


public:
    enum { IDD = IDD_LOCATE_PAGE };

    CBCGPMenuButton         _tradeSelBtn;

    CBCGPMaskEdit           _buyOrSell;
    CBCGPMaskEdit           _bosCode;
    CBCGPMaskEdit           _bosPrice;
    CBCGPMaskEdit           _bosQuantity;
    CBCGPMaskEdit           _bosOrder;

    CBCGPStatic             _buyOrSellLab;
    CBCGPStatic             _bosCodeLab;
    CBCGPStatic             _bosPriceLab;
    CBCGPStatic             _bosQuantityLab;
    CBCGPStatic             _bosOrderLab;

    CBCGPMaskEdit           _cancel;
    CBCGPMaskEdit           _cancelList;
    CBCGPMaskEdit           _delegate;
    CBCGPMaskEdit           _delegateList;

    CBCGPEdit               _locateFile;

protected:
    virtual void    DoDataExchange(CDataExchange *pDX);
    virtual BOOL    OnInitDialog();
    virtual BOOL    OnSetActive();

protected:
    afx_msg void    OnTimer(UINT nIDEvent);
    afx_msg void    OnBOSChange();
    afx_msg LRESULT OnTargetWnd(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

private:
    CTargetLocationPic      m_pic;
    CMenu                   m_menu;
    UINT                    m_bosId;
};



#endif // !__STOCK_LOCATE_PROP_PAGE_H__
