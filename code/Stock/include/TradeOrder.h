#ifndef __TRADE_ORDER_H__
#define __TRADE_ORDER_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <unordered_map>
#include "StockConfig.h"
#include "ApiHelper.h"
#include "Utils.h"

struct CTradeOrder
{
    CString     code;
    CString     price;
    CString     quant;
    CString     date;
    CString     id; // reserve if we can get the delegate info
};


class CTradeOrderManager : public Singleton<CTradeOrderManager>
{
public:
    CTradeOrderManager() : m_id(0) {}
   ~CTradeOrderManager() {}

public:
    int     Trade(StockTradeOp op, CString const& code, CString const& price, CString const& quant);

    enum CancelOrderResult
    {
        COR_OK = 0,
        COR_NotFound,
        COR_Dealed
    };

    CancelOrderResult   CancelOrder(int order);

private:
    bool    SetText(HandlePtr process, HWND hwnd, CString const& text) const; // TODO : move all hwnd manipulate out of this class


private:
    typedef std::unordered_map<int, CTradeOrder>    OrderMap;
    OrderMap        m_orders;
    int             m_id;
};




#endif // !__TRADE_ORDER_H__
