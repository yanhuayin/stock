#ifndef __STOCK_LOCATE_PROP_PAGE_H__
#define __STOCK_LOCATE_PROP_PAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "StockConfig.h"
#include "TargetLocationPic.h"

struct CLocateControl
{
    CBCGPStatic    *label;
    CBCGPMaskEdit  *p;
    CBCGPEdit      *h;
    POINT           pos;
    HWND            hwnd;
    HTREEITEM       hitem;
};

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

    CLocateControl          _ctrls[LT_Num];

    CBCGPEdit               _locateFile;

protected:
    virtual void    DoDataExchange(CDataExchange *pDX);
    virtual BOOL    OnInitDialog();
    virtual void    OnOK();

protected:
    afx_msg void    OnTimer(UINT nIDEvent);
    afx_msg void    OnBOSChange();
    afx_msg void    OnFileChange();
    afx_msg void    OnClear();
    afx_msg LRESULT OnTargetWnd(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

private:
    void            InitCtrls();
    CBCGPMaskEdit*  FindMaskCtrl(LocateType type);
    CBCGPStatic*    FindLabelCtrl(LocateType type);
    CBCGPEdit*      FindEditCtrl(LocateType type);
    void            SwitchBOS();
    void            SetCtrlText(LocateType type);
    void            SetCtrlFocus();
    LocateType      GetCtrlFocus();

private:
    CBCGPMaskEdit           m_buyOrSell;
    CBCGPMaskEdit           m_bosCode;
    CBCGPMaskEdit           m_bosPrice;
    CBCGPMaskEdit           m_bosQuantity;
    CBCGPMaskEdit           m_bosOrder;

    CBCGPStatic             m_buyOrSellLab;
    CBCGPStatic             m_bosCodeLab;
    CBCGPStatic             m_bosPriceLab;
    CBCGPStatic             m_bosQuantityLab;
    CBCGPStatic             m_bosOrderLab;

    CBCGPEdit               m_hbuyOrSell;
    CBCGPEdit               m_hbosCode;
    CBCGPEdit               m_hbosPrice;
    CBCGPEdit               m_hbosQuant;
    CBCGPEdit               m_hbosOrder;

    CBCGPMaskEdit           m_cancel;
    CBCGPMaskEdit           m_cancelList;
    CBCGPMaskEdit           m_delegate;
    CBCGPMaskEdit           m_delegateList;

    CBCGPEdit               m_hcancel;
    CBCGPEdit               m_hcancelList;
    CBCGPEdit               m_hdelegate;
    CBCGPEdit               m_hdelegateList;

    CTargetLocationPic      m_pic;
    CMenu                   m_menu;
    UINT                    m_bosId;

    CString                 m_target;
    DWORD                   m_tID;

    bool                    m_dirty;
};



#endif // !__STOCK_LOCATE_PROP_PAGE_H__
