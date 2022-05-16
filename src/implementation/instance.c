//
// Created by loghin on 5/4/22.
//

#include <meshLoader/publicTypes>
#include <stdalign.h>
#include "privateUtility.h"
#include "../config/instanceCnf.h"
#include "instance.h"

static __MeshLoader_Instance_Control __MeshLoader_Instance_control;

MeshLoader_Result MeshLoader_createInstance (
        MeshLoader_InstanceCreateInfo   const * pCreateInfo,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks,
        MeshLoader_Instance                   * pInstance
) {
    MeshLoader_Instance          newInstance;
    MeshLoader_Result            result;

    pAllocationCallbacks = __MeshLoader_Utility_nonNullAllocationCallbacks ( pAllocationCallbacks );

    if ( __MeshLoader_Instance_noInstanceExists ( & __MeshLoader_Instance_control ) ) {
        result = __MeshLoader_Instance_createControl (
                & __MeshLoader_Instance_control,
                pAllocationCallbacks
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }
    }

    result = __MeshLoader_Instance_allocateInstance (
            & __MeshLoader_Instance_control,
            & newInstance,
            pAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        __MeshLoader_Instance_freeInstance (
                & __MeshLoader_Instance_control,
                newInstance,
                pAllocationCallbacks
        );

        if ( ! __MeshLoader_Instance_noInstanceExists ( & __MeshLoader_Instance_control ) ) {
            __MeshLoader_Instance_destroyControl ( &
                    __MeshLoader_Instance_control,
                    pAllocationCallbacks
            );
        }

        return result;
    }

    result = __MeshLoader_Instance_construct (
            newInstance,
            pCreateInfo,
            pAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        __MeshLoader_Instance_freeInstance (
                & __MeshLoader_Instance_control,
                newInstance,
                pAllocationCallbacks
        );

        if ( ! __MeshLoader_Instance_noInstanceExists ( & __MeshLoader_Instance_control ) ) {
            __MeshLoader_Instance_destroyControl (
                    & __MeshLoader_Instance_control,
                    pAllocationCallbacks
            );
        }

        return result;
    }

    * pInstance = newInstance;

    return MeshLoader_Result_Success;
}

void MeshLoader_destroyInstance (
        MeshLoader_Instance                     instance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    if ( instance == MeshLoader_invalidHandle ) {
        return;
    }

    pAllocationCallbacks = __MeshLoader_Utility_nonNullAllocationCallbacks ( pAllocationCallbacks );

    __MeshLoader_Instance_destruct (
            instance,
            pAllocationCallbacks
    );

    __MeshLoader_Instance_freeInstance (
            & __MeshLoader_Instance_control,
            instance,
            pAllocationCallbacks
    );

    if ( __MeshLoader_Instance_noInstanceExists ( & __MeshLoader_Instance_control ) ) {
        __MeshLoader_Instance_destroyControl (
                & __MeshLoader_Instance_control,
                pAllocationCallbacks
        );
    }
}

