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
    CBCGPMaskEdit       m_code;
    CBCGPEdit           m_name;
    CBCGPEdit           m_quota;
    CBCGPEdit           m_left;
    CBCGPMaskEdit       m_quantity;
    CBCGPSpinButtonCtrl m_qSpin;
    CBCGPMaskEdit       m_q1;
    CBCGPMaskEdit       m_q2;
    CBCGPButton         m_q1Minus;
    CBCGPButton         m_q1Plus;
    CBCGPButton         m_q2Minus;
    CBCGPButton         m_q2Plus;
    CBCGPButton         m_qDouble;
    CBCGPButton         m_qHalf;
    CBCGPGridCtrl       m_info;
    CBCGPGridCtrl       m_order;

public:
    virtual void    DoDataExchange(CDataExchange *pDx);

protected:
    virtual BOOL    OnInitDialog();
    virtual BOOL    OnCommand(WPARAM wParam, LPARAM lParam);

protected:
    DECLARE_MESSAGE_MAP()
};




#endif
