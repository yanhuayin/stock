#include "stdafx.h"
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

    m_viewModel.clear();
    m_modelView.clear();

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

InfoNumArrayPtr CTradeControl::RequestInfo(TradeViewHandle h, CandidatesList * c)
{
    return InfoNumArrayPtr();
}
