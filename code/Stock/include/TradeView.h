#ifndef __TRADE_VIEW_H__
#define __TRADE_VIEW_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "TradeWnd.h"

class CTradeView : public CBCGPDockingControlBar, public std::enable_shared_from_this<CTradeView>
{
public:
    CTradeView();
    virtual ~CTradeView();

public:
    void AdjustLayout();

public:
    void OnQ1PlusClicked();
    void OnQ1MinusClicked();

protected:
    CTradeWnd       m_tradeWnd;

protected:
    afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void    OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
};




#endif
