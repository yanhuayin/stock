#include "stdafx.h"
#include "ApiHelper.h"


HWND TopWndFromPoint(POINT pt)
{
    HWND unHwnd = ::WindowFromPoint(pt);

    HWND grayHwnd = ::GetWindow(unHwnd, GW_CHILD);
    RECT tempRc;
    BOOL bFind = FALSE;
    while (grayHwnd)
    {
        ::GetWindowRect(grayHwnd, &tempRc);
        if (::PtInRect(&tempRc, pt))
        {
            bFind = TRUE;
            break;
        }
        else
        {
            grayHwnd = ::GetWindow(grayHwnd, GW_HWNDNEXT);
        }
    }

    if (bFind == TRUE)
    {
        unHwnd = grayHwnd;
        bFind = FALSE;
    }

    return unHwnd;
}




