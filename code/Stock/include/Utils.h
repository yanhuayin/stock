#ifndef __STOCK_UTILS_H__
#define __STOCK_UTILS_H__

#if _MSC_VER > 1000
#pragma once
#endif

#define ST_SAFE_DELETE(p)           if(p) { delete (p); (p) = NULL;}
#define ST_SAFE_DELETE_ARRAY(p)     if(p) { delete [] (p); (p) = NULL; }
#define ST_SAFE_RELEASE(p)          if(p) {  (p)->release(); (p) = NULL;}

#define ST_ARRAY_SIZE(x)            (sizeof((x)) / sizeof((x)[0]))


#endif