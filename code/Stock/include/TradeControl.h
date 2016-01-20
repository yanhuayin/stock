#ifndef __TRADE_CONTROL_H__
#define __TRADE_CONTROL_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <list>
#include <vector>
#include <unordered_map>
#include "StockConfig.h"
#include "Utils.h"


class CTradeControl : public Singleton<CTradeControl>
{
public:
    CTradeControl() : m_mainWnd(nullptr), m_init(false) {}
   ~CTradeControl() {}

public:
    bool    Init(CStockMainFrame *pMainWnd);
    bool    Close();
    bool    IsInit() const { return m_init; }
    bool    IsClose() const { return !m_init; }
    bool    NeedTimer() const;

public:
    void    ViewClosed(TradeViewHandle h);

    bool    RequestInfo(TradeViewHandle h, CString const& code);
    
    void    RefreshViewsInfo(TradeModelHandle h); // used by multi thread request only

    void    Update(); // used by immediate request type only

    int     Trade(TradeViewHandle h, StockInfoType info, StockTradeOp op);

    UINT    Quota() const;

    void    RefreshViewQuota(UINT quota) const;

    void    RefreshViewsTrade();

    StockOrderResult    CancelOrder(TradeViewHandle h, int order);

private:
    typedef std::list<TradeViewHandle>  ViewList;
    typedef std::shared_ptr<ViewList>   ViewListPtr;
    typedef std::list<int>              OrderList;
    typedef std::shared_ptr<OrderList>  OrderListPtr;
    typedef std::unordered_map<TradeModelHandle, ViewListPtr>       ModelViewMap;
    typedef std::unordered_map<TradeViewHandle, TradeModelHandle>   ViewModelMap;
    typedef std::unordered_map<TradeViewHandle, OrderListPtr>       ViewOrderMap;
    typedef std::unordered_map<int, TradeViewHandle>                OrderViewMap;

private:
    bool    Watch(TradeViewHandle v, TradeModelHandle m);
    void    RefreshViewsInfo(TradeModelHandle h, ViewListPtr vls);
    void    RefreshViewInfo(TradeModelHandle m, TradeViewHandle v);

    ModelViewMap        m_modelView;
    ViewModelMap        m_viewModel;
    OrderViewMap        m_orderView;
    ViewOrderMap        m_viewOrder;
    CStockMainFrame    *m_mainWnd;
    bool                m_init;
};



#endif