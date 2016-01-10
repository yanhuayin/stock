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
    bool    IsInit() { return m_init; }
    bool    IsClose() { return !m_init; }

public:
    void    ViewClosed(TradeViewHandle h);

    bool    RequestInfo(TradeViewHandle h, CString const& code);
    
    void    RefreshView(TradeModelHandle h);

private:
    bool    Watch(TradeViewHandle v, TradeModelHandle m);

private:
    typedef std::list<TradeViewHandle>  ViewList;
    typedef std::shared_ptr<ViewList>   ViewListPtr;
    typedef std::unordered_map<TradeModelHandle, ViewListPtr>       ModelViewMap;
    typedef std::unordered_map<TradeViewHandle, TradeModelHandle>   ViewModelMap;

    ModelViewMap        m_modelView;
    ViewModelMap        m_viewModel;
    CStockMainFrame    *m_mainWnd;
    bool                m_init;
};



#endif