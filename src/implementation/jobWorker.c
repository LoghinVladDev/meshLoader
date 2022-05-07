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

    MeshLoader_Result result;

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

    __MeshLoader_ScopedAllocationCallbacks scopedAllocationCallbacks = {
            .pAllocationCallbacks       = pAllocationCallbacks,
            .allocationScope            = MeshLoader_SystemAllocationScope_Object,
            .explicitAllocationPurpose  = NULL
    };

    result = __MeshLoader_Mutex_create (
            & pManager->lock,
            & scopedAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    allocationNotification.pMemory = pAllocationCallbacks->allocationFunction (
            pAllocationCallbacks->pUserData,
            allocationNotification.size,
            allocationNotification.alignment,
            allocationNotification.allocationScope
    );

    if ( allocationNotification.pMemory == NULL ) {
        __MeshLoader_Mutex_destroy (
                pManager->lock,
                & scopedAllocationCallbacks
        );

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

        result = __MeshLoader_Mutex_create (
                & pManager->pWorkers [ threadIndex ].lock,
                & scopedAllocationCallbacks
        );

        if ( result != MeshLoader_Result_Success ) {

            allocationNotification.pMemory                  = ( void * ) pManager->pWorkers;
            allocationNotification.size                     = sizeof ( __MeshLoader_JobWorker ) * threadCount;
            allocationNotification.alignment                = alignof ( __MeshLoader_JobWorker );
            allocationNotification.explicitMemoryPurpose    = "Destroys the Context for the Worker Threads";

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

            __MeshLoader_Mutex_destroy (
                    pManager->lock,
                    & scopedAllocationCallbacks
            );

            return result;
        }

        result = __MeshLoader_Thread_create (
                & pManager->pWorkers [ threadIndex ].thread,
                & __MeshLoader_JobWorker_main,
                NULL,
                & scopedAllocationCallbacks
        );

        if ( result != MeshLoader_Result_Success ) {

            for ( MeshLoader_uint32 secondaryThreadIndex = 0U; secondaryThreadIndex < pManager->length; ++ secondaryThreadIndex ) {

                __MeshLoader_Mutex_destroy (
                        pManager->pWorkers [ secondaryThreadIndex ].lock,
                        & scopedAllocationCallbacks
                );
            }

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

            __MeshLoader_Mutex_destroy (
                    pManager->lock,
                    & scopedAllocationCallbacks
            );

            return result;
        }

        pManager->pWorkers [ threadIndex ].pCurrentDispatcherContextNode = NULL;
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

    __MeshLoader_ScopedAllocationCallbacks scopedAllocationCallbacks = {
            .pAllocationCallbacks       = pAllocationCallbacks,
            .allocationScope            = MeshLoader_SystemAllocationScope_Object,
            .explicitAllocationPurpose  = NULL
    };

    for ( MeshLoader_uint32 threadIndex = 0U; threadIndex < pManager->length; ++ threadIndex ) {
        __MeshLoader_Thread_destroy (
                pManager->pWorkers [ threadIndex ].thread,
                & scopedAllocationCallbacks
        );

        __MeshLoader_Mutex_destroy (
                pManager->pWorkers [ threadIndex ].lock,
                & scopedAllocationCallbacks
        );
    }

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

    __MeshLoader_Mutex_destroy (
            pManager->lock,
            & scopedAllocationCallbacks
    );
}

MeshLoader_Result __MeshLoader_JobWorker_newJobsAddedNotification (
        __MeshLoader_JobWorker_Manager * pManager,
        MeshLoader_uint32                jobCount
) {

    MeshLoader_Result result;

    result = __MeshLoader_Mutex_lock (
            pManager->lock
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    for ( MeshLoader_uint32 workerIndex = 0U; jobCount != 0U && workerIndex < pManager->length; ++ workerIndex, -- jobCount ) {

        MeshLoader_bool isRunning = MeshLoader_true;

        __MeshLoader_Thread_isRunning (
                pManager->pWorkers [ workerIndex ].thread,
                & isRunning
        );

        if ( ! isRunning ) {
            __MeshLoader_Thread_start (
                    pManager->pWorkers [ workerIndex ].thread
            );
        }
    }

    return MeshLoader_Result_Success;
}

static void __MeshLoader_JobWorker_main (
        __MeshLoader_Thread_Parameters const * pParameters
) {
    (void) pParameters;
}
