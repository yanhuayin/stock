#ifndef __TRADE_VIEW_H__
#define __TRADE_VIEW_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <memory>
#include <array>
#include "StockConfig.h"
#include "TradeWnd.h"

class CTradeView : public CBCGPDockingControlBar, public std::enable_shared_from_this<CTradeView>
{
public:
    CTradeView();
    virtual ~CTradeView();

public:
    void AdjustLayout();

public:
    typedef std::array<double, SIT_Num>     InfoNumArray;
    typedef std::shared_ptr<InfoNumArray>   InfoNumArrayPtr;

    void    SetName(CString const& name);
    void    SetInfo(StockInfoField field, InfoNumArrayPtr info);
    void    Flush() { m_tradeWnd.UpdateData(FALSE); }

public:
    void    OnOK();

protected:
    virtual void    PostNcDestroy();

protected:
    CTradeWnd       m_tradeWnd;

protected:
    afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void    OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
};




#endif
