#include "stdafx.h"
#include "resource.h"
#include "ApiHelper.h"
#include "StockGlobal.h"
#include "TargetLocationPic.h"

BEGIN_MESSAGE_MAP(CTargetLocationPic, CStatic)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

CTargetLocationPic::CTargetLocationPic()
{
}

void CTargetLocationPic::OnLButtonDown(UINT nFlags, CPoint point)
{
    this->SetCapture();

    HCURSOR hc = ::LoadCursor(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_LOCATE_CURSOR));
    ::SetCursor(hc);

    this->ShowWindow(SW_HIDE);

    this->GetParent()->SetTimer(ST_LOCATE_TIME_ID, ST_LOCATE_TIME_ELAPSE, NULL);

    CStatic::OnLButtonDown(nFlags, point);
}

void CTargetLocationPic::OnLButtonUp(UINT nFlags, CPoint point)
{
    this->GetParent()->KillTimer(ST_LOCATE_TIME_ID);

    ReleaseCapture();

    this->ShowWindow(SW_SHOW);

    ::GetCursorPos(&m_pos);
    m_tHWnd = WinApi::TopWndFromPoint(m_pos);

    this->GetParent()->PostMessage(ST_LOCATE_WND_MSG, 0, 0);

    CStatic::OnLButtonUp(nFlags, point);
}
