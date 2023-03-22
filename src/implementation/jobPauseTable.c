//
// Created by loghin on 3/22/2023.
//

#include "jobPauseTable.h"
#include <stdalign.h>
#include <string.h>

#define __MeshLoader_JobPauseTable_MaxGrowthIndex   28U
#define __MeshLoader_JobPauseTable_LoadFactor       1U


static MeshLoader_size const __MeshLoader_JobPauseTable_PrimeRehashPolicyTable [__MeshLoader_JobPauseTable_MaxGrowthIndex + 1U] = {
        13U, 27U, 57U, 117U, 237U,
        477U, 957U, 1917U, 3837U, 7677U,
        15357U, 30717U, 61437U, 122877U,
        245757U, 491517U, 983037U, 1966077U,
        3932157U, 7864317U, 15728637U, 31457277U,
        62914557U, 125829117U, 251658237U,
        503316477U, 1006632957U, 2013265917U,
        4026531837U
};


static MeshLoader_size __MeshLoader_JobPauseTable_getRehashSize (
        MeshLoader_size,
        MeshLoader_size,
        MeshLoader_size,
        MeshLoader_size *
);


static inline MeshLoader_size __MeshLoader_JobPauseTable_hash (
        __MeshLoader_Job_RuntimeContext const * pContext
) {

    return (MeshLoader_size) pContext;
}


