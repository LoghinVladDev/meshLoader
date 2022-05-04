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

#endif // __MESH_LOADER_PRIVATE_UTILITY_H__
