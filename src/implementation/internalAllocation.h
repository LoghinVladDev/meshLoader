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
extern void __MeshLoader_InternalAllocation_clear ();

static inline MeshLoader_bool __MeshLoader_InternalAllocation_allocationCallbacksEqual (
        MeshLoader_AllocationCallbacks  const * pLeft,
        MeshLoader_AllocationCallbacks  const * pRight
) {

    if ( pLeft == pRight ) {
        return MeshLoader_true;
    }

    if ( pLeft == NULL || pRight == NULL ) {
        return MeshLoader_false;
    }

    return
            pLeft->pUserData            == pRight->pUserData &&
            pLeft->allocationFunction   == pRight->allocationFunction &&
            pLeft->reallocationFunction == pRight->reallocationFunction &&
            pLeft->freeFunction         == pRight->freeFunction;
}

#endif // __MESH_LOADER_INTERNAL_ALLOCATION_H__
