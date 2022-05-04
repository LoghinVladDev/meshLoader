//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_INTERNAL_ALLOCATION_H__
#define __MESH_LOADER_INTERNAL_ALLOCATION_H__

#include <meshLoader/publicTypes>

typedef struct {
    MeshLoader_AllocationCallbacks  const * pAllocationCallbacks;
    MeshLoader_SystemAllocationScope        allocationScope;
    MeshLoader_StringLiteral                explicitAllocationPurpose;
} __MeshLoader_ScopedAllocationCallbacks;

extern MeshLoader_AllocationCallbacks const * __MeshLoader_InternalAllocation_getCallbacks ();

#endif // __MESH_LOADER_INTERNAL_ALLOCATION_H__
