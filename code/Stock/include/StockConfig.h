#ifndef __STOCK_CONFIG_H__
#define __STOCK_CONFIG_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <memory>

#include "rapidjson\document.h"
#include "rapidjson\stringbuffer.h"
#include "rapidjson\writer.h"

class CTradeView;
class CTradeWnd;
class CStockMainFrame;
class CTradeControl;
class CTradeModel;
class CTradeSettings;

typedef std::shared_ptr<CTradeView>     TradeViewPtr;
typedef std::shared_ptr<CTradeModel>    TradeModelPtr;

typedef TradeViewPtr                    TradeViewHandle;
typedef TradeModelPtr                   TradeModelHandle;

//typedef rapidjson::UTF8<TCHAR>                      RapidUTF8;
typedef rapidjson::UTF16<>                          RapidUTF16;
typedef rapidjson::GenericDocument<RapidUTF16>      RapidDocument;
typedef rapidjson::GenericValue<RapidUTF16>         RapidValue;
typedef rapidjson::GenericStringBuffer<RapidUTF16>  RapidStringBuffer;
typedef rapidjson::Writer<RapidStringBuffer, RapidUTF16, RapidUTF16>  RapidWriter;

enum LocateType
{
    LT_Buy = 0,
    LT_BuyCode,
    LT_BuyPrice,
    LT_BuyQuant,
    LT_BuyOrder,

    LT_Sell,
    LT_SellCode,
    LT_SellPrice,
    LT_SellQuant,
    LT_SellOrder,

    LT_Cancel,
    LT_CancelList,

    LT_Delegate,
    LT_DelegateList,

    LT_Num
};

#endif
