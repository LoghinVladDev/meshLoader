//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_MESH_LOADER_H__
#define __MESH_LOADER_MESH_LOADER_H__

#include <meshLoader/publicTypes>

extern MeshLoader_Result MeshLoader_createInstance (
        MeshLoader_InstanceCreateInfo   const * pCreateInfo,
        MeshLoader_Instance                   * pInstance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

extern void MeshLoader_destroyInstance (
        MeshLoader_Instance                     instance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

#endif // __MESH_LOADER_MESH_LOADER_H__
