//
// Created by loghin on 5/6/22.
//

#include "jobPriorityQueue.h"
#include <stdalign.h>

extern MeshLoader_Result __MeshLoader_JobPriorityQueue_construct (
        __MeshLoader_JobPriorityQueue                 * pQueue,
        MeshLoader_uint32                               capacity,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_Result                   result;
    MeshLoader_AllocationNotification   allocationNotification = {
            .structureType              = MeshLoader_StructureType_AllocationNotification,
            .pNext                      = NULL,
            .pMemory                    = NULL,
            .pOldMemory                 = NULL,
            .size                       = sizeof ( __MeshLoader_JobPriorityQueue_Entry ) * capacity,
            .alignment                  = alignof ( __MeshLoader_JobPriorityQueue_Entry ),
            .allocationScope            = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose      = "Creates a Priority Queue Array Base ( Heap )"
    };

    result = __MeshLoader_Mutex_create (
            & pQueue->lock,
            pAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    allocationNotification.pMemory = pAllocationCallbacks->pAllocationCallbacks->allocationFunction (
            pAllocationCallbacks->pAllocationCallbacks->pUserData,
            allocationNotification.size,
            allocationNotification.alignment,
            allocationNotification.allocationScope
    );

    if ( allocationNotification.pMemory == NULL ) {
        __MeshLoader_Mutex_destroy (
                pQueue->lock,
                pAllocationCallbacks
        );

        return MeshLoader_Result_OutOfMemory;
    }

    if ( pAllocationCallbacks->pAllocationCallbacks->internalAllocationNotificationFunction != NULL ) {
        pAllocationCallbacks->pAllocationCallbacks->internalAllocationNotificationFunction (
                pAllocationCallbacks->pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    pQueue->ppEntries   = ( __MeshLoader_JobPriorityQueue_Entry * ) allocationNotification.pMemory;
    pQueue->capacity    = capacity;
    pQueue->length      = 0U;

    return MeshLoader_Result_Success;
}

extern void __MeshLoader_JobPriorityQueue_destruct (
        __MeshLoader_JobPriorityQueue           const * pQueue,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = pQueue->ppEntries,
            .pOldMemory             = NULL,
            .size                   = sizeof ( __MeshLoader_JobPriorityQueue_Entry ) * pQueue->capacity,
            .alignment              = alignof ( __MeshLoader_JobPriorityQueue_Entry ),
            .allocationScope        = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose  = "Destroys a Priority Queue Array Base ( Heap )"
    };

    if ( pAllocationCallbacks->pAllocationCallbacks->internalFreeNotificationFunction != NULL ) {
        pAllocationCallbacks->pAllocationCallbacks->internalFreeNotificationFunction (
                pAllocationCallbacks->pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    pAllocationCallbacks->pAllocationCallbacks->freeFunction (
            pAllocationCallbacks->pAllocationCallbacks->pUserData,
            allocationNotification.pMemory
    );

    __MeshLoader_Mutex_destroy (
            pQueue->lock,
            pAllocationCallbacks
    );
}
