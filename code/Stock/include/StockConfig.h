#ifndef __STOCK_CONFIG_H__
#define __STOCK_CONFIG_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <memory>
#include <string>
#include <mutex>
#include <atomic>

#include "rapidjson\document.h"
#include "rapidjson\stringbuffer.h"
#include "rapidjson\writer.h"
#include "rapidjson\encodedstream.h"

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> >   String;

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

typedef rapidjson::UTF8<>                           RapidUTF8;
typedef rapidjson::UTF16<>                          RapidUTF16;
typedef rapidjson::GenericDocument<RapidUTF16>      RapidDocument;
typedef rapidjson::GenericValue<RapidUTF16>         RapidValue;
typedef rapidjson::StringBuffer                     RapidStringBuffer; // for output buffer
typedef rapidjson::StringStream                     RapidStringStream; // for input stream
typedef rapidjson::EncodedInputStream<RapidUTF8, RapidStringStream>         RapidEncodeInputStream;
typedef rapidjson::EncodedOutputStream<RapidUTF8, RapidStringBuffer>        RapidEncodeOutputStream;
typedef rapidjson::Writer<RapidEncodeOutputStream, RapidUTF16, RapidUTF8>   RapidWriter;

typedef std::mutex                  Mutex;
typedef std::unique_lock<Mutex>     Lock;

typedef std::atomic_ullong          IdType;

enum LocateType
{
    LT_App = 0,

    LT_Buy,
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
    LT_CancelBtn,
    LT_CancelList,

    LT_Query,
    LT_Delegate,
    LT_DelegateList,

    LT_Num
};

enum StockInfoType
{
    SIT_Sell10 = 0,
    SIT_Sell9,
    SIT_Sell8,
    SIT_Sell7,
    SIT_Sell6,
    SIT_Sell5,
    SIT_Sell4,
    SIT_Sell3,
    SIT_Sell2,
    SIT_Sell1,

    SIT_Buy1,
    SIT_Buy2,
    SIT_Buy3,
    SIT_Buy4,
    SIT_Buy5,
    SIT_Buy6,
    SIT_Buy7,
    SIT_Buy8,
    SIT_Buy9,
    SIT_Buy10,

    SIT_Num
};

enum StockInfoField
{
    SIF_Price = 0,
    SIF_Quant,

    SIF_Num
};

enum StockOrderField
{
    SOF_Time = 0,
    SOF_Code,
    SOF_Name,
    SOF_Flag,
    SOF_Price,
    SOF_Quant,
    SOF_Id,
    SOF_Turnover,

    SOF_Num
};

enum StockTradeOp
{
    STO_Buy = 0,
    STO_Sell,

    STO_Num
};

enum StockOrderResult
{
    SOR_OK = 0,
    SOR_LeftOK,
    SOR_Dealed,
    SOR_Error
};

typedef std::array<double, SIT_Num>     InfoNumArray;
//typedef std::shared_ptr<InfoNumArray>   InfoNumArrayPtr;

//namespace std
//{
//    template<> struct hash<CString>
//    {
//        typedef CString argument_type;
//        typedef std::size_t result_type;
//
//        result_type operator()(argument_type const& s) const
//        {
//            CMapStringToOb hasher;
//            return hasher.HashKey(s);
//        }
//    };
//}

#endif
