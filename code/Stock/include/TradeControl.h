#ifndef __TRADE_CONTROL_H__
#define __TRADE_CONTROL_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "StockConfig.h"
#include "Utils.h"


class CTradeControl : public Singleton<CTradeControl>
{
public:
    CTradeControl();
   ~CTradeControl();

public:

};



#endif