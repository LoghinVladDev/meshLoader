//
// Created by loghin on 5/8/22.
//

#include <memory.h>
#include <stdalign.h>
#include "jobMemoryAllocator.h"
#include "../config/instanceCnf.h"
#include "job.h"

MeshLoader_Result __MeshLoader_JobMemoryAllocator_construct (
        __MeshLoader_JobMemoryAllocator       * pMemoryAllocator,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacksToStore,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacksToUse
) {

    MeshLoader_Result result;

    __MeshLoader_ScopedAllocationCallbacks scopedAllocationCallbacks = {
            .pAllocationCallbacks       = pAllocationCallbacksToUse,
            .allocationScope            = MeshLoader_SystemAllocationScope_Object,
            .explicitAllocationPurpose  = NULL
    };

    result = __MeshLoader_Mutex_create (
            & pMemoryAllocator->allocatorLock,
            & scopedAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    (void) memcpy (
            & pMemoryAllocator->storedAllocationCallbacks,
            pAllocationCallbacksToStore,
            sizeof ( MeshLoader_AllocationCallbacks )
    );

    pMemoryAllocator->trackingList.pEntries = NULL;
    pMemoryAllocator->trackingList.length   = 0U;
    pMemoryAllocator->trackingList.capacity = 0U;
    pMemoryAllocator->freeCount             = 0U;

    return MeshLoader_Result_Success;
}

void __MeshLoader_JobMemoryAllocator_destruct (
        __MeshLoader_JobMemoryAllocator       * pMemoryAllocator,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    __MeshLoader_ScopedAllocationCallbacks scopedAllocationCallbacks = {
            .pAllocationCallbacks       = pAllocationCallbacks,
            .allocationScope            = MeshLoader_SystemAllocationScope_Object,
            .explicitAllocationPurpose  = NULL
    };

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType              = MeshLoader_StructureType_AllocationNotification,
            .pNext                      = NULL,
            .pMemory                    = NULL,
            .pOldMemory                 = NULL,
            .size                       = 0U,
            .alignment                  = 1U,
            .allocationScope            = MeshLoader_SystemAllocationScope_Worker,
            .explicitMemoryPurpose      = "Garbage Collector - Deletion of Memory allocated from a worker thread"
    };

    for ( MeshLoader_uint32 memoryIndex = 0U; memoryIndex < pMemoryAllocator->trackingList.length; ++ memoryIndex ) {

        if ( pMemoryAllocator->trackingList.pEntries [ memoryIndex ].pMemory == NULL ) {
            continue;
        }

        allocationNotification.pMemory      = pMemoryAllocator->trackingList.pEntries [ memoryIndex ].pMemory;
        allocationNotification.alignment    = pMemoryAllocator->trackingList.pEntries [ memoryIndex ].alignment;

        if ( pMemoryAllocator->storedAllocationCallbacks.internalFreeNotificationFunction != NULL ) {
            pMemoryAllocator->storedAllocationCallbacks.internalFreeNotificationFunction (
                    pMemoryAllocator->storedAllocationCallbacks.pUserData,
                    & allocationNotification
            );
        }

        pMemoryAllocator->storedAllocationCallbacks.freeFunction (
                pMemoryAllocator->storedAllocationCallbacks.pUserData,
                allocationNotification.pMemory
        );
    }

    allocationNotification.pMemory                  = pMemoryAllocator->trackingList.pEntries;
    allocationNotification.size                     = pMemoryAllocator->trackingList.capacity;
    allocationNotification.alignment                = alignof ( void * );
    allocationNotification.allocationScope          = MeshLoader_SystemAllocationScope_Object;
    allocationNotification.explicitMemoryPurpose    = "Garbage Collector - Destroys the array used to track memory allocated from a thread";

    if ( pMemoryAllocator->storedAllocationCallbacks.internalFreeNotificationFunction != NULL ) {
        pMemoryAllocator->storedAllocationCallbacks.internalFreeNotificationFunction (
                pMemoryAllocator->storedAllocationCallbacks.pUserData,
                & allocationNotification
        );
    }

    pMemoryAllocator->storedAllocationCallbacks.freeFunction (
            pMemoryAllocator->storedAllocationCallbacks.pUserData,
            allocationNotification.pMemory
    );

    __MeshLoader_Mutex_destroy (
            pMemoryAllocator->allocatorLock,
            & scopedAllocationCallbacks
    );
}

