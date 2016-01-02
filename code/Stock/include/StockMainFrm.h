#ifndef __STOCK_MAIN_FRM_H__
#define __STOCK_MAIN_FRM_H__

#if _MSC_VER > 1000
#pragma once
#endif

#define CMDIFrameWnd    CBCGPMDIFrameWnd


class CStockMainFrame : public CMDIFrameWnd
{
    DECLARE_DYNAMIC(CStockMainFrame)

public:
    CStockMainFrame();
    virtual ~CStockMainFrame();

public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //afx_msg void OnWindowManager();
    //afx_msg void OnViewCustomize();
    afx_msg void OnTradeNew();
    afx_msg void OnTradeLocate();
    afx_msg void OnTradeSettings();
    afx_msg void OnClose();
    DECLARE_MESSAGE_MAP()

protected:
    BOOL CreateDockingBars();
    void SetDockingBarsIcons(BOOL bHiColorIcons);

private:
    CBCGPMenuBar            m_menu;
    CBCGPToolBar            m_toolbar;
};




#endif
