//
// Created by loghin on 5/4/22.
//

#include "internalAllocation.h"
#include "../config/instanceCnf.h"

#include <stdlib.h>
#include <stdio.h>

#include <meshLoader/utility>
#include <stdalign.h>
#include <memory.h>

#include "mutex.h"
#include <stdatomic.h>

typedef struct __MeshLoader_InternalAllocation_MemoryTrackingNode {
    struct __MeshLoader_InternalAllocation_MemoryTrackingNode * pNext;
    MeshLoader_AllocationNotification                           notification;
} __MeshLoader_InternalAllocation_MemoryTrackingNode;

typedef struct {
    __MeshLoader_InternalAllocation_MemoryTrackingNode * pHead;
    MeshLoader_uint32                                    count;
    atomic_bool                                         mutexInitialized;
    __MeshLoader_Mutex                                   lock;
} __MeshLoader_InternalAllocation_MemoryTrackingList;

static __MeshLoader_InternalAllocation_MemoryTrackingList __MeshLoader_InternalAllocation_memoryTrackingList = {
        .pHead              = NULL,
        .count              = 0U,
        .mutexInitialized   = MeshLoader_false
};

static void __MeshLoader_InternalAllocation_trackAllocation (
        MeshLoader_AllocationNotification   const *
);

static void __MeshLoader_InternalAllocation_trackReallocation (
        MeshLoader_AllocationNotification   const *
);

static void __MeshLoader_InternalAllocation_trackFree (
        MeshLoader_AllocationNotification   const *
);

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

#if MESH_LOADER_DEBUG_MODE

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

#if MESH_LOADER_DEBUG_MODE

