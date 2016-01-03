#include "stdafx.h"
#include "TradeWnd.h"

CTradeWnd::CTradeWnd(CWnd * parent)
    : CBCGPDialog(CTradeWnd::IDD, parent)
{
    this->EnableVisualManagerStyle(globalData.m_bUseVisualManagerInBuiltInDialogs, TRUE);
}

CTradeWnd::~CTradeWnd()
{
}

BEGIN_MESSAGE_MAP(CTradeWnd, CBCGPDialog)
END_MESSAGE_MAP()

