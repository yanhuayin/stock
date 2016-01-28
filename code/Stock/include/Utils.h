#ifndef __STOCK_UTILS_H__
#define __STOCK_UTILS_H__

#if _MSC_VER > 1000
#pragma once
#endif

#define ST_SAFE_DELETE(p)           if(p) { delete (p); (p) = nullptr;}
#define ST_SAFE_DELETE_ARRAY(p)     if(p) { delete [] (p); (p) = nullptr; }
#define ST_SAFE_RELEASE(p)          if(p) {  (p)->release(); (p) = nullptr;}

#define ST_ARRAY_SIZE(x)            (sizeof((x)) / sizeof((x)[0]))

template <typename T>
T clamp(T const& x, T const& lower_bound, T const& upper_bound)
{
    if (x < lower_bound) return lower_bound;
    if (x > upper_bound) return upper_bound;

    return x;
}


template<typename T>
class Singleton
{
public:
    static T&   Instance() { static T _instance; return _instance; }
    static T*   Instance_ptr() { return &Instance(); }

public:
    Singleton() {}
    virtual ~Singleton() {}

private:
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);
};

#endif