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
    DECLARE_DYNCREATE(CTradeView);

public:
    CTradeView();
    virtual ~CTradeView();

public:
    typedef std::array<CString const*, SOF_Num>   OrderStrArray;

    void    SetName(CString const& name);
    void    SetInfo(StockInfoField field, InfoNumArrayPtr info);
    void    SetOrder(int order, OrderStrArray const& strs);
    void    SetQuota(CString const& quota);
    void    SetLeft(CString const& left);
    void    FlushInfo() { m_tradeWnd._info.UpdateData(FALSE); }
    void    FlushOrder() { m_tradeWnd._order.UpdateData(FALSE); }
    void    FlushQuota() { m_tradeWnd._quota.UpdateData(FALSE); }
    void    Flush() { m_tradeWnd.UpdateData(FALSE); }
    //void    GetCode(CString &outCode) const;
    void    GetQuant(CString &outQuant) const;
    //void    GetPrice(StockInfoType info, CString &outPrice) const;

public:
    void    OnOK();
    void    OnTrade(StockInfoType info, StockTradeOp op);
    void    OnCancelOrder(CBCGPGridRow *pRow);

protected:
    virtual void    PostNcDestroy();

protected:
    CTradeWnd       m_tradeWnd;

protected:
    afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void    OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
};

class CStockMDIChild : public CBCGPMDIChildWnd
{
    DECLARE_DYNCREATE(CStockMDIChild)

public:
    CStockMDIChild() {}

protected:
    afx_msg void OnDestroy();
    DECLARE_MESSAGE_MAP()
};


#endif
