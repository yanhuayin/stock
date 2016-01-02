// Stock.h : main header file for the Stock application
//
#ifndef __STOCK_H__
#define __STOCK_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CStockApp:
// See Stock.cpp for the implementation of this class
//

class CStockApp : public CBCGPWinApp
{
public:
    CStockApp();
    virtual ~CStockApp();

    // Overrides
public:
    virtual BOOL    InitInstance();
    virtual int     ExitInstance();
    virtual void    PreLoadState();

    virtual void    LoadCustomState();
    virtual void    SaveCustomState();

    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CStockApp theApp;

#endif