static void __MeshLoader_InternalAllocation_allocationNotify (
        void                                      * pUserData,
        MeshLoader_AllocationNotification   const * pNotification
) {
    (void) pUserData;

    __MeshLoader_InternalAllocation_trackAllocation ( pNotification );

    fprintf (
            stdout,
            "[%s:%d] Allocation of %llu bytes -> %#020llx, aligned at %llu, scope : %s. Purpose : %s\n",
            __FILE__,
            __LINE__,
            pNotification->size,
            ( MeshLoader_size ) pNotification->pMemory,
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

    if ( pNotification->pOldMemory == NULL ) {
        __MeshLoader_InternalAllocation_trackAllocation ( pNotification );
    } else {
        __MeshLoader_InternalAllocation_trackReallocation ( pNotification );
    }

    fprintf (
            stdout,
            "[%s:%d] Reallocation of %llu bytes at %#020llx from old %#020llx, aligned at %llu, scope : %s. Purpose : %s\n",
            __FILE__,
            __LINE__,
            pNotification->size,
            ( MeshLoader_size ) pNotification->pMemory,
            ( MeshLoader_size ) pNotification->pOldMemory,
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

    __MeshLoader_InternalAllocation_trackFree ( pNotification );

    fprintf (
            stdout,
            "[%s:%d] Free of %#020llx. Purpose : %s\n",
            __FILE__,
            __LINE__,
            ( MeshLoader_size ) pNotification->pMemory,
            ( pNotification->explicitMemoryPurpose == NULL ? "Unspecified" : pNotification->explicitMemoryPurpose )
    );
}

static void * __MeshLoader_InternalAllocation_trackingAllocateFunction (
        void                              * pUserData,
        MeshLoader_size                     size,
        MeshLoader_size                     alignment,
        MeshLoader_SystemAllocationScope    scope
) {
    (void) pUserData;
    (void) alignment;
    (void) scope;

    return malloc ( size );
}

static void * __MeshLoader_InternalAllocation_trackingReallocateFunction (
        void                              * pUserData,
        void                              * pOriginal,
        MeshLoader_size                     size,
        MeshLoader_size                     alignment,
        MeshLoader_SystemAllocationScope    scope
) {
    (void) pUserData;
    (void) alignment;
    (void) scope;

    return realloc ( pOriginal, size );
}

static void __MeshLoader_InternalAllocation_trackingFreeFunction (
        void                              * pUserData,
        void                              * pMemory
) {
    (void) pUserData;

    free ( pMemory );
}

static MeshLoader_AllocationCallbacks const __MeshLoader_InternalAllocation_trackingAllocationCallbacks = {
        .pUserData                                  = NULL,
        .allocationFunction                         = & __MeshLoader_InternalAllocation_trackingAllocateFunction,
        .reallocationFunction                       = & __MeshLoader_InternalAllocation_trackingReallocateFunction,
        .freeFunction                               = & __MeshLoader_InternalAllocation_trackingFreeFunction,
        .internalAllocationNotificationFunction     = NULL,
        .internalReallocationNotificationFunction   = NULL,
        .internalFreeNotificationFunction           = NULL
};

static __MeshLoader_ScopedAllocationCallbacks const __MeshLoader_InternalAllocation_trackingScopedAllocationCallbacks = {
        .pAllocationCallbacks       = & __MeshLoader_InternalAllocation_trackingAllocationCallbacks,
        .allocationScope            = MeshLoader_SystemAllocationScope_Component,
        .explicitAllocationPurpose  = NULL
};

static inline void __MeshLoader_InternalAllocation_initAllocationTracking () {

    (void) __MeshLoader_Mutex_applyModuleLock();

    __MeshLoader_InternalAllocation_memoryTrackingList.count = 0U;
    __MeshLoader_InternalAllocation_memoryTrackingList.pHead = NULL;

    if ( ! __MeshLoader_InternalAllocation_memoryTrackingList.mutexInitialized ) {
        (void) __MeshLoader_Mutex_create(
                & __MeshLoader_InternalAllocation_memoryTrackingList.lock,
                & __MeshLoader_InternalAllocation_trackingScopedAllocationCallbacks
        );

        __MeshLoader_InternalAllocation_memoryTrackingList.mutexInitialized = MeshLoader_true;
    }

    __MeshLoader_Mutex_removeModuleLock();
}

static inline MeshLoader_bool __MeshLoader_InternalAllocation_allocationTrackingListEmpty () {
    return __MeshLoader_InternalAllocation_memoryTrackingList.pHead == NULL;
}

static void __MeshLoader_InternalAllocation_trackAllocation (
        MeshLoader_AllocationNotification   const * pNotification
) {
    if ( __MeshLoader_InternalAllocation_allocationTrackingListEmpty () ) {
        __MeshLoader_InternalAllocation_initAllocationTracking ();
    }

    (void) __MeshLoader_Mutex_lock ( __MeshLoader_InternalAllocation_memoryTrackingList.lock );

    __MeshLoader_InternalAllocation_MemoryTrackingNode * pNode = ( __MeshLoader_InternalAllocation_MemoryTrackingNode * ) __MeshLoader_InternalAllocation_trackingAllocateFunction (
            NULL,
            sizeof ( __MeshLoader_InternalAllocation_MemoryTrackingNode ),
            alignof ( __MeshLoader_InternalAllocation_MemoryTrackingNode ),
            MeshLoader_SystemAllocationScope_Component
    );

    (void) memcpy ( & pNode->notification, pNotification, sizeof ( MeshLoader_AllocationNotification ) );
    pNode->pNext = __MeshLoader_InternalAllocation_memoryTrackingList.pHead;
    __MeshLoader_InternalAllocation_memoryTrackingList.pHead = pNode;
    ++ __MeshLoader_InternalAllocation_memoryTrackingList.count;

    __MeshLoader_Mutex_unlock ( __MeshLoader_InternalAllocation_memoryTrackingList.lock );
}

static void __MeshLoader_InternalAllocation_trackReallocation (
        MeshLoader_AllocationNotification   const * pNotification
) {

    (void) __MeshLoader_Mutex_lock ( __MeshLoader_InternalAllocation_memoryTrackingList.lock );
    __MeshLoader_InternalAllocation_MemoryTrackingNode * pHead = __MeshLoader_InternalAllocation_memoryTrackingList.pHead;

    while ( pHead != NULL ) {

        if ( pHead->notification.pMemory == pNotification->pOldMemory ) {
            pHead->notification.pMemory                 = pNotification->pMemory;
            pHead->notification.size                    = pNotification->size;
            pHead->notification.alignment               = pNotification->alignment;
            pHead->notification.allocationScope         = pNotification->allocationScope;
            pHead->notification.explicitMemoryPurpose   = pNotification->explicitMemoryPurpose;

            __MeshLoader_Mutex_unlock ( __MeshLoader_InternalAllocation_memoryTrackingList.lock );
            return;
        }

        pHead = pHead->pNext;
    }

    __MeshLoader_Mutex_unlock ( __MeshLoader_InternalAllocation_memoryTrackingList.lock );
}

static void __MeshLoader_InternalAllocation_trackFree (
        MeshLoader_AllocationNotification   const * pNotification
) {

    (void) __MeshLoader_Mutex_lock ( __MeshLoader_InternalAllocation_memoryTrackingList.lock );

    if ( __MeshLoader_InternalAllocation_memoryTrackingList.pHead->notification.pMemory == pNotification->pMemory ) {
        __MeshLoader_InternalAllocation_MemoryTrackingNode * pCopy = __MeshLoader_InternalAllocation_memoryTrackingList.pHead;
        __MeshLoader_InternalAllocation_memoryTrackingList.pHead = __MeshLoader_InternalAllocation_memoryTrackingList.pHead->pNext;

        __MeshLoader_InternalAllocation_trackingFreeFunction (
                NULL,
                pCopy
        );

        -- __MeshLoader_InternalAllocation_memoryTrackingList.count;
        __MeshLoader_Mutex_unlock ( __MeshLoader_InternalAllocation_memoryTrackingList.lock );
        return;
    }

    __MeshLoader_InternalAllocation_MemoryTrackingNode * pHead = __MeshLoader_InternalAllocation_memoryTrackingList.pHead;
    while ( pHead != NULL && pHead->pNext != NULL ) {

        if ( pHead->pNext->notification.pMemory == pNotification->pMemory ) {
            __MeshLoader_InternalAllocation_MemoryTrackingNode * pCopy = pHead->pNext;
            pHead->pNext = pHead->pNext->pNext;

            __MeshLoader_InternalAllocation_trackingFreeFunction (
                    NULL,
                    pCopy
            );

            -- __MeshLoader_InternalAllocation_memoryTrackingList.count;
            __MeshLoader_Mutex_unlock ( __MeshLoader_InternalAllocation_memoryTrackingList.lock );
            return;
        }

        pHead = pHead->pNext;
    }

    __MeshLoader_Mutex_unlock ( __MeshLoader_InternalAllocation_memoryTrackingList.lock );
}

void __MeshLoader_InternalAllocation_clear () {

    if ( __MeshLoader_InternalAllocation_memoryTrackingList.count > 0U ) {
        fprintf (
                stderr,
                "Warning : %u addresses not freed:\n",
                __MeshLoader_InternalAllocation_memoryTrackingList.count
        );
    }

    while ( __MeshLoader_InternalAllocation_memoryTrackingList.pHead != NULL ) {

        fprintf (
                stderr,
                "\t@ %#020llx, size = %llu, alignment = %llu, scope = %s, originalPurpose = %s\n",
                ( MeshLoader_size ) __MeshLoader_InternalAllocation_memoryTrackingList.pHead->notification.pMemory,
                __MeshLoader_InternalAllocation_memoryTrackingList.pHead->notification.size,
                __MeshLoader_InternalAllocation_memoryTrackingList.pHead->notification.alignment,
                MeshLoader_SystemAllocationScope_toString ( __MeshLoader_InternalAllocation_memoryTrackingList.pHead->notification.allocationScope ),
                __MeshLoader_InternalAllocation_memoryTrackingList.pHead->notification.explicitMemoryPurpose
        );

        __MeshLoader_InternalAllocation_MemoryTrackingNode * pCopy = __MeshLoader_InternalAllocation_memoryTrackingList.pHead;
        __MeshLoader_InternalAllocation_memoryTrackingList.pHead = __MeshLoader_InternalAllocation_memoryTrackingList.pHead->pNext;
        __MeshLoader_InternalAllocation_trackingFreeFunction (
                NULL,
                pCopy
        );
    }

    __MeshLoader_Mutex_destroy (
            __MeshLoader_InternalAllocation_memoryTrackingList.lock,
            & __MeshLoader_InternalAllocation_trackingScopedAllocationCallbacks
    );
}

#else

void __MeshLoader_InternalAllocation_clear () {
    /* left empty purposefully outside debug */
}

#endif
