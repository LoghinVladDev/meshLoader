//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_LINUX_MUTEX_H__
#define __MESH_LOADER_LINUX_MUTEX_H__

#include <meshLoader/publicTypes>
#include "../internalAllocation.h"

extern MeshLoader_Result __MeshLoader_applyModuleLock ();
extern void __MeshLoader_removeModuleLock ();

extern MeshLoader_Result __MeshLoader_Mutex_create (
        struct __MeshLoader_Linux_Mutex              **,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern void __MeshLoader_Mutex_destroy (
        struct __MeshLoader_Linux_Mutex               *,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern MeshLoader_Result __MeshLoader_Mutex_lock (
        struct __MeshLoader_Linux_Mutex *
);

extern void __MeshLoader_Mutex_unlock (
        struct __MeshLoader_Linux_Mutex *
);

#endif // __MESH_LOADER_LINUX_MUTEX_H__
