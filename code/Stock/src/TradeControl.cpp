#include "stdafx.h"
#include <algorithm>
#include <atomic>
#include "resource.h"
#include "StockGlobal.h"
#include "TradeControl.h"
#include "TradeModel.h"
#include "TradeView.h"
#include "StockMainFrm.h"
#include "Stock.h"
#include "TradeOrder.h"
#include "TradeTime.h"

#define ST_MODEL_UPDATE_ELAPSE      3000

namespace
{
    std::atomic_bool        sQuit(false);
}

class CStockUpdateThread : public CWinThread
{
    DECLARE_DYNCREATE(CStockUpdateThread)

public:
    CStockUpdateThread() {}

public:
    virtual BOOL    InitInstance() { m_bAutoDelete = FALSE; return TRUE; }

protected:
    afx_msg void    OnUpdate(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP();
};

IMPLEMENT_DYNCREATE(CStockUpdateThread, CWinThread)

BEGIN_MESSAGE_MAP(CStockUpdateThread, CWinThread)
    ON_THREAD_MESSAGE(ST_MODEL_UPDATE_MSG, OnUpdate)
END_MESSAGE_MAP()

void CStockUpdateThread::OnUpdate(WPARAM wParam, LPARAM lParam)
{
    do
    {
        CTradeControl::Instance().Update();

        ::Sleep(ST_MODEL_UPDATE_ELAPSE);

    } while (!sQuit);

    ::AfxEndThread(0, FALSE);
}

bool CTradeControl::Init(CStockMainFrame * pMainWnd)
{
    m_mainWnd = pMainWnd;

    if (CTradeModelManager::Instance().Type() == CTradeModelManager::RT_Poll)
    {
        m_worker = (CStockUpdateThread*)AfxBeginThread(RUNTIME_CLASS(CStockUpdateThread), 0, 0);
        m_worker->PostThreadMessage(ST_MODEL_UPDATE_MSG, 0, 0);
    }

    m_init = true;
    return m_init;
}

bool CTradeControl::Close()
{
    if (CTradeModelManager::Instance().Type() == CTradeModelManager::RT_Poll)
    {
        sQuit = true;

        if (::WaitForSingleObject(m_worker->m_hThread, ST_MODEL_UPDATE_ELAPSE * 3) == WAIT_OBJECT_0)
        {
            DWORD err = 0;
            ::GetExitCodeThread(m_worker->m_hThread, &err);
        }
        else
        {
            // TODO:
        }

        ST_SAFE_DELETE(m_worker);
    }

    m_init = false;

    m_modelView.clear();
    m_viewModel.clear();
    m_orderView.clear();
    m_viewOrder.clear();

    m_mainWnd = nullptr;

    theApp.AppData().Clean();

    return CTradeModelManager::Instance().Shutdown();
}

void CTradeControl::ViewClosed(TradeViewHandle h)
{
    if (m_init)
    {
        {
            auto it = m_viewModel.find(h);
            if (it != m_viewModel.end())
            {
                UINT hM = it->second;

                auto itM = m_modelView.find(hM);
                if (itM != m_modelView.end())
                {
                    itM->second->remove(h);
                    if (itM->second->empty())
                    {
                        CTradeModelManager::Instance().FreeModel(hM);

                        Lock locker(m_mvMutex); // U ----> Update
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
        UINT hm = cmm.FindModel(tmpCode, &cands);

        if (hm != ST_MODEL_INVALID_ID)
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

        AfxMessageBox(IDS_STOCK_NOT_FOUND);
        return false;
    }

    return false;
}

void CTradeControl::RefreshViewsInfo(TradeModelHandle h) const
{
#ifdef DEBUG
    CTradeModelManager &cmm = CTradeModelManager::Instance();
    ASSERT(cmm.Type() == CTradeModelManager::RT_Subscribe);
#endif

    if (m_init && h)
    {
        auto it = m_modelView.find(h->Id());

        if (it != m_modelView.end())
        {
            this->RefreshViewsInfo(h, it->second);
        }
    }
}

void CTradeControl::RefreshViewsInfo(UINT m)
{
    auto it = m_modelView.find(m);
    if (it != m_modelView.end())
    {
        TradeModelHandle h = CTradeModelManager::Instance().ModelHandle(m);

        if (h)
        {
            this->RefreshViewsInfo(h, it->second);
        }
    }
}

void CTradeControl::Update()
{
    CTradeModelManager &cmm = CTradeModelManager::Instance();

    if (m_init)
    {
        Lock locker(m_mvMutex); // P ---> Watch/ViewClose

        for (auto & i : m_modelView)
        {
            if (cmm.RequestModel(i.first, true))
            {
                m_mainWnd->PostMessage(ST_MODEL_UPDATE_MSG, (WPARAM)(i.first), 0);
            }
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

UINT CTradeControl::IsTradeAvailable(TradeViewHandle v, CString const& quant, bool showErr) const
{
    if (!theApp.AppData().LocateData().IsReady())
    {
        if (showErr)
            AfxMessageBox(IDS_TRADE_NO_LOC_ERR);

        return ST_MODEL_INVALID_ID;
    }

    auto it = m_viewModel.find(v);
    if (it == m_viewModel.end())
    {
        if (showErr)
            AfxMessageBox(IDS_TRADE_NO_WATCH_ERR);

        return ST_MODEL_INVALID_ID;
    }

    if (quant.IsEmpty())
    {
        if (showErr)
            AfxMessageBox(IDS_TRADE_NO_QUANT_ERR);

        return ST_MODEL_INVALID_ID;
    }

    UINT left = this->Left();
    if (left == 0)
    {
        if (showErr)
            AfxMessageBox(IDS_TRADE_OVER_QUOTA_ERR);

        return ST_MODEL_INVALID_ID;
    }

    UINT q = (UINT)_ttoi(quant);

    if (q > left)
    {
        if (showErr)
            AfxMessageBox(IDS_TRADE_OVER_QUOTA_ERR);

        return ST_MODEL_INVALID_ID;
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

    UINT m = this->IsTradeAvailable(h, quant, true);
    if (m == ST_MODEL_INVALID_ID)
        return -1;

    CWaitCursor waiting;

    TradeModelHandle hM = CTradeModelManager::Instance().ModelHandle(m);

    CString code(hM->Code().c_str());
    
    CString price;
    h->GetPrice(info, price);

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

bool CTradeControl::Watch(TradeViewHandle v, UINT m)
{
    CTradeModelManager &cmm = CTradeModelManager::Instance();

    auto ivm = m_viewModel.find(v);

    if (ivm != m_viewModel.end())
    {
        if (ivm->second != m)
        {
            UINT rm = ivm->second;

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
                        cmm.FreeModel(rm);

                        Lock locker(m_mvMutex);
                        m_modelView.erase(irm);
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
        Lock locker(m_mvMutex); // U ---> Update

        ViewListPtr vls(new ViewList);
        vls->push_back(v);

        m_modelView[m] = vls;

        cmm.RequestModel(m);

        if (cmm.Type() == CTradeModelManager::RT_Poll)
            return true;

        return false;
    }
    else
    {
        imv->second->push_back(v);
    }

    return true;
}

void CTradeControl::RefreshViewsInfo(TradeModelHandle h, ViewListPtr vls) const
{
    InfoNumArray price, quant;

    ModelInfoData info = { &price, &quant };

    for (auto & i : (*vls))
    {
        i->SetName(h->Name().c_str());
        h->NumInfo(info);
        i->SetInfo(SIF_Price, price);
        i->SetInfo(SIF_Quant, quant);
        i->FlushInfo();
    }
}

void CTradeControl::RefreshViewInfo(UINT m, TradeViewHandle v) const
{
    TradeModelHandle h = CTradeModelManager::Instance().ModelHandle(m);

    if (h)
    {
        v->SetName(h->Name().c_str());

        InfoNumArray price, quant;
        ModelInfoData info = { &price, &quant };
        h->NumInfo(info);
        v->SetInfo(SIF_Price, price);
        v->SetInfo(SIF_Quant, quant);

        v->FlushInfo();
    }

}

void CTradeControl::RefreshViewLeft(TradeViewHandle h, CString const & left) const
{
    h->SetLeft(left);
}
