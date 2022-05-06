//
// Created by loghin on 5/5/2022.
//

#ifndef __MESH_LOADER_WIN32_MUTEX_H__
#define __MESH_LOADER_WIN32_MUTEX_H__

#include <meshLoader/publicTypes>
#include "../internalAllocation.h"

extern MeshLoader_Result __MeshLoader_Mutex_applyModuleLock ();
extern void __MeshLoader_Mutex_removeModuleLock ();

extern MeshLoader_Result __MeshLoader_Mutex_create (
        struct __MeshLoader_Win32_Mutex              **,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern void __MeshLoader_Mutex_destroy (
        struct __MeshLoader_Win32_Mutex               *,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern MeshLoader_Result __MeshLoader_Mutex_lock (
        struct __MeshLoader_Win32_Mutex *
);

extern void __MeshLoader_Mutex_unlock (
        struct __MeshLoader_Win32_Mutex *
);

extern void __MeshLoader_Mutex_clearModuleLock ();

#endif // __MESH_LOADER_WIN32_MUTEX_H__
