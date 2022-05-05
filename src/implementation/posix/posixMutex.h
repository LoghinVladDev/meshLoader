//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_POSIX_MUTEX_H__
#define __MESH_LOADER_POSIX_MUTEX_H__

#include <meshLoader/publicTypes>
#include "../internalAllocation.h"

extern MeshLoader_Result __MeshLoader_Mutex_applyModuleLock ();
extern void __MeshLoader_Mutex_removeModuleLock ();
extern void __MeshLoader_Mutex_clearModuleLock ();

extern MeshLoader_Result __MeshLoader_Mutex_create (
        struct __MeshLoader_Posix_Mutex              **,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern void __MeshLoader_Mutex_destroy (
        struct __MeshLoader_Posix_Mutex               *,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern MeshLoader_Result __MeshLoader_Mutex_lock (
        struct __MeshLoader_Posix_Mutex *
);

extern void __MeshLoader_Mutex_unlock (
        struct __MeshLoader_Posix_Mutex *
);

#endif // __MESH_LOADER_POSIX_MUTEX_H__
