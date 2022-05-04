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

extern MeshLoader_Result MeshLoader_createJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsCreateInfo       const * pCreateInfo,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

extern void MeshLoader_destroyJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_uint32                       jobCount,
        MeshLoader_Job                  const * pJobs,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

#endif // __MESH_LOADER_MESH_LOADER_H__
