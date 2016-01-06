#ifndef __STOCK_GLOBAL_H__
#define __STOCK_GLOBAL_H__

#if _MSC_VER > 1000
#pragma once
#endif

#define ST_APP_REGISTRY_KEY      _T("zhangzhen\\Stock\\1.0.0")

#define ST_APP_REGISTRY_SECTION  _T("Settings")

#define ST_CONFIG_FILE_EXT      (_T(".json"))
#define ST_CONFIG_FILE_FILTER   (_T("Json File (*.json)|*.json||"))

#define ST_APP_NAME             (_T("Stock"))

#define ST_LOCATE_WND_MSG       (WM_APP + 1)


#endif // !__STOCK_GLOBAL_H__
