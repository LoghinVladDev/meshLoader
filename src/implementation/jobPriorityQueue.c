//
// Created by loghin on 5/6/22.
//

#include <stdalign.h>
#include "jobPriorityQueue.h"

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

    allocationNotification.pMemory = pAllocationCallbacks->pAllocationCallbacks->allocationFunction (
            pAllocationCallbacks->pAllocationCallbacks->pUserData,
            allocationNotification.size,
            allocationNotification.alignment,
            allocationNotification.allocationScope
    );

    if ( allocationNotification.pMemory == NULL ) {
        return MeshLoader_Result_OutOfMemory;
    }

    if ( pAllocationCallbacks->pAllocationCallbacks->internalAllocationNotificationFunction != NULL ) {
        pAllocationCallbacks->pAllocationCallbacks->internalAllocationNotificationFunction (
                pAllocationCallbacks->pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    pQueue->pEntries    = ( __MeshLoader_JobPriorityQueue_Entry * ) allocationNotification.pMemory;
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
            .pMemory                = pQueue->pEntries,
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
}

MeshLoader_Result __MeshLoader_JobPriorityQueue_peek (
        __MeshLoader_JobPriorityQueue           const * pQueue,
        __MeshLoader_JobPriorityQueue_Entry    const ** ppEntry
) {

    if ( __MeshLoader_JobPriorityQueue_empty ( pQueue ) ) {
        return MeshLoader_Result_PriorityQueueEmpty;
    }

    * ppEntry = & pQueue->pEntries[0];

    return MeshLoader_Result_Success;
}

static inline void __MeshLoader_JobPriorityQueue_swap (
        __MeshLoader_JobPriorityQueue_Entry * pLeftEntry,
        __MeshLoader_JobPriorityQueue_Entry * pRightEntry
) {
    __MeshLoader_JobPriorityQueue_Entry auxiliary   = * pLeftEntry;
    * pLeftEntry                                    = * pRightEntry;
    * pRightEntry                                   = auxiliary;
}

MeshLoader_Result __MeshLoader_JobPriorityQueue_push (
        __MeshLoader_JobPriorityQueue   * pQueue,
        __MeshLoader_Job_RuntimeContext * pRuntimeContext,
        float                             priority
) {

    if ( __MeshLoader_JobPriorityQueue_full ( pQueue ) ) {
        return MeshLoader_Result_PriorityQueueFull;
    }

    MeshLoader_uint32 lastIndex     = pQueue->length;
    MeshLoader_uint32 parentIndex   = ( lastIndex - 1U ) / 2U;

    ++ pQueue->length;
    pQueue->pEntries [ lastIndex ].pContext = pRuntimeContext;
    pQueue->pEntries [ lastIndex ].priority = priority;

    while (
            lastIndex != 0U &&
            pQueue->pEntries [ parentIndex ].priority < pQueue->pEntries [ lastIndex ].priority
    ) {

        __MeshLoader_JobPriorityQueue_swap (
                & pQueue->pEntries [ parentIndex ],
                & pQueue->pEntries [ lastIndex ]
        );

        lastIndex   = parentIndex;
        parentIndex = ( lastIndex - 1 ) / 2;
    }

    return MeshLoader_Result_Success;
}

MeshLoader_Result __MeshLoader_JobPriorityQueue_pop (
        __MeshLoader_JobPriorityQueue       * pQueue,
        __MeshLoader_Job_RuntimeContext    ** ppRuntimeContext
) {

    if ( __MeshLoader_JobPriorityQueue_empty ( pQueue ) ) {
        return MeshLoader_Result_PriorityQueueEmpty;
    }

    * ppRuntimeContext = pQueue->pEntries [ 0U ].pContext;

    if ( pQueue->length == 1U ) {
        -- pQueue->length;
        return MeshLoader_Result_Success;
    }

    pQueue->pEntries[0] = pQueue->pEntries [ pQueue->length - 1U ];
    -- pQueue->length;

    MeshLoader_uint32 rootIndex = 0U;

    while ( MeshLoader_true ) {
        MeshLoader_uint32 leftIndex     = 2U * rootIndex + 1U;
        MeshLoader_uint32 rightIndex    = 2U * rootIndex + 2U;
        MeshLoader_uint32 smallestIndex = rootIndex;

        if (
                leftIndex < pQueue->length &&
                pQueue->pEntries [ leftIndex ].priority > pQueue->pEntries [ smallestIndex ].priority
        ) {
            smallestIndex = leftIndex;
        }

        if (
                rightIndex < pQueue->length &&
                pQueue->pEntries [ rightIndex ].priority > pQueue->pEntries [ smallestIndex ].priority
        ) {
            smallestIndex = rightIndex;
        }

        if ( smallestIndex == rootIndex ) {
            break;
        } else {

            __MeshLoader_JobPriorityQueue_swap (
                    & pQueue->pEntries [ rootIndex ],
                    & pQueue->pEntries [ smallestIndex ]
            );

            rootIndex = smallestIndex;
        }
    }

    return MeshLoader_Result_Success;
}