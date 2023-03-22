//
// Created by loghin on 5/6/22.
//

#include <stdalign.h>
#include "jobDispatcher.h"
#include "instance.h"
#include "jobWorker.h"
#include "job.h"
#include <memory.h>
#include <meshLoader/customJob>

MeshLoader_Result __MeshLoader_JobDispatcher_construct (
        __MeshLoader_JobDispatcher            * pDispatcher,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_Result result;

    __MeshLoader_ScopedAllocationCallbacks scopedAllocationCallbacks = {
            .pAllocationCallbacks       = pAllocationCallbacks,
            .allocationScope            = MeshLoader_SystemAllocationScope_Object,
            .explicitAllocationPurpose  = NULL
    };

    pDispatcher->contextList = NULL;

    result = __MeshLoader_Mutex_create (
            & pDispatcher->lock,
            & scopedAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    return MeshLoader_Result_Success;
}

void __MeshLoader_JobDispatcher_destruct (
        __MeshLoader_JobDispatcher            * pDispatcher,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    __MeshLoader_ScopedAllocationCallbacks scopedAllocationCallbacks = {
            .pAllocationCallbacks           = pAllocationCallbacks,
            .allocationScope                = MeshLoader_SystemAllocationScope_Object,
            .explicitAllocationPurpose      = NULL
    };

    while ( pDispatcher->contextList != NULL ) {
        __MeshLoader_JobDispatcher_ContextNode * pToDelete  = pDispatcher->contextList;
        pDispatcher->contextList                            = pDispatcher->contextList->pNext;
        __MeshLoader_JobDispatcher_freeContext ( pToDelete );
    }

    __MeshLoader_Mutex_destroy (
            pDispatcher->lock,
            & scopedAllocationCallbacks
    );
}

static void __MeshLoader_JobDispatcher_freeContext (
        __MeshLoader_JobDispatcher_ContextNode * pNode
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = sizeof ( __MeshLoader_JobDispatcher_ContextNode ),
            .alignment              = alignof ( __MeshLoader_JobDispatcher_ContextNode ),
            .allocationScope        = MeshLoader_SystemAllocationScope_Object,
            .explicitMemoryPurpose  = "Destroys a JobDispatcher Context, holding a Job Queue for a started Job Batch"
    };

    __MeshLoader_ScopedAllocationCallbacks scopedAllocationCallbacks = {
            .pAllocationCallbacks       = & pNode->context.allocationCallbacks,
            .allocationScope            = MeshLoader_SystemAllocationScope_Object,
            .explicitAllocationPurpose  = NULL
    };

    __MeshLoader_Job_RuntimeContext * pJobRuntimeContext;

    while ( ! __MeshLoader_JobPriorityQueue_empty ( & pNode->context.jobQueue ) ) {
        (void) __MeshLoader_JobPriorityQueue_pop (
                & pNode->context.jobQueue,
                & pJobRuntimeContext
        );

        __MeshLoader_JobMemoryAllocator_destruct (
                & pJobRuntimeContext->jobMemoryAllocator,
                & pNode->context.allocationCallbacks
        );
    }

    __MeshLoader_JobPriorityQueue_destruct (
            & pNode->context.jobQueue,
            & scopedAllocationCallbacks
    );

    /* TODO: clear the inactive queue as well */

    allocationNotification.pMemory = pNode;

    if ( pNode->context.allocationCallbacks.internalFreeNotificationFunction != NULL ) {
        pNode->context.allocationCallbacks.internalFreeNotificationFunction (
                pNode->context.allocationCallbacks.pUserData,
                & allocationNotification
        );
    }

    pNode->context.allocationCallbacks.freeFunction (
            pNode->context.allocationCallbacks.pUserData,
            allocationNotification.pMemory
    );
}

