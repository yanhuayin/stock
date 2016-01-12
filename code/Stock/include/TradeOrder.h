#ifndef __TRADE_ORDER_H__
#define __TRADE_ORDER_H__

#if _MSC_VER > 1000
#pragma once
#endif


#include "StockConfig.h"
#include "Utils.h"

struct CTradeOrder
{

};


class CTradeOrderManager : public Singleton<CTradeOrderManager>
{
public:
    CTradeOrderManager() {}
   ~CTradeOrderManager() {}

public:


};




#endif // !__TRADE_ORDER_H__
