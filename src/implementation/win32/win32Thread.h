//
// Created by loghin on 5/5/2022.
//

#ifndef __MESH_LOADER_WIN32_THREAD_H__
#define __MESH_LOADER_WIN32_THREAD_H__

#if defined(WIN32)

#include <meshLoader/publicTypes>
#include "../internalAllocation.h"

extern MeshLoader_Result __MeshLoader_Thread_create (
        struct __MeshLoader_Win32_Thread             **,
        __MeshLoader_Thread_Function,
        __MeshLoader_Thread_Parameters          const *,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern void __MeshLoader_Thread_join (
        struct __MeshLoader_Win32_Thread *
);

extern void __MeshLoader_Thread_detach (
        struct __MeshLoader_Win32_Thread *
);

extern void __MeshLoader_Thread_kill (
        struct __MeshLoader_Win32_Thread *
);

extern void __MeshLoader_Thread_start (
        struct __MeshLoader_Win32_Thread *
);

extern void __MeshLoader_Thread_destroy (
        struct __MeshLoader_Win32_Thread              *,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern void __MeshLoader_Thread_isRunning (
        struct __MeshLoader_Win32_Thread    *,
        MeshLoader_bool                     *
);

#endif

#endif // __MESH_LOADER_WIN32_THREAD_H__
