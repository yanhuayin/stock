#ifndef __STOCK_CONFIG_H__
#define __STOCK_CONFIG_H__

#if _MSC_VER > 1000
#pragma once
#endif

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

typedef rapidjson::UTF8<TCHAR>                      RapidUTF8;
typedef rapidjson::GenericDocument<RapidUTF8>       RapidDocument;
typedef rapidjson::GenericValue<RapidUTF8>          RapidValue;
typedef rapidjson::GenericStringBuffer<RapidUTF8>   RapidStringBuffer;
typedef rapidjson::Writer<RapidStringBuffer, RapidUTF8, RapidUTF8>  RapidWriter;

#endif
