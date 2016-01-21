#include "stdafx.h"
#include <algorithm>
#include "resource.h"
#include "TradeControl.h"
#include "TradeModel.h"
#include "TradeView.h"
#include "StockMainFrm.h"
#include "Stock.h"
#include "TradeOrder.h"

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
                        itM->second = nullptr;
                        CTradeModelManager::Instance().FreeModel(itM->first);

                        m_modelView.erase(itM);
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

void CTradeControl::RefreshViewsInfo(TradeModelHandle h)
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

void CTradeControl::RefreshViewQuota(UINT quota) const
{
    CString quotaStr;
    quotaStr.Format(_T("%u"), quota);

    for (auto & i : m_viewModel)
    {
        i.first->SetQuota(quotaStr);
        i.first->Flush();
    }
}

void CTradeControl::RefreshViewsTrade()
{
    // TODO :
}

StockOrderResult CTradeControl::CancelOrder(TradeViewHandle h, int order)
{
    ASSERT(order > 0);

    StockOrderResult res = CTradeOrderManager::Instance().CancelOrder(order);

    if (res == SOR_OK || res == SOR_Dealed)
    {
        auto it = m_viewOrder.find(h);
        if (it != m_viewOrder.end())
        {
            auto &olst = *(it->second);
            olst.remove(order);
        }

        m_orderView.erase(order);
    }

    return res;
}

int CTradeControl::Trade(TradeViewHandle h, StockInfoType info, StockTradeOp op)
{
    CString code, price, quant;
    h->GetCode(code);
    h->GetQuant(quant);
    h->GetPrice(info, price);

    int res = CTradeOrderManager::Instance().Trade(op, code, price, quant);

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

        TradeOrder const& order = CTradeOrderManager::Instance().Order(res);

        CTradeView::OrderStrArray o;
        o[SOF_Code] = &(order.code);
        o[SOF_Name] = nullptr;
        o[SOF_Price] = &(order.price);
        o[SOF_Quant] = &(order.quant);
        o[SOF_Id] = &(order.id);

        h->SetOrder(res, o);
    }

    return res;
}

bool CTradeControl::Watch(TradeViewHandle v, TradeModelHandle m)
{
    CTradeModelManager &cmm = CTradeModelManager::Instance();

    auto ivm = m_viewModel.find(v);

    if (ivm != m_viewModel.end()) // it has watched a model
    {
        if (ivm->second != m) // check if the same model
        {
            TradeModelHandle rm = ivm->second;

            auto irm = m_modelView.find(rm);
            if (irm != m_modelView.end()) // disconnect it
            {
                auto rvls = *(irm->second);
                auto irvls = std::find(rvls.begin(), rvls.end(), v);
                if (irvls != rvls.end())
                {
                    rvls.erase(irvls);

                    if (rvls.empty())
                    {
                        irm->second = nullptr;
                        cmm.FreeModel(rm);

                        m_modelView.erase(irm);
                    }
                }
            }
        }
        else // has watched this model
        {
            return false; // return false then no need to refresh the view
        }
    }

    // if v has watched a model, now it is disconected or no watch at all
    m_viewModel[v] = m;

    auto imv = m_modelView.find(m);
    if (imv == m_modelView.end()) // first watch
    {
        ViewListPtr vls(new ViewList);
        vls->push_back(v);

        m_modelView[m] = vls;

        if (cmm.RequestModel(m))
        {
            if (cmm.Type() == CTradeModelManager::RT_Immediate)
                return true; // if request successfully, then refresh the view
        }

        return false; // for multi thread request, do not refresh at the first request
    }
    else // this model has been requested
    {
        imv->second->push_back(v); // watch it
    }

    return true;
}

void CTradeControl::RefreshViewsInfo(TradeModelHandle h, ViewListPtr vls)
{
    for (auto & i : (*vls))
    {
        i->SetName(h->Name().c_str());
        i->SetInfo(SIF_Price, h->NumInfo(SIF_Price));
        i->SetInfo(SIF_Quant, h->NumInfo(SIF_Quant));
        i->Flush();
    }
}

void CTradeControl::RefreshViewInfo(TradeModelHandle m, TradeViewHandle v)
{
    v->SetName(m->Name().c_str());
    v->SetInfo(SIF_Price, m->NumInfo(SIF_Price));
    v->SetInfo(SIF_Quant, m->NumInfo(SIF_Quant));
    v->Flush();
}

//InfoNumArrayPtr CTradeControl::RequestInfo(TradeViewHandle h, CandidatesList * c)
//{
//    return InfoNumArrayPtr();
//}
