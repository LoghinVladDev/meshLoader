//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_MUTEX_H__
#define __MESH_LOADER_MUTEX_H__

#if defined(__linux__) || defined(__APPLE__)
typedef struct __MeshLoader_Posix_Mutex * __MeshLoader_Mutex;
#include "posix/posixMutex.h"
#elif defined(WIN32)
typedef struct __MeshLoader_Win32_Mutex * __MeshLoader_Mutex;
#include "win32/win32Mutex.h"
#endif

#endif // __MESH_LOADER_MUTEX_H__
