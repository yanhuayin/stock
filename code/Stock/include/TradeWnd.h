#ifndef __TRADE_WND_H__
#define __TRADE_WND_H__


#if _MSC_VER > 1000
#pragma once
#endif

#include "resource.h"

class CTradeWnd : public CBCGPDialog
{
public:
    CTradeWnd(CWnd *parent = NULL);
    virtual ~CTradeWnd();

public:
    enum { IDD = IDD_TRADEFORM };

public:
    CBCGPMaskEdit       _code;
    CBCGPEdit           _name;
    CBCGPEdit           _quota;
    CBCGPEdit           _left;
    CBCGPMaskEdit       _quantity;
    CBCGPSpinButtonCtrl _qSpin;
    CBCGPMaskEdit       _q1;
    CBCGPMaskEdit       _q2;
    CBCGPButton         _q1Minus;
    CBCGPButton         _q1Plus;
    CBCGPButton         _q2Minus;
    CBCGPButton         _q2Plus;
    CBCGPButton         _qDouble;
    CBCGPButton         _qHalf;
    CBCGPGridCtrl       _info;
    CBCGPGridCtrl       _order;

public:
    virtual void    DoDataExchange(CDataExchange *pDx);

public:
    typedef std::function<void()>  ButtonClickEvent;
    
    ButtonClickEvent    _q1PlusClickEvent;
    ButtonClickEvent    _q1MinusClickEvent;

protected:
    virtual BOOL    OnInitDialog();

protected:
    afx_msg void    OnQ1PlusClicked();
    afx_msg void    OnQ1MinusClicked();
    afx_msg void    OnDeltPosSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void    OnEnter();
    afx_msg LRESULT HandleInitDialog(WPARAM wPram, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

private:
    CStatic         m_infoRect;
    CStatic         m_orderRect;
};




#endif
