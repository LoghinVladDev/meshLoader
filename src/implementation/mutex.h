//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_MUTEX_H__
#define __MESH_LOADER_MUTEX_H__

#if defined(__linux__)
typedef struct __MeshLoader_Posix_Mutex * __MeshLoader_Mutex;
#include "posix/posixMutex.h"
#endif

#endif // __MESH_LOADER_MUTEX_H__
