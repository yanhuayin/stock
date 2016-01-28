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

class CStockUpdateThread;

class CTradeControl : public Singleton<CTradeControl>
{
public:
    CTradeControl() : m_mainWnd(nullptr), m_init(false), m_worker(nullptr) {}
   ~CTradeControl() {}

public:
    bool    Init(CStockMainFrame *pMainWnd);
    bool    Close();
    bool    IsInit() const { return m_init; }
    bool    IsClose() const { return !m_init; }

public:
    void    ViewClosed(TradeViewHandle h);

    bool    RequestInfo(TradeViewHandle h, CString const& code);
    
    void    RefreshViewsInfo(TradeModelHandle h) const; // RT_Subscribe

    void    RefreshViewsInfo(UINT m); // RT_Poll
    
    void    Update(); // [P]

    int     Trade(TradeViewHandle h, StockInfoType info, StockTradeOp op);

    UINT    Quota() const;

    UINT    Left() const;

    void    RefreshViewsQuota(UINT quota) const;

    void    RefreshViewsLeft(UINT left) const;

    UINT    IsTradeAvailable(TradeViewHandle v, CString const& quant, bool showErr = false) const;

    StockOrderResult    CancelOrder(TradeViewHandle h, int order);

private:
    typedef std::list<TradeViewHandle>  ViewList;
    typedef std::shared_ptr<ViewList>   ViewListPtr;
    typedef std::list<int>              OrderList;
    typedef std::shared_ptr<OrderList>  OrderListPtr;
    typedef std::unordered_map<UINT, ViewListPtr>                   ModelViewMap;
    typedef std::unordered_map<TradeViewHandle, UINT>               ViewModelMap;
    typedef std::unordered_map<TradeViewHandle, OrderListPtr>       ViewOrderMap;
    typedef std::unordered_map<int, TradeViewHandle>                OrderViewMap;

private:
    bool    Watch(TradeViewHandle v, UINT m);
    void    RefreshViewsInfo(TradeModelHandle h, ViewListPtr vls) const;
    void    RefreshViewInfo(UINT m, TradeViewHandle v) const;
    void    RefreshViewLeft(TradeViewHandle h, CString const& left) const;

    CMutex              m_mvMutex;

    ModelViewMap        m_modelView;
    ViewModelMap        m_viewModel;
    OrderViewMap        m_orderView;
    ViewOrderMap        m_viewOrder;
    CStockMainFrame    *m_mainWnd;
    bool                m_init;

    CStockUpdateThread *m_worker;
};



#endif