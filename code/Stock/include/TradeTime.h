#ifndef __TRADE_TIME_H__
#define __TRADE_TIME_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <ctime>
#include <chrono>
#include "stdafx.h"

#define ST_TIME_STR_FORMAT          _T("%H:%M:%S")
#define ST_TIME_STR_DELIMITER       _T(":")
#define CalcTimeDiff(t1, t2)        std::chrono::duration_cast<TradeDuration>((t1) - (t2))
#define CalcAbsDuration(dura, t1, t2)   if ((t1) > (t2)) dura = CalcTimeDiff((t1), (t2)); else dura = CalcTimeDiff((t2), (t1));

typedef std::chrono::system_clock               TradeClock;
typedef TradeClock::time_point                  TradeTimePoint;
typedef std::chrono::seconds                    TradeDuration;

// The string must have the format "%H:%M:%S"
inline TradeTimePoint  StrToTime(CString const& str)
{
    int pos = 0;
    CString hour = str.Tokenize(ST_TIME_STR_DELIMITER, pos);
    CString min = str.Tokenize(ST_TIME_STR_DELIMITER, pos);
    CString sec = str.Tokenize(ST_TIME_STR_DELIMITER, pos);

    CTime now = CTime::GetCurrentTime();
    std::tm t;
    now.GetLocalTm(&t);

    t.tm_sec    = _ttoi(sec);
    t.tm_min    = _ttoi(min);
    t.tm_hour   = _ttoi(hour);

    return TradeClock::from_time_t(std::mktime(&t));
}

inline void TimeToStr(TradeTimePoint const& time, CString &str)
{
    CTime ct(TradeClock::to_time_t(time));

    str = ct.Format(ST_TIME_STR_FORMAT);
}


#endif // !__TRADE_TIME_H__

