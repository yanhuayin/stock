#ifndef __TRADE_MODEL_H__
#define __TRADE_MODEL_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <array>
#include <unordered_map>
#include "wind\WAPIWrapperCpp.h"
#include "StockConfig.h"
#include "Utils.h"

class CTradeModel : public std::enable_shared_from_this<CTradeModel>
{
public:
    CTradeModel() : m_reqId(0) {}
   ~CTradeModel() {}

public:
    typedef std::array<double, SIT_Num>     InfoNumArray;
    typedef std::shared_ptr<InfoNumArray>   InfoNumArrayPtr;

public:
    //ULONGLONG       ReqId() { return m_reqId; }
    //String const&   WindCode() { return m_windCode; }
    String const&   Code() { return m_windCode; }
    String const&   Name() { return m_name; }
    InfoNumArrayPtr NumInfo(StockInfoField field);

private:
    friend class CTradeModelManager;

    String          m_code;
    String          m_name;
    String          m_windCode;

    InfoNumArrayPtr     m_price;
    InfoNumArrayPtr     m_quant;

    ULONGLONG           m_reqId;
};


class CTradeModelManager : public Singleton<CTradeModelManager>
{
public:
    CTradeModelManager() : m_init(false) {}
   ~CTradeModelManager() {}

public:
    enum RequestType
    {
        RT_MultiThread = 0,
        RT_Immediate,

        RT_Num
    };

    bool    Init(RequestType type = RT_Immediate);
    bool    Shutdown();
    bool    IsInit() { return m_init; }

public:
    void    FreeModel(TradeModelHandle h);
    bool    RequestModel(TradeModelHandle h, bool force = false);

    // only used by multi thread request
    void    UpdateModel(ULONGLONG reqId, WindData const& wd);

    typedef std::vector<String> CandidatesList;
    TradeModelHandle FindModel(String const& code, CandidatesList *c);

    RequestType Type() const { return m_type; }

private:
    void    UpdateModel(TradeModelHandle h, WindData const& wd);

private:
    typedef std::unordered_map<String, TradeModelHandle>    ModelMap;
    ModelMap        m_models;

    // for multithread use
    typedef std::unordered_map<ULONGLONG, TradeModelHandle> ReqMap;
    ReqMap          m_reqs;

    String          m_00;
    String          m_30;
    String          m_60;

    String          m_date;
    bool            m_init;

    RequestType     m_type;
};


#endif
