#ifndef __TRADE_WND_H__
#define __TRADE_WND_H__


#if _MSC_VER > 1000
#pragma once
#endif

#include "resource.h"
#include <functional>

class CButtonItem : public CBCGPGridItem
{
public:
    CButtonItem(LPCTSTR lpszText, UINT id);

protected:
    virtual CWnd* CreateInPlaceEdit(CRect, BOOL&)
    {
        return nullptr;
    }

    virtual BOOL OnSetCursor() const
    {
        return FALSE;
    }

    virtual void OnDrawValue(CDC *pDC, CRect rect);
    virtual BOOL OnClickValue(UINT uiMsg, CPoint point);

    UINT m_id;
};

class CTradeWnd : public CBCGPDialog
{
public:
    CTradeWnd(CWnd *parent = NULL);
    virtual ~CTradeWnd();

public:
    enum { IDD = IDD_TRADEFORM };

    enum StockInfoCol
    {
        SIC_OpBuy = 0,
        SIC_Name,
        SIC_Price,
        SIC_Quant,
        SIC_OpSell,

        SIC_Num
    };

    enum TradeOrderCol
    {
        TOC_Cancel = 0,
        TOC_Local,
        TOC_Time,
        TOC_Code,
        TOC_Name,
        TOC_Flag,
        TOC_Price,
        TOC_Quant,
        TOC_Id,
        TOC_Turnover,

        TOC_Num
    };

public:
    CBCGPMaskEdit       _code;
    CBCGPEdit           _name;
    CBCGPEdit           _quota;
    CBCGPEdit           _left;
    CBCGPMaskEdit       _quantity;
    CBCGPSpinButtonCtrl _qSpin;
    CBCGPMaskEdit       _q1;
    CBCGPMaskEdit       _q2;
    CBCGPButton         _q1Minus;
    CBCGPButton         _q1Plus;
    CBCGPButton         _q2Minus;
    CBCGPButton         _q2Plus;
    CBCGPButton         _qDouble;
    CBCGPButton         _qHalf;
    CBCGPGridCtrl       _info;
    CBCGPGridCtrl       _order;

public:
    CBCGPGridRow*       AddOrderRow();
    void                GetPrice(StockInfoType info, CString & outPrice) const;

public:
    virtual void    DoDataExchange(CDataExchange *pDx);

public:
    //typedef std::function<void()>   ButtonClickEvent;
    typedef std::function<void()>   OKEvent;
    typedef std::function<void(CBCGPGridRow*)>    CancelOrderEvent;
    typedef std::function<void(StockInfoType, StockTradeOp)> TradeEvent;
    
    //ButtonClickEvent    _q1PlusClickEvent;
    //ButtonClickEvent    _q1MinusClickEvent;
    OKEvent             _enterOKEvent;
    TradeEvent          _tradeEvent;
    CancelOrderEvent    _cancelOrderEvent;

protected:
    virtual BOOL    OnInitDialog();

protected:
    afx_msg void    OnQ1PlusClicked();
    afx_msg void    OnQ1MinusClicked();
    afx_msg void    OnQ2PlusClicked();
    afx_msg void    OnQ2MinusClicked();
    afx_msg void    OnQDoubleClicked();
    afx_msg void    OnQHalfClicked();
    afx_msg void    OnDeltPosSpinCtrl(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void    OnEnter();
    afx_msg void    OnCancelOrder();
    afx_msg void    OnSellSell10();
    afx_msg void    OnSellSell9();
    afx_msg void    OnSellSell8();
    afx_msg void    OnSellSell7();
    afx_msg void    OnSellSell6();
    afx_msg void    OnSellSell5();
    afx_msg void    OnSellSell4();
    afx_msg void    OnSellSell3();
    afx_msg void    OnSellSell2();
    afx_msg void    OnSellSell1();
    afx_msg void    OnSellBuy1();
    afx_msg void    OnSellBuy2();
    afx_msg void    OnSellBuy3();
    afx_msg void    OnSellBuy4();
    afx_msg void    OnSellBuy5();
    afx_msg void    OnSellBuy6();
    afx_msg void    OnSellBuy7();
    afx_msg void    OnSellBuy8();
    afx_msg void    OnSellBuy9();
    afx_msg void    OnSellBuy10();
    afx_msg void    OnBuySell10();
    afx_msg void    OnBuySell9();
    afx_msg void    OnBuySell8();
    afx_msg void    OnBuySell7();
    afx_msg void    OnBuySell6();
    afx_msg void    OnBuySell5();
    afx_msg void    OnBuySell4();
    afx_msg void    OnBuySell3();
    afx_msg void    OnBuySell2();
    afx_msg void    OnBuySell1();
    afx_msg void    OnBuyBuy1();
    afx_msg void    OnBuyBuy2();
    afx_msg void    OnBuyBuy3();
    afx_msg void    OnBuyBuy4();
    afx_msg void    OnBuyBuy5();
    afx_msg void    OnBuyBuy6();
    afx_msg void    OnBuyBuy7();
    afx_msg void    OnBuyBuy8();
    afx_msg void    OnBuyBuy9();
    afx_msg void    OnBuyBuy10();
    afx_msg LRESULT HandleInitDialog(WPARAM wPram, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

private:
    CStatic         m_infoRect;
    CStatic         m_orderRect;
};




#endif
