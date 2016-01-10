#include "stdafx.h"
#include <algorithm>
#include "resource.h"
#include "TradeControl.h"
#include "TradeModel.h"
#include "TradeView.h"
#include "StockMainFrm.h"

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

    m_mainWnd = nullptr;

    return CTradeModelManager::Instance().Shutdown();
}

void CTradeControl::ViewClosed(TradeViewHandle h)
{
    if (m_init)
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
            return this->Watch(h, hm);
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

void CTradeControl::RefreshView(TradeModelHandle h)
{
    if (m_init && h)
    {
        auto it = m_modelView.find(h);

        if (it != m_modelView.end())
        {
            auto vls = *(it->second);

            for (auto i = vls.begin(); i != vls.end(); ++i)
            {
                (*i)->SetName(h->Name().c_str());
                (*i)->SetInfo(SIF_Price, h->NumInfo(SIF_Price));
                (*i)->SetInfo(SIF_Price, h->NumInfo(SIF_Quant));
                (*i)->Flush();
            }
        }
    }
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
            return true;
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

        return cmm.RequestModel(m);
    }
    else // this model has been requested
    {
        imv->second->push_back(v); // watch it
    }

    return true;
}

//InfoNumArrayPtr CTradeControl::RequestInfo(TradeViewHandle h, CandidatesList * c)
//{
//    return InfoNumArrayPtr();
//}
