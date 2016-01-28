#include "stdafx.h"
#include <algorithm>
#include "resource.h"
#include "TradeControl.h"
#include "TradeModel.h"
#include "TradeView.h"
#include "StockMainFrm.h"
#include "Stock.h"
#include "TradeOrder.h"
#include "TradeTime.h"

bool CTradeControl::Init(CStockMainFrame * pMainWnd)
{
    m_mainWnd = pMainWnd;
    m_init = true;
    return m_init;
}

bool CTradeControl::Close()
{
    m_init = false;

    m_modelView.clear();
    m_viewModel.clear();
    m_orderView.clear();
    m_viewOrder.clear();

    m_mainWnd = nullptr;

    theApp.AppData().Clean();

    return CTradeModelManager::Instance().Shutdown();
}

bool CTradeControl::NeedTimer() const
{
    // =================== TEST ===============================
    return CTradeModelManager::Instance().Type() == CTradeModelManager::RT_Immediate;
    //return false;
    // =================== TEST ===============================
}

void CTradeControl::ViewClosed(TradeViewHandle h)
{
    if (m_init)
    {
        {
            auto it = m_viewModel.find(h);
            if (it != m_viewModel.end())
            {
                TradeModelHandle hM = it->second;

                auto itM = m_modelView.find(hM);
                if (itM != m_modelView.end())
                {
                    itM->second->remove(h);
                    if (itM->second->empty())
                    {
                        //itM->second = nullptr;
                        CTradeModelManager::Instance().FreeModel(itM->first);

                        //m_modelView.erase(itM);
                    }
                }

                m_viewModel.erase(it);
            }
        }

        {
            auto it = m_viewOrder.find(h);
            if (it != m_viewOrder.end())
            {
                auto &ols = *(it->second);
                for (auto o : ols)
                {
                    m_orderView.erase(o);
                }
                it->second = nullptr;
                m_viewOrder.erase(it);
            }
        }

        m_mainWnd->RemoveView(h);
    }
}

bool CTradeControl::RequestInfo(TradeViewHandle h, CString const& code)
{
    if (m_init)
    {
        CTradeModelManager &cmm = CTradeModelManager::Instance();
        
        CTradeModelManager::CandidatesList cands;

        String tmpCode(code.GetString());
        TradeModelHandle hm = cmm.FindModel(tmpCode, &cands);

        if (hm)
        {
            if (this->Watch(h, hm)) // if refresh view
            {
                this->RefreshViewInfo(hm, h);
            }

            return true;
        }

        if (!cands.empty())
        {
            // TODO: more candidates found, let user choose one
            AfxMessageBox(_T("this function has not been implemented yet!"));
            return false;
        }

        CString err(MAKEINTRESOURCE(IDS_STOCK_NOT_FOUND));
        AfxMessageBox(err);
        return false;
    }

    return false;
}

void CTradeControl::RefreshViewsInfo(TradeModelHandle h) const
{
#ifdef DEBUG
    CTradeModelManager &cmm = CTradeModelManager::Instance();
    ASSERT(cmm.Type() == CTradeModelManager::RT_MultiThread);
#endif

    if (m_init && h)
    {
        auto it = m_modelView.find(h);

        if (it != m_modelView.end())
        {
            this->RefreshViewsInfo(h, it->second);
        }
    }
}

void CTradeControl::Update()
{
#ifdef DEBUG
    CTradeModelManager &cmm = CTradeModelManager::Instance();
    ASSERT(cmm.Type() == CTradeModelManager::RT_Immediate);
#endif

    if (m_init)
    {
        for (auto & i : m_modelView)
        {
            cmm.RequestModel(i.first, true);
            this->RefreshViewsInfo(i.first, i.second);
        }
    }
}

UINT CTradeControl::Quota() const
{
    return theApp.AppData().TradeSettingsData().Quota();
}

UINT CTradeControl::Left() const
{
    UINT trade = CTradeOrderManager::Instance().TotalTrade();
    UINT quota = theApp.AppData().TradeSettingsData().Quota();

    return quota >= trade ? (quota - trade) : 0;
}

void CTradeControl::RefreshViewsQuota(UINT quota) const
{
    CString quotaStr;
    quotaStr.Format(_T("%u"), quota);

    for (auto & i : m_viewModel)
    {
        i.first->SetQuota(quotaStr);
    }
}

void CTradeControl::RefreshViewsLeft(UINT left) const
{
    CString l; 
    l.Format(_T("%u"), left); 

    for (auto &i : m_viewModel) 
    {
        this->RefreshViewLeft(i.first, l); 
    }
}

TradeModelHandle CTradeControl::IsTradeAvailable(TradeViewHandle v, CString const& quant, bool showErr) const
{
    if (!theApp.AppData().LocateData().IsReady())
    {
        if (showErr)
            AfxMessageBox(IDS_TRADE_NO_LOC_ERR);

        return nullptr;
    }

    auto it = m_viewModel.find(v);
    if (it == m_viewModel.end())
    {
        if (showErr)
            AfxMessageBox(IDS_TRADE_NO_WATCH_ERR);

        return nullptr;
    }

    if (quant.IsEmpty())
    {
        if (showErr)
            AfxMessageBox(IDS_TRADE_NO_QUANT_ERR);

        return nullptr;
    }

    UINT left = this->Left();
    if (left == 0)
    {
        if (showErr)
            AfxMessageBox(IDS_TRADE_OVER_QUOTA_ERR);

        return nullptr;
    }

    UINT q = (UINT)_ttoi(quant);

    if (q > left)
    {
        if (showErr)
            AfxMessageBox(IDS_TRADE_OVER_QUOTA_ERR);

        return nullptr;
    }

    return it->second;
}

