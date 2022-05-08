//
// Created by loghin on 5/7/22.
//

#include "jobWorker.h"
#include "instance.h"
#include <stdalign.h>

MeshLoader_Result __MeshLoader_JobWorker_Manager_construct (
        MeshLoader_Instance                     instance,
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

    __MeshLoader_Thread_Parameter   threadParameterList[2U] = { {
                    .pData = NULL
            }, {
                    .pData = instance
            }
    };

    __MeshLoader_Thread_Parameters  threadParameters = {
            .parameterCount = 2U,
            .pParameters    = & threadParameterList[0]
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

        threadParameterList [ 0U ].pData = & pManager->pWorkers [ threadIndex ];

        result = __MeshLoader_Thread_create (
                & pManager->pWorkers [ threadIndex ].thread,
                & __MeshLoader_JobWorker_main,
                & threadParameters,
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

        pManager->pWorkers [ threadIndex ].pCurrentDispatcherContextNode    = NULL;
        pManager->pWorkers [ threadIndex ].pRuntimeContext                  = NULL;
        pManager->pWorkers [ threadIndex ].state                            = __MeshLoader_JobWorker_State_NotStarted;
        pManager->pWorkers [ threadIndex ].keepAlive                        = MeshLoader_false;
        pManager->pWorkers [ threadIndex ].errorReason                      = NULL;
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

MeshLoader_Result __MeshLoader_JobWorker_Manager_newJobsAddedNotification (
        __MeshLoader_JobWorker_Manager * pManager,
        MeshLoader_uint32                jobCount
) {

    MeshLoader_Result               result;

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

            pManager->pWorkers [ workerIndex ].keepAlive = MeshLoader_true;
            __MeshLoader_Thread_start (
                    pManager->pWorkers [ workerIndex ].thread
            );
        }
    }

    __MeshLoader_Mutex_unlock (
            pManager->lock
    );

    return MeshLoader_Result_Success;
}

MeshLoader_Result __MeshLoader_JobWorker_Manager_anyWorkersRunning (
        __MeshLoader_JobWorker_Manager * pManager,
        MeshLoader_bool                * pAnyRunning
) {

    MeshLoader_Result result;

    result = __MeshLoader_Mutex_lock (
            pManager->lock
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    for ( MeshLoader_uint32 workerIndex = 0U; workerIndex < pManager->length; ++ workerIndex ) {

        __MeshLoader_Thread_isRunning (
                pManager->pWorkers [ workerIndex ].thread,
                pAnyRunning
        );

        if ( * pAnyRunning ) {
            __MeshLoader_Mutex_unlock (
                    pManager->lock
            );

            return MeshLoader_Result_Success;
        }
    }

    * pAnyRunning = MeshLoader_false;
    __MeshLoader_Mutex_unlock (
            pManager->lock
    );

    return MeshLoader_Result_Success;
}

static void __MeshLoader_JobWorker_main (
        __MeshLoader_Thread_Parameters const * pParameters
) {

    MeshLoader_Result result;
    __MeshLoader_JobWorker * pThis      = ( __MeshLoader_JobWorker * ) pParameters->pParameters[0U].pData;
    MeshLoader_Instance      instance   = ( MeshLoader_Instance ) pParameters->pParameters[1U].pData;

    pThis->state = __MeshLoader_JobWorker_State_Initializing;

    while ( pThis->keepAlive ) {
        pThis->state = __MeshLoader_JobWorker_State_AcquiringJobContext;

        result = __MeshLoader_JobDispatcher_acquireJob (
                & instance->dispatcher,
                & pThis->pCurrentDispatcherContextNode,
                & pThis->pRuntimeContext,
                & pThis->activeJobPriority
        );

        if ( result != MeshLoader_Result_Success ) {
            pThis->state        = __MeshLoader_JobWorker_State_Error;
            pThis->errorReason  = "Failed to Acquire a job";
            break;
        }

        if ( pThis->pRuntimeContext == NULL ) {
            return;
        }

        /* use context & execute job step */

        pThis->state = __MeshLoader_JobWorker_State_ReleasingJobContext;

        result = __MeshLoader_JobDispatcher_releaseJob (
                & instance->dispatcher,
                pThis->pCurrentDispatcherContextNode,
                pThis->pRuntimeContext,
                pThis->activeJobPriority
        );

        if ( result != MeshLoader_Result_Success ) {
            pThis->state        = __MeshLoader_JobWorker_State_Error;
            pThis->errorReason  = "Failed to Release a job";
            break;
        }

        pThis->activeJobPriority                = .0f;
        pThis->pCurrentDispatcherContextNode    = NULL;
        pThis->pRuntimeContext                  = NULL;
    }

    if ( pThis->state == __MeshLoader_JobWorker_State_Error ) {
        /* empty for now */
    }

    pThis->state = __MeshLoader_JobWorker_State_Cleanup;
}