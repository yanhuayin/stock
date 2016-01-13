#ifndef __STOCK_SPLASH_H__
#define __STOCK_SPLASH_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <functional>
#include "resource.h"


class CStockSplash : public CDialog
{
public:
    CStockSplash();
   ~CStockSplash();

public:
    enum { IDD = IDD_SPLASH };

public:
    virtual void    DoDataExchange(CDataExchange *pDX);

public:
    void    Worker(CWinThread *pThread) { m_worker = pThread; }

protected:
    virtual BOOL    OnInitDialog();

protected:
    afx_msg LRESULT OnLoading(WPARAM wParam, LPARAM lParam);
    afx_msg HBRUSH  OnCtlColor(CDC *pDC, CWnd *pWnd, UINT ctlColor);
    DECLARE_MESSAGE_MAP()

private:
    CString         m_status;
    CWinThread     *m_worker;
};




#endif