StockOrderResult CTradeControl::CancelOrder(TradeViewHandle h, int order)
{
    ASSERT(order > 0);

    CWaitCursor waiting;

    StockOrderResult res = CTradeOrderManager::Instance().CancelOrder(order);

    if (res == SOR_OK || res == SOR_Dealed || res == SOR_LeftOK)
    {
        auto it = m_viewOrder.find(h);
        if (it != m_viewOrder.end())
        {
            auto &olst = *(it->second);
            olst.remove(order);
        }

        m_orderView.erase(order);

        if (res == SOR_LeftOK)
        {
            TradeOrder const& o = CTradeOrderManager::Instance().Order(order);

            CString msg;
            msg.Format(IDS_ORDER_CANCEL_LEFT, o.turnover);

            AfxMessageBox(msg);
        }

    }

    return res;
}

int CTradeControl::Trade(TradeViewHandle h, StockInfoType info, StockTradeOp op)
{
    CString quant;
    h->GetQuant(quant);

    TradeModelHandle m = this->IsTradeAvailable(h, quant, true);
    if (!m)
        return -1;

    CWaitCursor waiting;

    CString code(m->Code().c_str());
    
    auto pInfo = m->NumInfo(SIF_Price);
    CString price;
    price.Format(_T("%.3f"), (*pInfo)[info]);

    CTradeOrderManager &tom = CTradeOrderManager::Instance();
    int res = tom.Trade(op, code, price, quant);

    if (res > 0)
    {
        m_orderView.insert(std::make_pair(res, h));

        auto it = m_viewOrder.find(h);
        if (it != m_viewOrder.end())
        {
            it->second->push_back(res);
        }
        else
        {
            OrderListPtr ls = OrderListPtr(new OrderList);
            ls->push_back(res);
            m_viewOrder.insert(std::make_pair(h, ls));
        }

        TradeOrder const& order = tom.Order(res);

        ViewOrderData vod;
        TimeToStr(order.local, vod.local);
        vod.code = order.code;
        vod.flag = order.flag;
        vod.price = order.price;
        vod.quant = order.quant;
        vod.turnover = order.turnover;
        
        if (!order.deal)
        {
            TimeToStr(order.time, vod.time);
            vod.id = order.id;
        }
        else
        {
            vod.time = vod.local;
        }

        h->ViewOrder(res, vod);
        h->FlushOrder();

        this->RefreshViewsLeft(this->Left());
    }
    else
    {
        AfxMessageBox(IDS_TRADE_ERROR);
    }

    return res;
}

bool CTradeControl::Watch(TradeViewHandle v, TradeModelHandle m)
{
    CTradeModelManager &cmm = CTradeModelManager::Instance();

    auto ivm = m_viewModel.find(v);

    if (ivm != m_viewModel.end())
    {
        if (ivm->second != m)
        {
            TradeModelHandle rm = ivm->second;

            auto irm = m_modelView.find(rm);
            if (irm != m_modelView.end())
            {
                auto &rvls = *(irm->second);
                auto irvls = std::find(rvls.begin(), rvls.end(), v);
                if (irvls != rvls.end())
                {
                    rvls.erase(irvls);

                    if (rvls.empty())
                    {
                        //irm->second = nullptr;
                        cmm.FreeModel(rm);

                        //m_modelView.erase(irm);
                    }
                }
            }
        }
        else
        {
            return false;
        }
    }

    m_viewModel[v] = m;

    auto imv = m_modelView.find(m);
    if (imv == m_modelView.end())
    {
        ViewListPtr vls(new ViewList);
        vls->push_back(v);

        m_modelView[m] = vls;

        if (cmm.RequestModel(m))
        {
            if (cmm.Type() == CTradeModelManager::RT_Immediate)
                return true;
        }

        return false;
    }
    else
    {
        if (imv->second->empty())
        {
            cmm.RequestModel(m);
            if (cmm.Type() == CTradeModelManager::RT_MultiThread)
                return false;
        }

        imv->second->push_back(v);
    }

    return true;
}

void CTradeControl::RefreshViewsInfo(TradeModelHandle h, ViewListPtr vls) const
{
    for (auto & i : (*vls))
    {
        i->SetName(h->Name().c_str());
        i->SetInfo(SIF_Price, h->NumInfo(SIF_Price));
        i->SetInfo(SIF_Quant, h->NumInfo(SIF_Quant));
        i->FlushInfo();
    }
}

void CTradeControl::RefreshViewInfo(TradeModelHandle m, TradeViewHandle v) const
{
    v->SetName(m->Name().c_str());
    v->SetInfo(SIF_Price, m->NumInfo(SIF_Price));
    v->SetInfo(SIF_Quant, m->NumInfo(SIF_Quant));
    v->FlushInfo();
}

void CTradeControl::RefreshViewLeft(TradeViewHandle h, CString const & left) const
{
    h->SetLeft(left);
}

//InfoNumArrayPtr CTradeControl::RequestInfo(TradeViewHandle h, CandidatesList * c)
//{
//    return InfoNumArrayPtr();
//}