MeshLoader_Result MeshLoader_Job_allocateMemory2 (
        MeshLoader_Job_Context  context,
        MeshLoader_size         size,
        MeshLoader_size         alignment,
        void                 ** ppMemory
) {

    MeshLoader_Result result;
    MeshLoader_AllocationNotification allocationNotification = {
            .structureType              = MeshLoader_StructureType_AllocationNotification,
            .pNext                      = NULL,
            .pMemory                    = NULL,
            .pOldMemory                 = NULL,
            .size                       = size,
            .alignment                  = alignment,
            .allocationScope            = MeshLoader_SystemAllocationScope_Worker,
            .explicitMemoryPurpose      = "Memory allocated from a worker thread"
    };

    result = __MeshLoader_Mutex_lock (
            context->pMemoryAllocator->allocatorLock
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    allocationNotification.pMemory = context->pMemoryAllocator->storedAllocationCallbacks.allocationFunction (
            context->pMemoryAllocator->storedAllocationCallbacks.pUserData,
            allocationNotification.size,
            allocationNotification.alignment,
            allocationNotification.allocationScope
    );

    if ( allocationNotification.pMemory == NULL ) {
        __MeshLoader_Mutex_unlock (
                context->pMemoryAllocator->allocatorLock
        );

        return MeshLoader_Result_OutOfMemory;
    }

    if ( context->pMemoryAllocator->storedAllocationCallbacks.internalAllocationNotificationFunction != NULL ) {
        context->pMemoryAllocator->storedAllocationCallbacks.internalAllocationNotificationFunction (
                context->pMemoryAllocator->storedAllocationCallbacks.pUserData,
                & allocationNotification
        );
    }

    result = __MeshLoader_JobMemoryAllocator_trackMemory (
            context->pMemoryAllocator,
            allocationNotification.pMemory,
            alignment
    );

    if ( result != MeshLoader_Result_Success ) {
        allocationNotification.explicitMemoryPurpose = "Memory de-allocated from a worker thread due to garbage collector tracking error";

        if ( context->pMemoryAllocator->storedAllocationCallbacks.internalFreeNotificationFunction != NULL ) {
            context->pMemoryAllocator->storedAllocationCallbacks.internalFreeNotificationFunction (
                    context->pMemoryAllocator->storedAllocationCallbacks.pUserData,
                    & allocationNotification
            );
        }

        context->pMemoryAllocator->storedAllocationCallbacks.freeFunction (
                context->pMemoryAllocator->storedAllocationCallbacks.pUserData,
                allocationNotification.pMemory
        );

        __MeshLoader_Mutex_unlock (
                context->pMemoryAllocator->allocatorLock
        );

        return MeshLoader_Result_OutOfMemory;
    }

    * ppMemory = allocationNotification.pMemory;

    __MeshLoader_Mutex_unlock (
            context->pMemoryAllocator->allocatorLock
    );

    return MeshLoader_Result_Success;
}

static inline MeshLoader_size __MeshLoader_JobMemoryAllocator_max (
        MeshLoader_size a,
        MeshLoader_size b
) {
    return a > b ? a : b;
}

static MeshLoader_Result __MeshLoader_JobMemoryAllocator_trackMemory (
        __MeshLoader_JobMemoryAllocator       * pMemoryAllocator,
        void                                  * pMemory,
        MeshLoader_size                         alignment
) {

    if ( pMemoryAllocator->trackingList.length >= pMemoryAllocator->trackingList.capacity ) {

        MeshLoader_AllocationNotification allocationNotification = {
                .structureType          = MeshLoader_StructureType_AllocationNotification,
                .pNext                  = NULL,
                .pMemory                = NULL,
                .pOldMemory             = pMemoryAllocator->trackingList.pEntries,
                .size                   = __MeshLoader_JobMemoryAllocator_max ( pMemoryAllocator->trackingList.length + 1U, 2 * pMemoryAllocator->trackingList.capacity ) * sizeof ( __MeshLoader_JobMemoryAllocator_TrackedEntry ),
                .alignment              = alignof ( __MeshLoader_JobMemoryAllocator_TrackedEntry ),
                .allocationScope        = MeshLoader_SystemAllocationScope_Object,
                .explicitMemoryPurpose  = "Memory Resized, being the array used to track memory allocated from a Thread, for garbage collection in case of thread kill"
        };

        allocationNotification.pMemory = pMemoryAllocator->storedAllocationCallbacks.reallocationFunction (
                pMemoryAllocator->storedAllocationCallbacks.pUserData,
                allocationNotification.pOldMemory,
                allocationNotification.size,
                allocationNotification.alignment,
                allocationNotification.allocationScope
        );

        if ( allocationNotification.pMemory == NULL ) {

            allocationNotification.pMemory               = allocationNotification.pOldMemory;
            allocationNotification.explicitMemoryPurpose = "Deletes the array used to track memory allocated from a Thread, for garbage collection in case of thread kill - error on realloc";

            if ( pMemoryAllocator->storedAllocationCallbacks.internalFreeNotificationFunction != NULL ) {
                pMemoryAllocator->storedAllocationCallbacks.internalFreeNotificationFunction (
                        pMemoryAllocator->storedAllocationCallbacks.pUserData,
                        & allocationNotification
                );
            }

            pMemoryAllocator->storedAllocationCallbacks.freeFunction (
                    pMemoryAllocator->storedAllocationCallbacks.pUserData,
                    allocationNotification.pMemory
            );

            return MeshLoader_Result_OutOfMemory;
        }

        if ( pMemoryAllocator->storedAllocationCallbacks.internalReallocationNotificationFunction != NULL ) {
            pMemoryAllocator->storedAllocationCallbacks.internalReallocationNotificationFunction (
                    pMemoryAllocator->storedAllocationCallbacks.pUserData,
                    & allocationNotification
            );
        }

        pMemoryAllocator->trackingList.pEntries     = ( __MeshLoader_JobMemoryAllocator_TrackedEntry * ) allocationNotification.pMemory;
        pMemoryAllocator->trackingList.capacity     = allocationNotification.size / sizeof ( __MeshLoader_JobMemoryAllocator_TrackedEntry );

    }

    pMemoryAllocator->trackingList.pEntries [ pMemoryAllocator->trackingList.length ++ ] = ( __MeshLoader_JobMemoryAllocator_TrackedEntry ) {
            .pMemory    = pMemory,
            .alignment  = alignment
    };

    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_Job_freeMemory (
        MeshLoader_Job_Context    context,
        void                    * pMemory
) {

    MeshLoader_Result result;

    result = __MeshLoader_Mutex_lock (
            context->pMemoryAllocator->allocatorLock
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    result = __MeshLoader_JobMemoryAllocator_unTrackMemory (
            context->pMemoryAllocator,
            pMemory
    );

    if ( result != MeshLoader_Result_Success ) {
        __MeshLoader_Mutex_unlock (
                context->pMemoryAllocator->allocatorLock
        );

        return result;
    }

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType              = MeshLoader_StructureType_AllocationNotification,
            .pNext                      = NULL,
            .pMemory                    = pMemory,
            .pOldMemory                 = NULL,
            .size                       = 0U,
            .alignment                  = 0U,
            .allocationScope            = MeshLoader_SystemAllocationScope_Worker,
            .explicitMemoryPurpose      = "Memory freed from a worker thread"
    };

    if ( context->pMemoryAllocator->storedAllocationCallbacks.internalFreeNotificationFunction != NULL ) {
        context->pMemoryAllocator->storedAllocationCallbacks.internalFreeNotificationFunction (
                context->pMemoryAllocator->storedAllocationCallbacks.pUserData,
                & allocationNotification
        );
    }

    context->pMemoryAllocator->storedAllocationCallbacks.freeFunction (
            context->pMemoryAllocator->storedAllocationCallbacks.pUserData,
            allocationNotification.pMemory
    );

    __MeshLoader_Mutex_unlock (
            context->pMemoryAllocator->allocatorLock
    );

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_JobMemoryAllocator_unTrackMemory (
        __MeshLoader_JobMemoryAllocator       * pMemoryAllocator,
        void                                  * pMemory
) {

    for ( MeshLoader_uint32 memoryIndex = 0U; memoryIndex < pMemoryAllocator->trackingList.length; ++ memoryIndex ) {
        if ( pMemoryAllocator->trackingList.pEntries [ memoryIndex ].pMemory == pMemory ) {
            pMemoryAllocator->trackingList.pEntries [ memoryIndex ].pMemory     = NULL;
            pMemoryAllocator->trackingList.pEntries [ memoryIndex ].alignment   = 0U;
        }
    }

    pMemoryAllocator->freeCount ++;

    if (
            pMemoryAllocator->trackingList.length > 0U &&
            pMemoryAllocator->freeCount > pMemoryAllocator->trackingList.length
    ) {
        pMemoryAllocator->freeCount = pMemoryAllocator->trackingList.length - 1U;
    }

    if ( pMemoryAllocator->freeCount >= MESH_LOADER_JOB_MEMORY_ALLOCATOR_CLEANUP_PERIOD ) {

        MeshLoader_AllocationNotification allocationNotification = {
                .structureType              = MeshLoader_StructureType_AllocationNotification,
                .pNext                      = NULL,
                .pMemory                    = NULL,
                .pOldMemory                 = NULL,
                .size                       = sizeof ( __MeshLoader_JobMemoryAllocator_TrackedEntry ) * ( pMemoryAllocator->trackingList.length - pMemoryAllocator->freeCount ),
                .alignment                  = alignof ( __MeshLoader_JobMemoryAllocator_TrackedEntry ),
                .allocationScope            = MeshLoader_SystemAllocationScope_Object,
                .explicitMemoryPurpose      = "Garbage Collector - new, shrunk buffer to store tracked allocations from a thread",
        };

        allocationNotification.pMemory = pMemoryAllocator->storedAllocationCallbacks.allocationFunction (
                pMemoryAllocator->storedAllocationCallbacks.pUserData,
                allocationNotification.size,
                allocationNotification.alignment,
                allocationNotification.allocationScope
        );

        if ( allocationNotification.pMemory == NULL ) {
            return MeshLoader_Result_OutOfMemory;
        }

        if ( pMemoryAllocator->storedAllocationCallbacks.internalAllocationNotificationFunction != NULL ) {
            pMemoryAllocator->storedAllocationCallbacks.internalAllocationNotificationFunction (
                    pMemoryAllocator->storedAllocationCallbacks.pUserData,
                    & allocationNotification
            );
        }

        pMemoryAllocator->trackingList.capacity = allocationNotification.size / sizeof ( __MeshLoader_JobMemoryAllocator_TrackedEntry );
        MeshLoader_uint32 newLength = 0U;
        for ( MeshLoader_uint32 memoryIndex = 0U; memoryIndex < pMemoryAllocator->trackingList.length; ++ memoryIndex ) {
            if ( pMemoryAllocator->trackingList.pEntries [ memoryIndex ].pMemory != NULL ) {
                ( ( __MeshLoader_JobMemoryAllocator_TrackedEntry * ) allocationNotification.pMemory ) [ newLength ++ ] =
                        pMemoryAllocator->trackingList.pEntries [ memoryIndex ];
            }
        }

        pMemoryAllocator->trackingList.length = newLength;

        void * pNewMemory                               = allocationNotification.pMemory;
        allocationNotification.pMemory                  = pMemoryAllocator->trackingList.pEntries;
        allocationNotification.explicitMemoryPurpose    = "Garbage Collection - Deletion of the old tracking buffer, replaced by new shrunk one";

        if ( pMemoryAllocator->storedAllocationCallbacks.internalFreeNotificationFunction != NULL ) {
            pMemoryAllocator->storedAllocationCallbacks.internalFreeNotificationFunction (
                    pMemoryAllocator->storedAllocationCallbacks.pUserData,
                    & allocationNotification
            );
        }

        pMemoryAllocator->storedAllocationCallbacks.freeFunction (
                pMemoryAllocator->storedAllocationCallbacks.pUserData,
                allocationNotification.pMemory
        );

        pMemoryAllocator->trackingList.pEntries = ( __MeshLoader_JobMemoryAllocator_TrackedEntry * ) pNewMemory;
        pMemoryAllocator->freeCount = 0U;
    }

    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_Job_reallocateMemory2 (
        MeshLoader_Job_Context      context,
        void                      * pOldMemory,
        MeshLoader_size             newSize,
        MeshLoader_size             alignment,
        void                     ** ppNewMemory
) {

    if ( pOldMemory == NULL ) {
        return MeshLoader_Job_allocateMemory2 (
                context,
                newSize,
                alignment,
                ppNewMemory
        );
    }

    MeshLoader_Result result;
    MeshLoader_AllocationNotification allocationNotification = {
            .structureType              = MeshLoader_StructureType_AllocationNotification,
            .pNext                      = NULL,
            .pMemory                    = NULL,
            .pOldMemory                 = pOldMemory,
            .size                       = newSize,
            .alignment                  = alignment,
            .allocationScope            = MeshLoader_SystemAllocationScope_Worker,
            .explicitMemoryPurpose      = "Memory reallocated from a worker thread"
    };

    result = __MeshLoader_Mutex_lock (
            context->pMemoryAllocator->allocatorLock
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    allocationNotification.pMemory = context->pMemoryAllocator->storedAllocationCallbacks.reallocationFunction (
            context->pMemoryAllocator->storedAllocationCallbacks.pUserData,
            allocationNotification.pOldMemory,
            allocationNotification.size,
            allocationNotification.alignment,
            allocationNotification.allocationScope
    );

    if ( allocationNotification.pMemory == NULL ) {
        __MeshLoader_Mutex_unlock (
                context->pMemoryAllocator->allocatorLock
        );

        return MeshLoader_Result_OutOfMemory;
    }

    if ( context->pMemoryAllocator->storedAllocationCallbacks.internalReallocationNotificationFunction != NULL ) {
        context->pMemoryAllocator->storedAllocationCallbacks.internalReallocationNotificationFunction (
                context->pMemoryAllocator->storedAllocationCallbacks.pUserData,
                & allocationNotification
        );
    }

    result = __MeshLoader_JobMemoryAllocator_reTrackMemory (
            context->pMemoryAllocator,
            allocationNotification.pOldMemory,
            allocationNotification.pMemory,
            alignment
    );

    if ( result != MeshLoader_Result_Success ) {
        __MeshLoader_Mutex_unlock (
                context->pMemoryAllocator->allocatorLock
        );

        return result;
    }

    __MeshLoader_Mutex_unlock (
            context->pMemoryAllocator->allocatorLock
    );

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_JobMemoryAllocator_reTrackMemory (
        __MeshLoader_JobMemoryAllocator       * pMemoryAllocator,
        void                                  * pOldMemory,
        void                                  * pMemory,
        MeshLoader_size                         alignment
) {

    for ( MeshLoader_uint32 memoryIndex = 0U; memoryIndex < pMemoryAllocator->trackingList.length; ++ memoryIndex ) {
        if ( pMemoryAllocator->trackingList.pEntries [ memoryIndex ].pMemory == pOldMemory ) {
            pMemoryAllocator->trackingList.pEntries [ memoryIndex ] = ( __MeshLoader_JobMemoryAllocator_TrackedEntry ) {
                    .pMemory    = pMemory,
                    .alignment  = alignment
            };

            return MeshLoader_Result_Success;
        }
    }

    return MeshLoader_Result_Success;
}

extern MeshLoader_Result MeshLoader_Job_releaseMemory (
        MeshLoader_Job_Context  context,
        void                  * pMemory
) {

    MeshLoader_Result result;

    result = __MeshLoader_Mutex_lock (
            context->pMemoryAllocator->allocatorLock
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    result = __MeshLoader_JobMemoryAllocator_unTrackMemory (
            context->pMemoryAllocator,
            pMemory
    );

    __MeshLoader_Mutex_unlock ( context->pMemoryAllocator->allocatorLock );
    return result;
}
