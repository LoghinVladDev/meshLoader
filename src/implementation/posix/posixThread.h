//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_POSIX_THREAD_H__
#define __MESH_LOADER_POSIX_THREAD_H__

#if defined(__linux__)

#include <meshLoader/publicTypes>
#include "../internalAllocation.h"

extern MeshLoader_Result __MeshLoader_Thread_create (
        struct __MeshLoader_Posix_Thread             **,
        __MeshLoader_Thread_Function,
        __MeshLoader_Thread_Parameters          const *,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern void __MeshLoader_Thread_join (
        struct __MeshLoader_Posix_Thread *
);

extern void __MeshLoader_Thread_detach (
        struct __MeshLoader_Posix_Thread *
);

extern void __MeshLoader_Thread_kill (
        struct __MeshLoader_Posix_Thread *
);

extern void __MeshLoader_Thread_start (
        struct __MeshLoader_Posix_Thread *
);

extern void __MeshLoader_Thread_destroy (
        struct __MeshLoader_Posix_Thread              *,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern void __MeshLoader_Thread_isRunning (
        struct __MeshLoader_Posix_Thread    *,
        MeshLoader_bool                     *
);

#endif

#endif // __MESH_LOADER_POSIX_THREAD_H__
