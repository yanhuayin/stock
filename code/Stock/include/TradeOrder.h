#ifndef __TRADE_ORDER_H__
#define __TRADE_ORDER_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <unordered_map>
#include "StockConfig.h"
#include "ApiHelper.h"
#include "Utils.h"
#include "TradeTime.h"

struct TradeOrder
{
    TradeTimePoint  local;
    TradeTimePoint  time;
    CString         code;
    CString         flag;
    CString         price;
    CString         quant;
    CString         id;
    CString         turnover;
    bool            deal;
};


class CTradeOrderManager : public Singleton<CTradeOrderManager>
{
public:
    CTradeOrderManager() : m_id(0), m_total(0) {} // TODO : load total from config file
   ~CTradeOrderManager() {}

public:
    int     Trade(StockTradeOp op, CString const& code, CString const& price, CString const& quant);
    StockOrderResult    CancelOrder(int order);
    TradeOrder const&   Order(int order) const;
    UINT                TotalTrade() const { return m_total; }

private:
    bool    SetText(HWND hwnd, CString const& text) const; // TODO : move all hwnd manipulate out of this class


private:
    typedef std::unordered_map<int, TradeOrder>    OrderMap;
    OrderMap        m_orders;
    int             m_id;
    UINT            m_total;
};




#endif // !__TRADE_ORDER_H__
