#ifndef __STOCK_LOCATE_PROP_PAGE_H__
#define __STOCK_LOCATE_PROP_PAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "StockConfig.h"
#include "TargetLocationPic.h"
#include "ApiHelper.h"

struct CLocateControl
{
    CBCGPStatic    *label;
    CBCGPMaskEdit  *p;
    CBCGPEdit      *h;
    LocateInfo      i;
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

    ListViewColumn          _cancleList[SOF_Num];
    ListViewColumn          _delegateList[SOF_Num];

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
    ListViewColumn* GetListViewColumns(LocateType type) 
    { 
        ASSERT(type == LT_CancelList || type == LT_DelegateList);
        if (type == LT_CancelList) 
            return &(_cancleList[0]); 
        return &(_delegateList[0]);
    }
    void            Withdraw(bool tSet, bool pSet)
    {
        if (tSet)
            m_target.Empty();
        if (pSet)
        {
            m_tID = 0;
            m_process = nullptr;
        }
    }

private:
    //CBCGPMaskEdit           m_buyOrSell;
    CBCGPMaskEdit           m_bosCode;
    CBCGPMaskEdit           m_bosPrice;
    CBCGPMaskEdit           m_bosQuantity;
    CBCGPMaskEdit           m_bosOrder;

    //CBCGPStatic             m_buyOrSellLab;
    CBCGPStatic             m_bosCodeLab;
    CBCGPStatic             m_bosPriceLab;
    CBCGPStatic             m_bosQuantityLab;
    CBCGPStatic             m_bosOrderLab;

    //CBCGPEdit               m_hbuyOrSell;
    CBCGPEdit               m_hbosCode;
    CBCGPEdit               m_hbosPrice;
    CBCGPEdit               m_hbosQuant;
    CBCGPEdit               m_hbosOrder;

    //CBCGPMaskEdit           m_cancel;
    CBCGPMaskEdit           m_cancelBtn;
    CBCGPMaskEdit           m_cancelList;

    //CBCGPMaskEdit           m_delegate;
    CBCGPMaskEdit           m_delegateList;

    //CBCGPEdit               m_hcancel;
    CBCGPEdit               m_hcancelBtn;
    CBCGPEdit               m_hcancelList;

    //CBCGPEdit               m_hdelegate;
    CBCGPEdit               m_hdelegateList;

    CBCGPMaskEdit           m_tree;
    CBCGPEdit               m_htree;

    CBCGPMaskEdit           m_app;
    CBCGPEdit               m_happ;

    CTargetLocationPic      m_pic;
    CMenu                   m_menu;
    UINT                    m_bosId;

    CString                 m_target;
    DWORD                   m_tID;
    HandlePtr               m_process;

    bool                    m_dirty;
};



#endif // !__STOCK_LOCATE_PROP_PAGE_H__