static MeshLoader_Result  __MeshLoader_Instance_construct (
        MeshLoader_Instance                     pInstance,
        MeshLoader_InstanceCreateInfo   const * pCreateInfo,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    MeshLoader_Result result;
    __MeshLoader_ScopedAllocationCallbacks scopedAllocationCallbacks = {
            .pAllocationCallbacks       = pAllocationCallbacks,
            .allocationScope            = MeshLoader_SystemAllocationScope_Instance,
            .explicitAllocationPurpose  = NULL
    };

    result = __MeshLoader_Mutex_create (
            & pInstance->instanceLock,
            & scopedAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    result = __MeshLoader_JobDispatcher_construct (
            & pInstance->dispatcher,
            pAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        __MeshLoader_Mutex_destroy (
                pInstance->instanceLock,
                & scopedAllocationCallbacks
        );

        return result;
    }

    result = __MeshLoader_JobWorker_Manager_construct (
            pInstance,
            & pInstance->workerManager,
            pCreateInfo->maxWorkerThreadCount,
            pAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {

        __MeshLoader_JobDispatcher_destruct (
                & pInstance->dispatcher,
                pAllocationCallbacks
        );

        __MeshLoader_Mutex_destroy (
                pInstance->instanceLock,
                & scopedAllocationCallbacks
        );

        return result;
    }

    pInstance->jobList                  = NULL;

#if MESH_LOADER_DEBUG_MODE

    pInstance->totalJobCount            = 0U;
    pInstance->readyJobCount            = 0U;
    pInstance->runningJobCount          = 0U;
    pInstance->stoppedJobCount          = 0U;
    pInstance->finishedJobCount         = 0U;
    pInstance->finishedErrorJobCount    = 0U;

#endif

    return MeshLoader_Result_Success;
}

static void __MeshLoader_Instance_destruct (
        MeshLoader_Instance                     pInstance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    __MeshLoader_ScopedAllocationCallbacks scopedAllocationCallbacks = {
            .pAllocationCallbacks       = pAllocationCallbacks,
            .allocationScope            = MeshLoader_SystemAllocationScope_Instance,
            .explicitAllocationPurpose  = NULL
    };

    __MeshLoader_Instance_freeAllJobs (
            pInstance,
            pAllocationCallbacks
    );

    __MeshLoader_JobWorker_Manager_destruct (
            & pInstance->workerManager,
            pAllocationCallbacks
    );

    __MeshLoader_JobDispatcher_destruct (
            & pInstance->dispatcher,
            pAllocationCallbacks
    );

    __MeshLoader_Mutex_destroy (
            pInstance->instanceLock,
            & scopedAllocationCallbacks
    );
}

static MeshLoader_Result __MeshLoader_Instance_allocateInstance (
        __MeshLoader_Instance_Control         * pControl,
        MeshLoader_Instance                   * pInstance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    __MeshLoader_Instance_Node    * pNewNode;
    MeshLoader_Result               result;

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = sizeof ( __MeshLoader_Instance_Node ),
            .alignment              = alignof ( __MeshLoader_Instance_Node ),
            .allocationScope        = MeshLoader_SystemAllocationScope_Component,
            .explicitMemoryPurpose  = "Creates an Instance Context Location, owned by the MeshLoader Component"
    };

    allocationNotification.pMemory = ( __MeshLoader_Instance_Node * ) pAllocationCallbacks->allocationFunction (
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

    result = __MeshLoader_Mutex_applyModuleLock();
    if ( result != MeshLoader_Result_Success ) {

        allocationNotification.explicitMemoryPurpose    = "Deletes a Node that could not be added due to failure of acquiring the module lock";

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

    pNewNode = ( __MeshLoader_Instance_Node * ) allocationNotification.pMemory;
    * pInstance = & pNewNode->instanceData;
    pNewNode->pNextInstanceNode = pControl->pInstanceList;
    pControl->pInstanceList = pNewNode;

    __MeshLoader_Mutex_removeModuleLock();
    return MeshLoader_Result_Success;
}

static void __MeshLoader_Instance_freeInstance (
        __MeshLoader_Instance_Control           * pControl,
        MeshLoader_Instance                       pInstance,
        MeshLoader_AllocationCallbacks    const * pAllocationCallbacks
) {

    __MeshLoader_Instance_Node * pHead;
    __MeshLoader_Instance_Node * pDeletionCopy;

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = sizeof ( __MeshLoader_Instance_Node ),
            .alignment              = alignof ( __MeshLoader_Instance_Node ),
            .allocationScope        = MeshLoader_SystemAllocationScope_Component,
            .explicitMemoryPurpose  = "Destroys an Instance Context Location, owned by the MeshLoader Component"
    };

    __MeshLoader_Mutex_applyModuleLock();

    if ( pControl->pInstanceList->pNextInstanceNode == NULL || & pControl->pInstanceList->instanceData == pInstance ) {

        allocationNotification.pMemory = pControl->pInstanceList;
        if ( pAllocationCallbacks->internalFreeNotificationFunction != NULL ) {
            pAllocationCallbacks->internalFreeNotificationFunction (
                    pAllocationCallbacks->pUserData,
                    & allocationNotification
            );
        }

        pDeletionCopy = pControl->pInstanceList;
        pControl->pInstanceList = pControl->pInstanceList->pNextInstanceNode;

        pAllocationCallbacks->freeFunction (
                pAllocationCallbacks->pUserData,
                pDeletionCopy
        );

        __MeshLoader_Mutex_removeModuleLock();
        return;
    }

    pHead = pControl->pInstanceList;

    while ( pHead->pNextInstanceNode != NULL ) {

        if ( pInstance == & pHead->pNextInstanceNode->instanceData ) {

            allocationNotification.pMemory = pHead->pNextInstanceNode;
            if ( pAllocationCallbacks->internalFreeNotificationFunction != NULL ) {
                pAllocationCallbacks->internalFreeNotificationFunction (
                        pAllocationCallbacks->pUserData,
                        & allocationNotification
                );
            }

            pDeletionCopy = pHead->pNextInstanceNode;
            pHead->pNextInstanceNode = pHead->pNextInstanceNode->pNextInstanceNode;

            pAllocationCallbacks->freeFunction (
                    pAllocationCallbacks->pUserData,
                    pDeletionCopy
            );

            __MeshLoader_Mutex_removeModuleLock();
            return;
        }

        pHead = pHead->pNextInstanceNode;
    }
}

MeshLoader_Result MeshLoader_createJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsCreateInfo       const * pCreateInfo,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    MeshLoader_Result result;

    pAllocationCallbacks = __MeshLoader_Utility_nonNullAllocationCallbacks ( pAllocationCallbacks );

    result = __MeshLoader_Instance_allocateJobs (
            instance,
            pCreateInfo->jobCount,
            & pCreateInfo->pJobs[0],
            pAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    for ( MeshLoader_uint32 jobIndex = 0U; jobIndex < pCreateInfo->jobCount; ++ jobIndex ) {
        result = __MeshLoader_Job_construct (
                pCreateInfo->pJobs [ jobIndex ],
                & pCreateInfo->pCreateJobInfos [ jobIndex ],
                pAllocationCallbacks
        );

        if ( result != MeshLoader_Result_Success ) {
            if ( ( pCreateInfo->flags & MeshLoader_JobsCreateFlag_ContinueIfError ) == 0U ) {
                __MeshLoader_Instance_freeJobs (
                        instance,
                        pCreateInfo->jobCount,
                        & pCreateInfo->pJobs[0],
                        pAllocationCallbacks
                );

                return result;
            } else {
                /** TODO : implement for ContinueIfError */
            }
        }
    }

#if MESH_LOADER_DEBUG_MODE

    instance->readyJobCount  = pCreateInfo->jobCount;
    instance->totalJobCount += instance->readyJobCount;

#endif

    return MeshLoader_Result_Success;
}

extern void MeshLoader_destroyJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_uint32                       jobCount,
        MeshLoader_Job                  const * pJobs,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    pAllocationCallbacks = __MeshLoader_Utility_nonNullAllocationCallbacks ( pAllocationCallbacks );

    for ( MeshLoader_uint32 jobIndex = 0U; jobIndex < jobCount; ++ jobIndex ) {

        /** TODO : implement for jobCounts */

        __MeshLoader_Job_destruct ( pJobs [ jobIndex ], pAllocationCallbacks );
    }

    __MeshLoader_Instance_freeJobs (
            instance,
            jobCount,
            & pJobs[0],
            pAllocationCallbacks
    );

#if MESH_LOADER_DEBUG_MODE

    instance->totalJobCount -= jobCount;

#endif

}

static MeshLoader_Result __MeshLoader_Instance_allocateJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_uint32                       jobCount,
        MeshLoader_Job                        * pJobs,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_uint32 allocatedJobCount = 0U;
    MeshLoader_Result result;

    result = __MeshLoader_Mutex_lock ( instance->instanceLock );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    result = __MeshLoader_Instance_allocateJobsFillExisting (
            instance,
            jobCount,
            & pJobs[0],
            & allocatedJobCount
    );

    if ( result != MeshLoader_Result_Success || allocatedJobCount == jobCount ) {
        __MeshLoader_Mutex_unlock ( instance->instanceLock );
        return result;
    }

    result = __MeshLoader_Instance_allocateJobsNewNodes (
            instance,
            jobCount,
            & pJobs[0],
            pAllocationCallbacks,
            allocatedJobCount
    );

    if ( result != MeshLoader_Result_Success ) {
        __MeshLoader_Instance_freeJobs (
                instance,
                allocatedJobCount,
                & pJobs[0],
                pAllocationCallbacks
        );

        for ( MeshLoader_uint32 jobIndex = 0U; jobIndex < jobCount; ++ jobIndex ) {
            pJobs [ jobIndex ] = MeshLoader_invalidHandle;
        }

        __MeshLoader_Mutex_unlock ( instance->instanceLock );
        return result;
    }

    __MeshLoader_Mutex_unlock ( instance->instanceLock );
    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Instance_allocateJobsFillExisting (
        MeshLoader_Instance                     instance,
        MeshLoader_uint32                       jobCount,
        MeshLoader_Job                        * pJobs,
        MeshLoader_uint32                     * pAllocatedJobCount
) {

    __MeshLoader_Instance_JobNode * pHead           = instance->jobList;

    MeshLoader_uint32         const usageFlagMax    = 1U << 31U;
    MeshLoader_uint32               usageIndex;

    * pAllocatedJobCount = 0U;

    while ( pHead != NULL ) {

        usageIndex = 0U;
        for ( MeshLoader_uint32 usageFlag = 1U << 0U; MeshLoader_true; usageFlag <<= 1U, ++ usageIndex ) {

            if ( * pAllocatedJobCount == jobCount ) {
                return MeshLoader_Result_Success;
            }

            if ( ( pHead->nodeUsage & usageFlag ) == 0U ) {
                pJobs [ ( * pAllocatedJobCount ) ++ ] = & pHead->jobs [ usageIndex ];
                pHead->nodeUsage |= usageFlag;
            }

            if ( usageFlag == usageFlagMax ) {
                break;
            }
        }

        pHead = pHead->pNextJobNode;
    }

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Instance_allocateJobsNewNodes (
        MeshLoader_Instance                     instance,
        MeshLoader_uint32                       jobCount,
        MeshLoader_Job                        * pJobs,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks,
        MeshLoader_uint32                       offset
) {
    MeshLoader_Result                       result;
    __MeshLoader_Instance_JobNode   const * pOriginalHead = instance->jobList;
    __MeshLoader_Instance_JobNode         * pNewNode;

    MeshLoader_uint32                 const usageFlagMax = 1U << 31U;
    MeshLoader_uint32                       usageIndex;

    while ( offset < jobCount ) {

        result = __MeshLoader_Instance_allocateJobNode (
                instance,
                & pNewNode,
                pAllocationCallbacks
        );

        if ( result != MeshLoader_Result_Success ) {
            __MeshLoader_Instance_cleanupAllocatedJobNodes (
                    instance,
                    pOriginalHead,
                    pAllocationCallbacks
            );

            return result;
        }

        usageIndex = 0U;
        for ( MeshLoader_uint32 usageFlag = 1U << 0U; MeshLoader_true; usageFlag <<= 1U, ++ usageIndex ) {

            if ( offset == jobCount ) {
                return MeshLoader_Result_Success;
            }

            pJobs [ offset ++ ] = & pNewNode->jobs [ usageIndex ];
            pNewNode->nodeUsage |= usageFlag;

            if ( usageFlag == usageFlagMax ) {
                break;
            }
        }
    }

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Instance_allocateJobNode (
        MeshLoader_Instance                     instance,
        __MeshLoader_Instance_JobNode        ** pNewNode,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
        .structureType          = MeshLoader_StructureType_AllocationNotification,
        .pNext                  = NULL,
        .pMemory                = NULL,
        .pOldMemory             = NULL,
        .size                   = sizeof ( __MeshLoader_Instance_JobNode ),
        .alignment              = alignof ( __MeshLoader_Instance_JobNode ),
        .allocationScope        = MeshLoader_SystemAllocationScope_Instance,
        .explicitMemoryPurpose  = "Creates a new Job Node Context to hold 32 Jobs"
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

    * pNewNode = ( __MeshLoader_Instance_JobNode * ) allocationNotification.pMemory;
    ( * pNewNode )->pNextJobNode = instance->jobList;
    instance->jobList = * pNewNode;

    ( * pNewNode )->nodeUsage = MeshLoader_nullFlags;

    return MeshLoader_Result_Success;
}

static void __MeshLoader_Instance_cleanupAllocatedJobNodes (
        MeshLoader_Instance                     instance,
        __MeshLoader_Instance_JobNode   const * pOriginalHead,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    __MeshLoader_Instance_JobNode     * pCopyToDelete;
    MeshLoader_AllocationNotification   allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = sizeof ( __MeshLoader_Instance_JobNode ),
            .alignment              = alignof ( __MeshLoader_Instance_JobNode ),
            .allocationScope        = MeshLoader_SystemAllocationScope_Instance,
            .explicitMemoryPurpose  = "Destroys a new Job Node Context to hold 32 Jobs"
    };

    while ( instance->jobList != pOriginalHead ) {

        pCopyToDelete = instance->jobList;
        instance->jobList = instance->jobList->pNextJobNode;

        allocationNotification.pMemory = pCopyToDelete;

        if ( pAllocationCallbacks->internalFreeNotificationFunction != NULL ) {
            pAllocationCallbacks->internalFreeNotificationFunction (
                    pAllocationCallbacks->pUserData,
                    & allocationNotification
            );
        }

        pAllocationCallbacks->freeFunction (
                pAllocationCallbacks->pUserData,
                pCopyToDelete
        );
    }
}

static void __MeshLoader_Instance_freeJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_uint32                       jobCount,
        MeshLoader_Job                  const * pJobs,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    __MeshLoader_Instance_JobNode * pHead;

    (void) __MeshLoader_Mutex_lock ( instance->instanceLock );

    __MeshLoader_Instance_freeJobsInFirstNode (
            instance,
            jobCount,
            & pJobs[0],
            pAllocationCallbacks
    );

    pHead = instance->jobList;

    while ( pHead != NULL && pHead->pNextJobNode != NULL ) {

        __MeshLoader_Instance_freeJobsNextInNode (
                pHead,
                jobCount,
                & pJobs[0],
                pAllocationCallbacks
        );
    }

    __MeshLoader_Mutex_unlock ( instance->instanceLock );
}

static void __MeshLoader_Instance_freeJobsInFirstNode (
        MeshLoader_Instance                     instance,
        MeshLoader_uint32                       jobCount,
        MeshLoader_Job                  const * pJobs,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_uint32               const usageFlagMax              = 1U << 31U;

    while ( instance->jobList != NULL ) {

        for ( MeshLoader_uint32 usageFlag = 1U << 0U, usageIndex = 0U; MeshLoader_true; usageFlag <<= 1U, ++ usageIndex ) {

            for ( MeshLoader_uint32 jobIndex = 0U; jobIndex < jobCount; ++ jobIndex ) {
                if ( & instance->jobList->jobs [ usageIndex ] == pJobs [ jobIndex ] ) {
                    instance->jobList->nodeUsage &= ~ usageFlag;
                    break;
                }
            }

            if ( usageFlag == usageFlagMax ) {
                break;
            }
        }

        if ( instance->jobList->nodeUsage == 0U ) {
            __MeshLoader_Instance_freeFirstJobNode (
                    instance,
                    pAllocationCallbacks
            );
        } else {
            break;
        }
    }
}

static void __MeshLoader_Instance_freeFirstJobNode (
        MeshLoader_Instance                     instance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    __MeshLoader_Instance_JobNode * pHeadCopy = instance->jobList;

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = pHeadCopy,
            .pOldMemory             = NULL,
            .size                   = sizeof ( __MeshLoader_Instance_Node ),
            .alignment              = alignof ( __MeshLoader_Instance_Node ),
            .allocationScope        = MeshLoader_SystemAllocationScope_Instance,
            .explicitMemoryPurpose  = "Deletes the first node holding 32 jobs"
    };

    instance->jobList = instance->jobList->pNextJobNode;

    if ( pAllocationCallbacks->internalFreeNotificationFunction != NULL ) {
        pAllocationCallbacks->internalFreeNotificationFunction (
                pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    pAllocationCallbacks->freeFunction (
            pAllocationCallbacks->pUserData,
            pHeadCopy
    );
}

static void __MeshLoader_Instance_freeJobsNextInNode (
        __MeshLoader_Instance_JobNode         * pPrevious,
        MeshLoader_uint32                       jobCount,
        MeshLoader_Job                  const * pJobs,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    __MeshLoader_Instance_JobNode       * pNode                     = pPrevious->pNextJobNode;
    MeshLoader_uint32               const usageFlagMax              = 1U << 31U;

    for ( MeshLoader_uint32 usageFlag = 1U << 0U, usageIndex = 0U; MeshLoader_true; usageFlag <<= 1U, ++ usageIndex ) {

        for ( MeshLoader_uint32 jobIndex = 0U; jobIndex < jobCount; ++ jobIndex ) {
            if ( & pNode->jobs [ usageIndex ] == pJobs [ jobIndex ] ) {
                pNode->nodeUsage &= ~ usageFlag;
                break;
            }
        }

        if ( usageFlag == usageFlagMax ) {
            break;
        }
    }

    if ( pNode->nodeUsage == 0U ) {
        __MeshLoader_Instance_freeNextJobNode (
                pPrevious,
                pAllocationCallbacks
        );
    }
}

static void __MeshLoader_Instance_freeNextJobNode (
        __MeshLoader_Instance_JobNode         * pPrevious,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    __MeshLoader_Instance_JobNode       * pNode                     = pPrevious->pNextJobNode;

    MeshLoader_AllocationNotification     allocationNotification    = {
            .structureType              = MeshLoader_StructureType_AllocationNotification,
            .pNext                      = NULL,
            .pMemory                    = pNode,
            .pOldMemory                 = NULL,
            .size                       = sizeof ( __MeshLoader_Instance_JobNode ),
            .alignment                  = alignof ( __MeshLoader_Instance_JobNode ),
            .allocationScope            = MeshLoader_SystemAllocationScope_Instance,
            .explicitMemoryPurpose      = "Deletes a Job Node holding 32 jobs"
    };

    if ( pAllocationCallbacks->internalFreeNotificationFunction != NULL ) {
        pAllocationCallbacks->internalFreeNotificationFunction (
                pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    pPrevious->pNextJobNode = pNode->pNextJobNode;

    pAllocationCallbacks->freeFunction (
            pAllocationCallbacks->pUserData,
            pNode
    );
}

static MeshLoader_Result __MeshLoader_Instance_createControl (
        __MeshLoader_Instance_Control         * pControl,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    (void) pAllocationCallbacks;

    pControl->pInstanceList = NULL;
    return MeshLoader_Result_Success;
}

static void __MeshLoader_Instance_destroyControl (
        __MeshLoader_Instance_Control   const * pControl,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    (void) pControl;
    (void) pAllocationCallbacks;

    __MeshLoader_Mutex_clearModuleLock();
    __MeshLoader_InternalAllocation_clear ();
}

static void __MeshLoader_Instance_freeAllJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    MeshLoader_AllocationNotification allocationNotification = {
            .structureType  = MeshLoader_StructureType_AllocationNotification,
            .pNext          = NULL,
            .pMemory        = NULL,
            .pOldMemory     = NULL,
            .size           = sizeof ( __MeshLoader_Instance_JobNode ),
            .alignment      = alignof ( __MeshLoader_Instance_JobNode ),
            .allocationScope    = MeshLoader_SystemAllocationScope_Instance,
            .explicitMemoryPurpose  = "Clears all Jobs in a Job Node at an Instance's Destruction"
    };

    while ( instance->jobList != NULL ) {

        __MeshLoader_Instance_freeAllJobsInNode (
                instance->jobList,
                pAllocationCallbacks
        );

        allocationNotification.pMemory = instance->jobList;
        instance->jobList = instance->jobList->pNextJobNode;

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
}

static void __MeshLoader_Instance_freeAllJobsInNode (
        __MeshLoader_Instance_JobNode         * pNode,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_uint32                   const flagValueMax              = 1U << 31U;
    MeshLoader_AllocationNotification         allocationNotification    = {
            .structureType                  = MeshLoader_StructureType_AllocationNotification,
            .pNext                          = NULL,
            .pMemory                        = pNode,
            .pOldMemory                     = NULL,
            .size                           = sizeof ( __MeshLoader_Instance_JobNode ),
            .alignment                      = alignof ( __MeshLoader_Instance_JobNode ),
            .allocationScope                = MeshLoader_SystemAllocationScope_Instance,
            .explicitMemoryPurpose          = "Destroys a Job Node holding 32 Job Instances"
    };

    for ( MeshLoader_uint32 flagValue = 1U << 0U, flagIndex = 0U; MeshLoader_true; flagValue <<= 1U, ++ flagIndex ) {

        if ( ( pNode->nodeUsage & flagValue ) != 0U ) {

            __MeshLoader_Job_destruct (
                    & pNode->jobs [ flagIndex ],
                    pAllocationCallbacks
            );
        }

        if ( flagValue == flagValueMax ) {
            break;
        }
    }

    if ( pAllocationCallbacks->internalFreeNotificationFunction != NULL ) {
        pAllocationCallbacks->internalFreeNotificationFunction (
                pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }
}
