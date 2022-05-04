//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_THREAD_H__
#define __MESH_LOADER_THREAD_H__

#if defined(__linux__)
typedef struct __MeshLoader_Posix_Thread * __MeshLoader_Thread;
#include "posix/posixThread.h"
#endif

#endif // __MESH_LOADER_THREAD_H__
