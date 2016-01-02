#ifndef __TRADE_VIEW_H__
#define __TRADE_VIEW_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "TradeWnd.h"

class CTradeView : public CBCGPDockingControlBar
{
public:
    CTradeView();
    virtual ~CTradeView();

public:

protected:
    CTradeWnd       m_tradeWnd;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()
};




#endif
