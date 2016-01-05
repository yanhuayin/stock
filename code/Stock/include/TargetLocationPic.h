#ifndef __TARGET_LOCATION_PIC_H__
#define __TARGET_LOCATION_PIC_H__

#if _MSC_VER > 1000
#pragma once
#endif

#define ST_LOCATE_TIME_ID       1
#define ST_LOCATE_TIME_ELAPSE   600

class CTargetLocationPic : public CStatic
{
public:
    CTargetLocationPic();

public:
    HWND            TargetHWnd() const { return m_tHWnd; }
    POINT const&    TargetPos() const { return m_pos; }

protected:
    afx_msg void    OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void    OnLButtonUp(UINT nFlags, CPoint point);

    DECLARE_MESSAGE_MAP()

private:
    CStatic m_pic;
    HWND    m_tHWnd;
    POINT   m_pos;
};

#endif // !__TARGET_LOCATION_PIC_H__

