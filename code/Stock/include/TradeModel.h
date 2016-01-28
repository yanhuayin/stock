#ifndef __TRADE_MODEL_H__
#define __TRADE_MODEL_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <array>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include "wind\WAPIWrapperCpp.h"
#include "StockConfig.h"
#include "Utils.h"

#define ST_MODEL_INVALID_ID         0

typedef std::mutex                  CMutex;
typedef std::unique_lock<CMutex>    CLock;

typedef std::atomic_ullong          IdType;

struct ModelInfoData
{
    InfoNumArray    *price;
    InfoNumArray    *quant;
};

class CTradeModel : public std::enable_shared_from_this<CTradeModel>
{
public:
    CTradeModel() : m_reqId(0) {}
   ~CTradeModel() {}

public:
    String const&   Code() const { return m_windCodeNum; }
    String const&   Name() const { return m_name; }
    void    NumInfo(StockInfoField field, InfoNumArray &info);
    void    NumInfo(ModelInfoData &d);
    UINT    Id() const { return m_id; }

private:
    friend class CTradeModelManager;

    String          m_code;
    String          m_name;
    String          m_windCode;
    String          m_windCodeNum;
    String          m_windCodeSuffix;

    InfoNumArray    m_price;
    InfoNumArray    m_quant;

    IdType          m_reqId;

    UINT            m_id;

    CMutex          m_m;
};


class CTradeModelManager : public Singleton<CTradeModelManager>
{
public:
    CTradeModelManager() : m_init(false) {}
   ~CTradeModelManager() {}

public:
    enum RequestType
    {
        RT_Poll = 0,
        RT_Subscribe,

        RT_Num
    };

    bool    Init(RequestType type = RT_Poll);
    bool    Shutdown();
    bool    IsInit() { return m_init; }

public:
    void    FreeModel(UINT id);
    bool    RequestModel(UINT id, bool update = false); // [P/U]

    void    UpdateModel(ULONGLONG reqId, WindData const& wd); // [S]

    typedef std::vector<String> CandidatesList;
    UINT    FindModel(String const& code, CandidatesList *c);

    RequestType Type() const { return m_type; }

    TradeModelHandle    ModelHandle(UINT id) { if (id <= m_list.size() && id > 0) { return m_list[id - 1]; } return nullptr; } // [P/U]
    TradeModelHandle    ModelHandle(String const& code) { auto it = m_models.find(code); if (it != m_models.end()) { return it->second; } return nullptr; } // [P/U]

private:
    bool    UpdateModel(TradeModelHandle h, WindData const& wd); // [P/U]

private:
    typedef std::unordered_map<String, TradeModelHandle>    ModelMap;
    ModelMap        m_models;

    typedef std::vector<TradeModelHandle> ModelList;
    ModelList       m_list;

    // for subscribe request
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
