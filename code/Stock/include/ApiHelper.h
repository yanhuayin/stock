#ifndef __API_HELPER_H__
#define __API_HELPER_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <memory>

typedef std::shared_ptr<void>   HandlePtr;
typedef std::shared_ptr<void>   VirtualPtr;

#define MakeHandlePtr(handle)  std::shared_ptr<void>((handle), [](HANDLE p){ if(p) ::CloseHandle(p); })
#define MakeVirtualPtr(vp)  std::shared_ptr<void>((vp), [&](LPVOID p){ if (p) ::VirtualFreeEx(process.get(), p, 0, MEM_RELEASE); })

inline BOOL FileExists(LPCTSTR path)
{
    DWORD dwAttrib = ::GetFileAttributes(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES) &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

HWND TopWndFromPoint(POINT pt);





#endif // !__API_HELPER_H__

