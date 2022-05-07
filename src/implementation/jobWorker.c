//
// Created by loghin on 5/7/22.
//

#include "jobWorker.h"
#include <stdalign.h>

MeshLoader_Result __MeshLoader_JobWorker_Manager_construct (
        __MeshLoader_JobWorker_Manager        * pManager,
        MeshLoader_uint32                       threadCount,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = sizeof ( __MeshLoader_JobWorker ) * threadCount,
            .alignment              = alignof ( __MeshLoader_JobWorker ),
            .allocationScope        = MeshLoader_SystemAllocationScope_Object,
            .explicitMemoryPurpose  = "Creates the Context for the Worker Threads"
    };

    allocationNotification.pMemory = pAllocationCallbacks->allocationFunction (
            pAllocationCallbacks->pUserData,
            allocationNotification.size,
            allocationNotification.alignment,
            allocationNotification.allocationScope
    );

    if ( allocationNotification.pMemory == NULL ) {
        return MeshLoader_Result_OutOfMemory;
    }

    if ( pAllocationCallbacks->internalAllocationNotificationFunction != NULL ) {
        pAllocationCallbacks->internalAllocationNotificationFunction (
                pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    pManager->pWorkers  = ( __MeshLoader_JobWorker * ) allocationNotification.pMemory;
    pManager->length    = threadCount;

    for ( MeshLoader_uint32 threadIndex = 0U; threadIndex < threadCount; ++ threadIndex ) {
        pManager->pWorkers [ threadIndex ].thread = __MeshLoader_Thread_InactiveThread;
    }

    return MeshLoader_Result_Success;
}

void __MeshLoader_JobWorker_Manager_destruct (
        __MeshLoader_JobWorker_Manager        * pManager,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = pManager->pWorkers,
            .pOldMemory             = NULL,
            .size                   = sizeof ( __MeshLoader_JobWorker ) * pManager->length,
            .alignment              = alignof ( __MeshLoader_JobWorker ),
            .allocationScope        = MeshLoader_SystemAllocationScope_Object,
            .explicitMemoryPurpose  = "Destroys the Context for the Worker Threads"
    };

    if ( pAllocationCallbacks->internalFreeNotificationFunction != NULL ) {
        pAllocationCallbacks->internalFreeNotificationFunction (
                pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    pAllocationCallbacks->freeFunction (
            pAllocationCallbacks->pUserData,
            allocationNotification.pMemory
    );
}
