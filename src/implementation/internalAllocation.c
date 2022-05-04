//
// Created by loghin on 5/4/22.
//

#include "internalAllocation.h"
#include "../config/instanceCnf.h"

#include <stdlib.h>
#include <stdio.h>

#include <meshLoader/utility>

static void * __MeshLoader_InternalAllocation_allocate (
        void                              *,
        MeshLoader_size,
        MeshLoader_size,
        MeshLoader_SystemAllocationScope
);

static void * __MeshLoader_InternalAllocation_reallocate (
        void                              *,
        void                              *,
        MeshLoader_size,
        MeshLoader_size,
        MeshLoader_SystemAllocationScope
);

static void __MeshLoader_InternalAllocation_free (
        void                              *,
        void                              *
);

#ifndef NDEBUG

static void __MeshLoader_InternalAllocation_allocationNotify (
        void                                      *,
        MeshLoader_AllocationNotification   const *
);

static void __MeshLoader_InternalAllocation_reallocationNotify (
        void                                      *,
        MeshLoader_AllocationNotification   const *
);

static void __MeshLoader_InternalAllocation_freeNotify (
        void                                      *,
        MeshLoader_AllocationNotification   const *
);

#endif

#if MESH_LOADER_DEBUG_MODE

static MeshLoader_AllocationCallbacks const __MeshLoader_InternalAllocation_debugCallbacks = {
        .structureType                              = MeshLoader_StructureType_AllocationCallbacks,
        .pNext                                      = NULL,
        .allocationFunction                         = & __MeshLoader_InternalAllocation_allocate,
        .reallocationFunction                       = & __MeshLoader_InternalAllocation_reallocate,
        .freeFunction                               = & __MeshLoader_InternalAllocation_free,
        .internalAllocationNotificationFunction     = & __MeshLoader_InternalAllocation_allocationNotify,
        .internalReallocationNotificationFunction   = & __MeshLoader_InternalAllocation_reallocationNotify,
        .internalFreeNotificationFunction           = & __MeshLoader_InternalAllocation_freeNotify
};

#else

static MeshLoader_AllocationCallbacks const __MeshLoader_InternalAllocation_callbacks = {
        .structureType                              = MeshLoader_StructureType_AllocationCallbacks,
        .pNext                                      = NULL,
        .allocationFunction                         = & __MeshLoader_InternalAllocation_allocate,
        .reallocationFunction                       = & __MeshLoader_InternalAllocation_reallocate,
        .freeFunction                               = & __MeshLoader_InternalAllocation_free,
        .internalAllocationNotificationFunction     = NULL,
        .internalReallocationNotificationFunction   = NULL,
        .internalFreeNotificationFunction           = NULL
};

#endif

MeshLoader_AllocationCallbacks const * __MeshLoader_InternalAllocation_getCallbacks () {

#if MESH_LOADER_DEBUG_MODE

    return & __MeshLoader_InternalAllocation_debugCallbacks;

#else

    return & __MeshLoader_InternalAllocation_callbacks;

#endif

}

static void * __MeshLoader_InternalAllocation_allocate (
        void                              * pUserData,
        MeshLoader_size                     size,
        MeshLoader_size                     alignment,
        MeshLoader_SystemAllocationScope    allocationScope
) {
    (void) alignment;
    (void) allocationScope;
    (void) pUserData;

    return malloc ( size );
}

static void * __MeshLoader_InternalAllocation_reallocate (
        void                              * pUserData,
        void                              * pOriginal,
        MeshLoader_size                     size,
        MeshLoader_size                     alignment,
        MeshLoader_SystemAllocationScope    allocationScope
) {
    (void) pUserData;
    (void) alignment;
    (void) allocationScope;

    return realloc ( pOriginal, size );
}

static void __MeshLoader_InternalAllocation_free (
        void                              * pUserData,
        void                              * pMemory
) {
    (void) pUserData;

    free ( pMemory );
}

#ifndef NDEBUG

static void __MeshLoader_InternalAllocation_allocationNotify (
        void                                      * pUserData,
        MeshLoader_AllocationNotification   const * pNotification
) {
    (void) pUserData;

    fprintf (
            stdout,
            "[%s:%d] Allocation of %llu bytes, aligned at %llu, scope : %s. Purpose : %s\n",
            __FILE__,
            __LINE__,
            pNotification->size,
            pNotification->alignment,
            MeshLoader_SystemAllocationScope_toString ( pNotification->allocationScope ),
            ( pNotification->explicitMemoryPurpose == NULL ? "Unspecified" : pNotification->explicitMemoryPurpose )
    );
}

static void __MeshLoader_InternalAllocation_reallocationNotify (
        void                                      * pUserData,
        MeshLoader_AllocationNotification   const * pNotification
) {
    (void) pUserData;

    fprintf (
            stdout,
            "[%s:%d] Reallocation of %llu bytes at %#020llx, aligned at %llu, scope : %s. Purpose : %s\n",
            __FILE__,
            __LINE__,
            pNotification->size,
            ( MeshLoader_size ) pNotification->pMemory,
            pNotification->alignment,
            MeshLoader_SystemAllocationScope_toString ( pNotification->allocationScope ),
            ( pNotification->explicitMemoryPurpose == NULL ? "Unspecified" : pNotification->explicitMemoryPurpose )
    );
}

static void __MeshLoader_InternalAllocation_freeNotify (
        void                                      * pUserData,
        MeshLoader_AllocationNotification   const * pNotification
) {
    (void) pUserData;

    fprintf (
            stdout,
            "[%s:%d] Free of %#020llx. Purpose : %s\n",
            __FILE__,
            __LINE__,
            ( MeshLoader_size ) pNotification->pMemory,
            ( pNotification->explicitMemoryPurpose == NULL ? "Unspecified" : pNotification->explicitMemoryPurpose )
    );
}

#endif
