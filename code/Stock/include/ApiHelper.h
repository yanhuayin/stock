#ifndef __API_HELPER_H__
#define __API_HELPER_H__

#if _MSC_VER > 1000
#pragma once
#endif


inline BOOL FileExists(LPCTSTR path)
{
    DWORD dwAttrib = ::GetFileAttributes(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES) &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

HWND TopWndFromPoint(POINT pt);





#endif // !__API_HELPER_H__

