//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_PRIVATE_UTILITY_H__
#define __MESH_LOADER_PRIVATE_UTILITY_H__

#include <meshLoader/publicTypes>
#include "internalAllocation.h"

static inline MeshLoader_AllocationCallbacks const * __MeshLoader_Utility_nonNullAllocationCallbacks (
        MeshLoader_AllocationCallbacks const * pUserAllocationCallbacks
) {
    return pUserAllocationCallbacks == NULL ? __MeshLoader_InternalAllocation_getCallbacks () : pUserAllocationCallbacks;
}

static inline MeshLoader_size __MeshLoader_Utility_maxSize (
        MeshLoader_size a,
        MeshLoader_size b
) {
    return a > b ? a : b;
}

#endif // __MESH_LOADER_PRIVATE_UTILITY_H__
