//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_MUTEX_H__
#define __MESH_LOADER_MUTEX_H__

#if defined(__linux__)
typedef struct __MeshLoader_Linux_Mutex * __MeshLoader_Mutex;
#include "linux/linuxMutex.h"
#endif

#endif // __MESH_LOADER_MUTEX_H__
