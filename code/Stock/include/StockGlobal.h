#ifndef __STOCK_GLOBAL_H__
#define __STOCK_GLOBAL_H__

#if _MSC_VER > 1000
#pragma once
#endif

#define ST_APP_REGISTRY_KEY      _T("zhangzhen\\Stock\\1.0.0")

#define ST_APP_REGISTRY_SECTION  _T("Settings")

#define ST_CONFIG_FILE_EXT      (_T(".json"))
#define ST_CONFIG_FILE_FILTER   (_T("Json File (*.json)|*.json||"))

#define ST_MAX_QUANTITY         9999999
#define ST_MIN_QUANTITY         0

#define ST_LOCATE_WND_MSG       (WM_APP + 1)
#define ST_LOAD_MSG             (WM_APP + 2)

#define ST_LOAD_ST_CONFIG       1
#define ST_INIT_DATABASE        2
#define ST_LOAD_ST_FINISH       5

#define ST_MAX_VIRTUAL_BUF      512


#endif // !__STOCK_GLOBAL_H__
