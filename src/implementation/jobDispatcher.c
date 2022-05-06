//
// Created by loghin on 5/6/22.
//

#include <stdalign.h>
#include "jobDispatcher.h"
#include "instance.h"
#include "job.h"
#include <memory.h>

MeshLoader_Result __MeshLoader_JobDispatcher_construct (
        __MeshLoader_JobDispatcher            * pDispatcher,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    (void) pAllocationCallbacks;

    pDispatcher->contextList = NULL;

    return MeshLoader_Result_Success;
}

void __MeshLoader_JobDispatcher_destruct (
        __MeshLoader_JobDispatcher            * pDispatcher,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    (void) pAllocationCallbacks;

    while ( pDispatcher->contextList != NULL ) {
        __MeshLoader_JobDispatcher_ContextNode * pToDelete  = pDispatcher->contextList;
        pDispatcher->contextList                            = pDispatcher->contextList->pNext;
        __MeshLoader_JobDispatcher_freeContext ( pToDelete );
    }
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

    __MeshLoader_JobPriorityQueue_destruct (
            & pNode->context.jobQueue,
            & scopedAllocationCallbacks
    );

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

    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_queryJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsQueryInfo              * pQueryInfo
) {
    (void) instance;
    (void) pQueryInfo;

    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_getResults (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsGetResultInfo          * pResults
) {
    (void) instance;
    (void) pResults;

    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_anyJobsRunning (
        MeshLoader_Instance                     instance,
        MeshLoader_bool                       * pAnyRunning
) {

    (void) instance;
    (void) pAnyRunning;

    return MeshLoader_Result_Success;
}