static inline __MeshLoader_JobPauseTable_Node * __MeshLoader_JobPauseTable_allocateNode (
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = sizeof (__MeshLoader_JobPauseTable_Node),
            .alignment              = alignof (__MeshLoader_JobPauseTable_Node),
            .allocationScope        = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose  = "Creates a Job Pause Table Node (Hash Table)"
    };

    allocationNotification.pMemory = pAllocationCallbacks->pAllocationCallbacks->allocationFunction (
            pAllocationCallbacks->pAllocationCallbacks->pUserData,
            allocationNotification.size,
            allocationNotification.alignment,
            allocationNotification.allocationScope
    );

    if (allocationNotification.pMemory == NULL) {
        return NULL;
    }

    if (pAllocationCallbacks->pAllocationCallbacks->internalAllocationNotificationFunction != NULL) {
        pAllocationCallbacks->pAllocationCallbacks->internalAllocationNotificationFunction (
                pAllocationCallbacks->pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    return (__MeshLoader_JobPauseTable_Node *) allocationNotification.pMemory;
}


static inline void __MeshLoader_JobPauseTable_freeNode (
        __MeshLoader_JobPauseTable_Node               * pNode,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = pNode,
            .pOldMemory             = NULL,
            .size                   = sizeof (__MeshLoader_JobPauseTable_Node),
            .alignment              = alignof (__MeshLoader_JobPauseTable_Node),
            .allocationScope        = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose  = "Destroys a Job Pause Table Node (Hash Table)"
    };

    if (pAllocationCallbacks->pAllocationCallbacks->internalFreeNotificationFunction != NULL) {
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


static inline MeshLoader_Result __MeshLoader_JobPauseTable_allocateBuckets (
        __MeshLoader_JobPauseTable                    * pTable,
        MeshLoader_size                                 newBucketCount,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = pTable->bucketArray,
            .size                   = sizeof (__MeshLoader_JobPauseTable_List) * newBucketCount,
            .alignment              = alignof (__MeshLoader_JobPauseTable_List),
            .allocationScope        = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose  = "Enlarges the Job Pause Table Bucket List (Hash Table)"
    };

    allocationNotification.pMemory = pAllocationCallbacks->pAllocationCallbacks->reallocationFunction (
            pAllocationCallbacks->pAllocationCallbacks->pUserData,
            allocationNotification.pOldMemory,
            allocationNotification.size,
            allocationNotification.alignment,
            allocationNotification.allocationScope
    );

    if (allocationNotification.pMemory == NULL) {
        return MeshLoader_Result_OutOfMemory;
    }

    if (pAllocationCallbacks->pAllocationCallbacks->internalReallocationNotificationFunction != NULL) {
        pAllocationCallbacks->pAllocationCallbacks->internalReallocationNotificationFunction (
                pAllocationCallbacks->pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    pTable->bucketArray = memset (
            allocationNotification.pMemory + pTable->bucketCount,
            0U,
            sizeof (__MeshLoader_JobPauseTable_List) * (newBucketCount - pTable->bucketCount)
    ) - pTable->bucketCount;

    pTable->bucketCount = newBucketCount;
    return MeshLoader_Result_Success;
}


static inline MeshLoader_Result __MeshLoader_JobPauseTable_rehash (
        __MeshLoader_JobPauseTable                    * pTable,
        MeshLoader_size                                 bucketCount,
        MeshLoader_size                                 newNodeHashValue,
        __MeshLoader_JobPauseTable_Node               * pNewNode,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_size                     oldBucketCount = pTable->bucketCount;
    MeshLoader_size                     newHashValue;
    MeshLoader_size                     newHashedIndex;
    __MeshLoader_JobPauseTable_Node   * pToMove;

    MeshLoader_Result result = __MeshLoader_JobPauseTable_allocateBuckets (
            pTable,
            bucketCount,
            pAllocationCallbacks
    );

    if (result != MeshLoader_Result_Success) {
        return result;
    }

    for (MeshLoader_size bucketIndex = 0ULL; bucketIndex < oldBucketCount; ++ bucketIndex) {

        while (pTable->bucketArray [bucketIndex] != NULL) {
            if (pNewNode == pTable->bucketArray [bucketIndex]) {
                newHashValue = newNodeHashValue;
            } else {
                newHashValue = __MeshLoader_JobPauseTable_hash (pTable->bucketArray [bucketIndex]->pContext);
            }

            newHashedIndex = newHashValue % pTable->bucketCount;
            if (newHashedIndex == bucketIndex) {
                break;
            }

            pToMove                                 = pTable->bucketArray [bucketIndex];
            pTable->bucketArray [bucketIndex]       = pTable->bucketArray [bucketIndex]->pNext;
            pToMove->pNext                          = pTable->bucketArray [newHashedIndex];
            pTable->bucketArray [newHashedIndex]    = pToMove;
        }

        __MeshLoader_JobPauseTable_Node * pHead = pTable->bucketArray [bucketIndex];
        while (pHead != NULL && pHead->pNext != NULL) {
            if (pNewNode == pHead->pNext) {
                newHashValue = newNodeHashValue;
            } else {
                newHashValue = __MeshLoader_JobPauseTable_hash (pHead->pNext->pContext);
            }

            newHashedIndex = newHashValue % pTable->bucketCount;
            if (bucketIndex == newHashedIndex) {
                pHead = pHead->pNext;
                continue;
            }

            pToMove                             = pHead->pNext;
            pHead->pNext                        = pHead->pNext->pNext;
            pToMove->pNext                      = pTable->bucketArray [newHashedIndex];
            pTable->bucketArray[newHashedIndex] = pToMove;
        }
    }

    return MeshLoader_Result_Success;
}


static MeshLoader_size __MeshLoader_JobPauseTable_getRehashSize (
        MeshLoader_size   bucketCount,
        MeshLoader_size   elementCount,
        MeshLoader_size   requiredCount,
        MeshLoader_size * growthIndex
) {

    if ( * growthIndex == __MeshLoader_JobPauseTable_MaxGrowthIndex ) {
        return __MeshLoader_JobPauseTable_PrimeRehashPolicyTable [ * growthIndex ];
    }

    MeshLoader_size const totalRequired        = requiredCount + elementCount;
    MeshLoader_size const loadAdjusted         = bucketCount * __MeshLoader_JobPauseTable_LoadFactor;
    MeshLoader_size const minimumGrowthFactor  = 2ULL;

    if ( loadAdjusted > totalRequired ) {
        return __MeshLoader_JobPauseTable_PrimeRehashPolicyTable [ * growthIndex ];
    }

    if ( totalRequired <= loadAdjusted * minimumGrowthFactor ) {
        return __MeshLoader_JobPauseTable_PrimeRehashPolicyTable [ ++ * growthIndex ];
    }

    while (
            totalRequired < __MeshLoader_JobPauseTable_PrimeRehashPolicyTable [ * growthIndex ] * __MeshLoader_JobPauseTable_LoadFactor &&
            * growthIndex < __MeshLoader_JobPauseTable_MaxGrowthIndex
    ) {
        ++ * growthIndex;
    }

    return __MeshLoader_JobPauseTable_PrimeRehashPolicyTable [ * growthIndex ];
}


MeshLoader_Result __MeshLoader_JobPauseTable_construct (
        __MeshLoader_JobPauseTable                    * pTable,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification   allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = sizeof (__MeshLoader_JobPauseTable_List) * __MeshLoader_JobPauseTable_PrimeRehashPolicyTable [0U],
            .alignment              = alignof (__MeshLoader_JobPauseTable_List),
            .allocationScope        = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose  = "Creates a Job Pause Table Bucket List (Hash Table)"
    };

    allocationNotification.pMemory = pAllocationCallbacks->pAllocationCallbacks->allocationFunction (
            pAllocationCallbacks->pAllocationCallbacks->pUserData,
            allocationNotification.size,
            allocationNotification.alignment,
            allocationNotification.allocationScope
    );

    if (allocationNotification.pMemory == NULL) {
        return MeshLoader_Result_OutOfMemory;
    }

    if (pAllocationCallbacks->pAllocationCallbacks->internalAllocationNotificationFunction != NULL) {
        pAllocationCallbacks->pAllocationCallbacks->internalAllocationNotificationFunction (
                pAllocationCallbacks->pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    pTable->size        = 0U;
    pTable->growthIndex = 0U;
    pTable->bucketCount = __MeshLoader_JobPauseTable_PrimeRehashPolicyTable [pTable->growthIndex];
    pTable->bucketArray = (__MeshLoader_JobPauseTable_BucketArray) allocationNotification.pMemory;

    return MeshLoader_Result_Success;
}


void __MeshLoader_JobPauseTable_destruct (
        __MeshLoader_JobPauseTable              const * pTable,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = sizeof (__MeshLoader_JobPauseTable_Node),
            .alignment              = alignof (__MeshLoader_JobPauseTable_Node),
            .allocationScope        = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose  = "Destroys a Job Pause Table Node (Hash Table)"
    };

    for (MeshLoader_uint32 bucketIndex = 0U; bucketIndex < pTable->bucketCount; ++ bucketIndex) {
        __MeshLoader_JobPauseTable_List currentBucket = pTable->bucketArray [bucketIndex];
        while (currentBucket != NULL) {

            allocationNotification.pMemory  = currentBucket;
            currentBucket                   = currentBucket->pNext;

            if (pAllocationCallbacks->pAllocationCallbacks->internalFreeNotificationFunction != NULL) {
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
    }

    allocationNotification.pMemory                  = pTable->bucketArray;
    allocationNotification.size                     = sizeof (__MeshLoader_JobPauseTable_List) * pTable->bucketCount;
    allocationNotification.alignment                = alignof (__MeshLoader_JobPauseTable_List);
    allocationNotification.explicitMemoryPurpose    = "Destroys a Job Pause Table Bucket List (Hash Table)";

    if (pAllocationCallbacks->pAllocationCallbacks->internalFreeNotificationFunction != NULL) {
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


MeshLoader_Result __MeshLoader_JobPauseTable_emplace (
        __MeshLoader_JobPauseTable                    * pTable,
        __MeshLoader_Job_RuntimeContext               * pRuntimeContext,
        float                                           priority,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_size                     bucketSize      = 0ULL;
    MeshLoader_size                     hashValue       = __MeshLoader_JobPauseTable_hash(pRuntimeContext);
    MeshLoader_size                     hashedIndex     = hashValue % pTable->bucketCount;
    __MeshLoader_JobPauseTable_Node   * pSeekAndNewNode = pTable->bucketArray [hashedIndex];

    while (pSeekAndNewNode != NULL) {
        if (pSeekAndNewNode->pContext == pRuntimeContext) {
            pSeekAndNewNode->priorityInQueue = priority;
            return MeshLoader_Result_PartialSuccess;
        }

        pSeekAndNewNode = pSeekAndNewNode->pNext;
        ++ bucketSize;
    }

    pSeekAndNewNode = __MeshLoader_JobPauseTable_allocateNode (pAllocationCallbacks);
    if (pSeekAndNewNode == NULL) {
        return MeshLoader_Result_OutOfMemory;
    }

    pSeekAndNewNode->pContext           = pRuntimeContext;
    pSeekAndNewNode->priorityInQueue    = priority;
    pSeekAndNewNode->pNext              = pTable->bucketArray [hashedIndex];
    pTable->bucketArray [hashedIndex]   = pSeekAndNewNode;

    ++ pTable->size;

    if (bucketSize >= __MeshLoader_JobPauseTable_LoadFactor) {
        MeshLoader_size newBucketSize = __MeshLoader_JobPauseTable_getRehashSize (
                pTable->bucketCount,
                pTable->size,
                1U,
                & pTable->growthIndex
        );

        if (newBucketSize != pTable->bucketCount) {
             MeshLoader_Result result = __MeshLoader_JobPauseTable_rehash (
                    pTable,
                    newBucketSize,
                    hashValue,
                    pSeekAndNewNode,
                    pAllocationCallbacks
            );

            if (result != MeshLoader_Result_Success) {
                pTable->bucketArray [hashedIndex] = pTable->bucketArray [hashedIndex]->pNext;
                __MeshLoader_JobPauseTable_freeNode (
                        pSeekAndNewNode,
                        pAllocationCallbacks
                );

                return result;
            }
        }
    }

    return MeshLoader_Result_Success;
}

MeshLoader_Result __MeshLoader_JobPauseTable_remove (
        __MeshLoader_JobPauseTable                    * pTable,
        __MeshLoader_Job_RuntimeContext         const * pRuntimeContext,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    if (pTable->size == 0ULL) {
        return MeshLoader_Result_PartialSuccess;
    }

    __MeshLoader_JobPauseTable_Node   * pToRemove;
    __MeshLoader_JobPauseTable_Node   * pHead;
    MeshLoader_size                     hashValue               = __MeshLoader_JobPauseTable_hash (pRuntimeContext);
    MeshLoader_size                     hashedIndex             = hashValue % pTable->bucketCount;

    if (pTable->bucketArray [hashedIndex] == NULL) {
        return MeshLoader_Result_PartialSuccess;
    }

    if (pTable->bucketArray [hashedIndex]->pContext == pRuntimeContext) {
        pToRemove                           = pTable->bucketArray [hashedIndex];
        pTable->bucketArray [hashedIndex]   = pTable->bucketArray [hashedIndex]->pNext;

        __MeshLoader_JobPauseTable_freeNode (pToRemove, pAllocationCallbacks);
        -- pTable->size;

        return MeshLoader_Result_Success;
    }

    pHead = pTable->bucketArray [hashedIndex];
    while (pHead->pNext != NULL) {

        if (pHead->pNext->pContext == pRuntimeContext) {
            pToRemove       = pHead->pNext;
            pHead->pNext    = pHead->pNext->pNext;

            __MeshLoader_JobPauseTable_freeNode (pToRemove, pAllocationCallbacks);
            -- pTable->size;

            return MeshLoader_Result_Success;
        }

        pHead = pHead->pNext;
    }

    return MeshLoader_Result_PartialSuccess;
}

MeshLoader_Result __MeshLoader_JobPauseTable_begin (
        __MeshLoader_JobPauseTable                    * pTable,
        __MeshLoader_JobPauseTable_Iterator           * pIterator
) {

    pIterator->bucketArray      = pTable->bucketArray;
    pIterator->bucketCount      = pTable->bucketCount;
    pIterator->bucketIndex      = 0U;
    pIterator->pCurrentNode     = pTable->bucketArray [0U];
    pIterator->pPreviousNode    = NULL;

    for (MeshLoader_size bucketIndex = 1ULL; bucketIndex < pTable->bucketCount && pIterator->pCurrentNode == NULL; ++ bucketIndex) {
        if (pTable->bucketArray [bucketIndex] != NULL) {
            pIterator->pCurrentNode = pTable->bucketArray [bucketIndex];
            pIterator->bucketIndex  = bucketIndex;
            break;
        }
    }

    return MeshLoader_Result_Success;
}

MeshLoader_Result __MeshLoader_JobPauseTable_end (
        __MeshLoader_JobPauseTable                    * pTable,
        __MeshLoader_JobPauseTable_Iterator           * pIterator
) {

    pIterator->bucketArray      = NULL;
    pIterator->bucketCount      = 0U;
    pIterator->bucketIndex      = 0U;
    pIterator->pCurrentNode     = NULL;
    pIterator->pPreviousNode    = NULL;

    return MeshLoader_Result_Success;
}

MeshLoader_Result __MeshLoader_JobPauseTable_Iterator_get (
        __MeshLoader_JobPauseTable_Iterator     const  * pIterator,
        __MeshLoader_Job_RuntimeContext               ** ppRuntimeContext,
        float                                          * pPriority
) {

    if (pIterator->bucketArray == NULL || pIterator->bucketIndex >= pIterator->bucketCount) {
        return MeshLoader_Result_IllegalArgument;
    }

    * ppRuntimeContext  = pIterator->pCurrentNode->pContext;
    * pPriority         = pIterator->pCurrentNode->priorityInQueue;

    return MeshLoader_Result_Success;
}

MeshLoader_Result __MeshLoader_JobPauseTable_Iterator_next (
        __MeshLoader_JobPauseTable_Iterator               * pIterator
) {

    if (pIterator->bucketArray == NULL || pIterator->bucketIndex >= pIterator->bucketCount) {
        return MeshLoader_Result_IllegalArgument;
    }

    if (pIterator->pCurrentNode->pNext != NULL) {
        pIterator->pPreviousNode    = pIterator->pCurrentNode;
        pIterator->pCurrentNode     = pIterator->pCurrentNode->pNext;
        return MeshLoader_Result_Success;
    }

    for (MeshLoader_size bucketIndex = pIterator->bucketIndex + 1ULL; bucketIndex < pIterator->bucketCount && pIterator->pCurrentNode == NULL; ++ bucketIndex) {
        if (pIterator->bucketArray [bucketIndex] != NULL) {
            pIterator->pCurrentNode     = pIterator->bucketArray [bucketIndex];
            pIterator->pPreviousNode    = NULL;
            pIterator->bucketIndex      = bucketIndex;
            return MeshLoader_Result_Success;
        }
    }

    pIterator->bucketIndex = pIterator->bucketCount;
    return MeshLoader_Result_Success;
}

MeshLoader_bool __MeshLoader_JobPauseTable_Iterator_equal (
        __MeshLoader_JobPauseTable_Iterator     const * pLeft,
        __MeshLoader_JobPauseTable_Iterator     const * pRight
) {

    return pLeft->pCurrentNode == pRight->pCurrentNode;
}
