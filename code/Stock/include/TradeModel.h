#ifndef __TRADE_MODEL_H__
#define __TRADE_MODEL_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <array>
#include <unordered_map>
#include "StockConfig.h"
#include "Utils.h"

class CTradeModel : public std::enable_shared_from_this<CTradeModel>
{
public:
    CTradeModel() {}
   ~CTradeModel() {}

public:

private:
    friend class CTradeModelManager;

    CString         m_code;
    CString         m_name;

    InfoNumArrayPtr    m_price;
    InfoNumArrayPtr    m_quant;
};


class CTradeModelManager : public Singleton<CTradeModelManager>
{
public:
    CTradeModelManager() {}
   ~CTradeModelManager() {}

public:
    bool    Init();
    bool    Shutdown();

public:
    void    FreeModel(TradeModelHandle h);

private:
    typedef std::unordered_map<CString, TradeModelHandle>   ModelMap;
    ModelMap        m_models;

    CString         m_00;
    CString         m_30;
    CString         m_60;

    CString         m_date;
};


#endif
