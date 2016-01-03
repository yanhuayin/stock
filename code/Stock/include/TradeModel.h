#ifndef __TRADE_MODEL_H__
#define __TRADE_MODEL_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "StockConfig.h"
#include "Utils.h"

class CTradeModel
{
public:
    CTradeModel();
   ~CTradeModel();

public:

private:

};


class CTradeModelManager : public Singleton<CTradeModelManager>
{
public:
    CTradeModelManager();
   ~CTradeModelManager();

public:

};


#endif
