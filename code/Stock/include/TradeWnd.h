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

protected:

    DECLARE_MESSAGE_MAP()
};




#endif