static MeshLoader_Result __MeshLoader_JobDispatcher_allocateContext (
        __MeshLoader_JobDispatcher_ContextNode       ** ppNode,
        MeshLoader_JobsStartInfo                const * pStartInfo
) {

    MeshLoader_Result                       result;
    MeshLoader_AllocationCallbacks  const * pAllocationCallbacks        = __MeshLoader_InternalAllocation_getCallbacks ( pStartInfo->pAllocationCallbacks );
    MeshLoader_AllocationNotification       allocationNotification      = {
            .structureType              = MeshLoader_StructureType_AllocationNotification,
            .pNext                      = NULL,
            .pMemory                    = NULL,
            .pOldMemory                 = NULL,
            .size                       = sizeof ( __MeshLoader_JobDispatcher_ContextNode ),
            .alignment                  = alignof ( __MeshLoader_JobDispatcher_ContextNode ),
            .allocationScope            = MeshLoader_SystemAllocationScope_Object,
            .explicitMemoryPurpose      = "Creates a Job Dispatcher Context Node that contains a Job Priority Queue"
    };

    __MeshLoader_ScopedAllocationCallbacks  scopedAllocationCallbacks   = {
            .pAllocationCallbacks       = pAllocationCallbacks,
            .allocationScope            = MeshLoader_SystemAllocationScope_Object,
            .explicitAllocationPurpose  = NULL
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

    * ppNode = ( __MeshLoader_JobDispatcher_ContextNode * ) allocationNotification.pMemory;

    (void) memcpy (
            & ( * ppNode )->context.allocationCallbacks,
            pAllocationCallbacks,
            sizeof ( MeshLoader_AllocationCallbacks )
    );

    ( * ppNode )->context.jobCount          = pStartInfo->jobCount;
    ( * ppNode )->context.finishedJobCount  = 0U;

    result = __MeshLoader_JobPriorityQueue_construct (
            & ( * ppNode )->context.jobQueue,
            pStartInfo->jobCount,
            & scopedAllocationCallbacks
    );

    /* TODO: construct inactive queue as well */

    if ( result != MeshLoader_Result_Success )  {
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

        return result;
    }

    for ( MeshLoader_uint32 jobIndex = 0U; jobIndex < pStartInfo->jobCount; ++ jobIndex ) {
        result = __MeshLoader_JobPriorityQueue_push (
                & ( * ppNode )->context.jobQueue,
                & pStartInfo->pJobs [ jobIndex ]->context,
                pStartInfo->pJobs [ jobIndex ]->priority
        );

        if ( result != MeshLoader_Result_Success ) {

            __MeshLoader_JobPriorityQueue_destruct (
                    & ( * ppNode )->context.jobQueue,
                    & scopedAllocationCallbacks
            );

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

            return result;
        }

        result = __MeshLoader_JobMemoryAllocator_construct (
                & pStartInfo->pJobs[jobIndex]->context.jobMemoryAllocator,
                pAllocationCallbacks,
                pAllocationCallbacks
        );

        if ( result != MeshLoader_Result_Success ) {

            __MeshLoader_JobPriorityQueue_destruct (
                    & ( * ppNode )->context.jobQueue,
                    & scopedAllocationCallbacks
            );

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

            return result;
        }
    }

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_JobDispatcher_createContext (
        __MeshLoader_JobDispatcher        * pDispatcher,
        MeshLoader_JobsStartInfo    const * pStartInfo
) {

    __MeshLoader_JobDispatcher_ContextNode    * pNewNode;
    MeshLoader_Result                           result;

    result = __MeshLoader_JobDispatcher_allocateContext (
            & pNewNode,
            pStartInfo
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    pNewNode->pNext             = pDispatcher->contextList;
    pDispatcher->contextList    = pNewNode;

    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_startJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsStartInfo        const * pStartInfo
) {
    MeshLoader_Result result;

    result = __MeshLoader_JobDispatcher_createContext (
            & instance->dispatcher,
            pStartInfo
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    result = __MeshLoader_JobWorker_Manager_newJobsAddedNotification (
            & instance->workerManager,
            pStartInfo->jobCount
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_pauseJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsPauseInfo        const * pPauseInfo
) {

    MeshLoader_Result   result;
    MeshLoader_uint32   pausedJobCount = 0U;

    result = __MeshLoader_Mutex_lock (instance->dispatcher.lock);
    if (result != MeshLoader_Result_Success) {
        return result;
    }

    for (MeshLoader_uint32 jobIndex = 0U; jobIndex < pPauseInfo->jobCount; ++ jobIndex) {

        MeshLoader_Job job = pPauseInfo->pJobs [jobIndex];

        switch ((MeshLoader_JobState) atomic_load_explicit (& job->context.jobState, memory_order_acquire)) {
            case MeshLoader_JobState_Paused:
                pausedJobCount ++;
            case MeshLoader_JobState_Ready:
            case MeshLoader_JobState_Stopped:
            case MeshLoader_JobState_Terminated:
            case MeshLoader_JobState_Finished:
            case MeshLoader_JobState_FinishedError:
                continue;
            case MeshLoader_JobState_Running:
                break;
        }

        __MeshLoader_Job_ChangeRequestType previousRequestedChange = (__MeshLoader_Job_ChangeRequestType) atomic_exchange_explicit (
                & job->context.requestedChange,
                __MeshLoader_Job_ChangeRequestType_Pause,
                memory_order_release
        );

        if (
                previousRequestedChange == __MeshLoader_Job_ChangeRequestType_None ||
                previousRequestedChange == __MeshLoader_Job_ChangeRequestType_Pause
        ) {
            ++ pausedJobCount;
        }
    }

    __MeshLoader_Mutex_unlock (instance->dispatcher.lock);
    if (pausedJobCount != pPauseInfo->jobCount) {
        return MeshLoader_Result_PartialSuccess;
    }

    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_resumeJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsResumeInfo       const * pResumeInfo
) {

    MeshLoader_Result   result;
    MeshLoader_uint32   resumedJobCount = 0U;

    result = __MeshLoader_Mutex_lock (instance->dispatcher.lock);
    if (result != MeshLoader_Result_Success) {
        return result;
    }

    for (MeshLoader_uint32 jobIndex = 0U; jobIndex < pResumeInfo->jobCount; ++ jobIndex) {

        MeshLoader_Job job = pResumeInfo->pJobs [jobIndex];

        switch ((MeshLoader_JobState) atomic_load_explicit (& job->context.jobState, memory_order_acquire)) {
            case MeshLoader_JobState_Running:
                resumedJobCount ++;
            case MeshLoader_JobState_Ready:
            case MeshLoader_JobState_Stopped:
            case MeshLoader_JobState_Terminated:
            case MeshLoader_JobState_Finished:
            case MeshLoader_JobState_FinishedError:
                continue;
            case MeshLoader_JobState_Paused:
                break;
        }

        __MeshLoader_Job_ChangeRequestType previousRequestedChange = (__MeshLoader_Job_ChangeRequestType) atomic_exchange_explicit (
                & job->context.requestedChange,
                __MeshLoader_Job_ChangeRequestType_Resume,
                memory_order_release
        );

        if (
                previousRequestedChange == __MeshLoader_Job_ChangeRequestType_None ||
                previousRequestedChange == __MeshLoader_Job_ChangeRequestType_Resume
        ) {
            ++ resumedJobCount;
        }
    }

    __MeshLoader_Mutex_unlock (instance->dispatcher.lock);
    if (resumedJobCount != pResumeInfo->jobCount) {
        return MeshLoader_Result_PartialSuccess;
    }

    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_queryJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsQueryInfo              * pQueryInfo
) {

    MeshLoader_Result result;

    for ( MeshLoader_uint32 jobIndex = 0U; jobIndex < pQueryInfo->jobCount; ++ jobIndex ) {

        result = __MeshLoader_Job_getProgress (
                & pQueryInfo->pQueryJobInfos [ jobIndex ].job->context,
                & pQueryInfo->pQueryJobInfos [ jobIndex ].progress
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }

        result = __MeshLoader_Job_getState (
                & pQueryInfo->pQueryJobInfos [ jobIndex ].job->context,
                & pQueryInfo->pQueryJobInfos [ jobIndex ].state
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }
    }

    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_anyJobsRunning (
        MeshLoader_Instance                     instance,
        MeshLoader_bool                       * pAnyRunning
) {

    return __MeshLoader_JobWorker_Manager_anyWorkersRunning (
            & instance->workerManager,
            pAnyRunning
    );
}

MeshLoader_Result __MeshLoader_JobDispatcher_acquireJob (
        __MeshLoader_JobDispatcher              * pDispatcher,
        __MeshLoader_JobDispatcher_ContextNode ** ppContextNode,
        __MeshLoader_Job_RuntimeContext        ** ppRuntimeContext,
        float                                   * pContextPriority
) {

    MeshLoader_Result                           result;
    __MeshLoader_JobPriorityQueue_Entry const * pEntry;

    __MeshLoader_JobDispatcher_ContextNode    * pHighestPriorityContext         = NULL;

    * ppRuntimeContext = NULL;

    result = __MeshLoader_Mutex_lock (
            pDispatcher->lock
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    /* TODO: seek in paused jobs for any to be resumed */
    * ppContextNode     = pDispatcher->contextList;

    while ( ( * ppContextNode ) != NULL ) {

        if ( __MeshLoader_JobPriorityQueue_empty ( & ( * ppContextNode )->context.jobQueue ) ) {
            ( * ppContextNode ) = ( * ppContextNode )->pNext;
            continue;
        }

        result = __MeshLoader_JobPriorityQueue_peek (
                & ( * ppContextNode )->context.jobQueue,
                & pEntry
        );

        if ( result != MeshLoader_Result_Success ) {
            __MeshLoader_Mutex_unlock (
                    pDispatcher->lock
            );

            return result;
        }

        /* TODO: Loop while jobs are paused, move to inactive queue */
        if ( pHighestPriorityContext == NULL || * pContextPriority < pEntry->priority ) {
            pHighestPriorityContext = * ppContextNode;
            * pContextPriority      = pEntry->priority;
        }

        ( * ppContextNode ) = ( * ppContextNode )->pNext;
    }

    /* TODO: if both from active and inactive are null */
    if ( pHighestPriorityContext == NULL ) {
        __MeshLoader_Mutex_unlock (
                pDispatcher->lock
        );

        return MeshLoader_Result_Success;
    }

    /* TODO: if inactive null */
    result = __MeshLoader_JobPriorityQueue_pop (
            & pHighestPriorityContext->context.jobQueue,
            ppRuntimeContext
    );

    if ( result != MeshLoader_Result_Success ) {
        * ppRuntimeContext = NULL;

        __MeshLoader_Mutex_unlock (
                pDispatcher->lock
        );

        return result;
    }

    * ppContextNode     = pHighestPriorityContext;

    if ( ( * ppRuntimeContext )->jobState == ( MeshLoader_uint8 ) MeshLoader_JobState_Ready ) {
        ( * ppRuntimeContext )->jobState = ( MeshLoader_uint8 ) MeshLoader_JobState_Running;
    }

    /* TODO: if active null */
    /* TODO: else, compare priorities */
    __MeshLoader_Mutex_unlock (
            pDispatcher->lock
    );

    return MeshLoader_Result_Success;
}

MeshLoader_Result __MeshLoader_JobDispatcher_releaseJob (
        __MeshLoader_JobDispatcher              * pDispatcher,
        __MeshLoader_JobDispatcher_ContextNode  * pContextNode,
        __MeshLoader_Job_RuntimeContext         * pRuntimeContext,
        float                                     contextPriority
) {

    MeshLoader_Result result;

    result = __MeshLoader_Mutex_lock (
            pDispatcher->lock
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    if (
            pRuntimeContext->jobState == ( MeshLoader_uint8 ) MeshLoader_JobState_Finished ||
            pRuntimeContext->jobState == ( MeshLoader_uint8 ) MeshLoader_JobState_FinishedError
    ) {
        pContextNode->context.finishedJobCount ++;

        __MeshLoader_JobMemoryAllocator_destruct (
                & pRuntimeContext->jobMemoryAllocator,
                & pContextNode->context.allocationCallbacks
        );

        if ( pContextNode->context.finishedJobCount == pContextNode->context.jobCount ) {
            __MeshLoader_JobDispatcher_removeContextNode (
                    pDispatcher,
                    pContextNode
            );
        }

        __MeshLoader_Mutex_unlock (
                pDispatcher->lock
        );

        return result;
    }

    /* TODO: identify whether job is to be paused or stopped, then insert to separate queue */
    result = __MeshLoader_JobPriorityQueue_push (
            & pContextNode->context.jobQueue,
            pRuntimeContext,
            contextPriority
    );

    if ( result != MeshLoader_Result_Success ) {
        __MeshLoader_Mutex_unlock (
                pDispatcher->lock
        );

        return result;
    }

    __MeshLoader_Mutex_unlock (
            pDispatcher->lock
    );

    return MeshLoader_Result_Success;
}

static void __MeshLoader_JobDispatcher_removeContextNode (
        __MeshLoader_JobDispatcher              * pDispatcher,
        __MeshLoader_JobDispatcher_ContextNode  * pContextNode
) {

    __MeshLoader_JobDispatcher_ContextNode * pToDelete;

    if ( pContextNode == pDispatcher->contextList ) {
        pToDelete                   = pDispatcher->contextList;
        pDispatcher->contextList    = pDispatcher->contextList->pNext;

        __MeshLoader_JobDispatcher_freeContext ( pToDelete );

        return;
    }

    __MeshLoader_JobDispatcher_ContextNode * pHead = pDispatcher->contextList;

    while ( pHead != NULL && pHead->pNext != NULL ) {

        if ( pHead->pNext == pContextNode ) {

            pToDelete       = pHead->pNext;
            pHead->pNext    = pHead->pNext->pNext;

            __MeshLoader_JobDispatcher_freeContext ( pToDelete );

            return;
        }

        pHead = pHead->pNext;
    }
}
