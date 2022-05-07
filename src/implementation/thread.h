//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_THREAD_H__
#define __MESH_LOADER_THREAD_H__

typedef struct {
    void * pData;
} __MeshLoader_Thread_Parameter;

typedef struct {
    __MeshLoader_Thread_Parameter * pParameters;
    MeshLoader_uint32               parameterCount;
} __MeshLoader_Thread_Parameters;

typedef void ( * __MeshLoader_Thread_Function ) (
        __MeshLoader_Thread_Parameters const *
);

#if defined(__linux__)
typedef struct __MeshLoader_Posix_Thread * __MeshLoader_Thread;
#include "posix/posixThread.h"
#elif defined(WIN32)
typedef struct __MeshLoader_Win32_Thread * __MeshLoader_Thread;
#include "win32/win32Thread.h"
#endif

#endif // __MESH_LOADER_THREAD_H__
