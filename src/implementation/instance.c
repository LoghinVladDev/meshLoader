//
// Created by loghin on 5/4/22.
//

#include <meshLoader/publicTypes>
#include <stdalign.h>
#include "privateTypes.h"
#include "privateUtility.h"
#include "../config/instanceCnf.h"
#include "instance.h"

static __MeshLoader_Instance_Control __MeshLoader_Instance_control = {
        .pInstanceList  = NULL
};

MeshLoader_Result MeshLoader_createInstance (
        MeshLoader_InstanceCreateInfo   const * pCreateInfo,
        MeshLoader_Instance                   * pInstance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    __MeshLoader_Instance_Node * pNewNode;
    MeshLoader_Result            result = MeshLoader_Result_ErrorUnknown;

    pAllocationCallbacks = __MeshLoader_Utility_nonNullAllocationCallbacks ( pAllocationCallbacks );

    result = __MeshLoader_Instance_acquireNewNode (
            & __MeshLoader_Instance_control,
            & pNewNode,
            pAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        __MeshLoader_Instance_removeInstanceNode (
                & __MeshLoader_Instance_control,
                & pNewNode->instanceData,
                pAllocationCallbacks
        );

        return result;
    }

    result = __MeshLoader_Instance_construct (
            & pNewNode->instanceData,
            pCreateInfo,
            pAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        __MeshLoader_Instance_removeInstanceNode (
                & __MeshLoader_Instance_control,
                & pNewNode->instanceData,
                pAllocationCallbacks
        );

        return result;
    }

    * pInstance = & pNewNode->instanceData;

    return MeshLoader_Result_Success;
}

void MeshLoader_destroyInstance (
        MeshLoader_Instance                     instance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    pAllocationCallbacks = __MeshLoader_Utility_nonNullAllocationCallbacks ( pAllocationCallbacks );

    __MeshLoader_Instance_destruct (
            instance,
            pAllocationCallbacks
    );

    __MeshLoader_Instance_removeInstanceNode (
            & __MeshLoader_Instance_control,
            instance,
            pAllocationCallbacks
    );
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

    pInstance->maxThreadCount = pCreateInfo->maxWorkerThreadCount;

    result = __MeshLoader_Mutex_create (
            & pInstance->instanceLock,
            & scopedAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

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

    __MeshLoader_Mutex_destroy (
            pInstance->instanceLock,
            & scopedAllocationCallbacks
    );
}

static MeshLoader_Result __MeshLoader_Instance_acquireNewNode (
        __MeshLoader_Instance_Control         * pControl,
        __MeshLoader_Instance_Node           ** ppNewNode,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_Result result = MeshLoader_Result_ErrorUnknown;

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

    if ( pAllocationCallbacks->internalAllocationNotificationFunction != NULL ) {

        pAllocationCallbacks->internalAllocationNotificationFunction (
                pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    * ppNewNode = allocationNotification.pMemory;

    result = __MeshLoader_applyModuleLock();
    if ( result != MeshLoader_Result_Success ) {

        allocationNotification.pMemory                  = * ppNewNode;
        allocationNotification.explicitMemoryPurpose    = "Deletes a Node that could not be added due to failure of acquiring the module lock";

        if ( pAllocationCallbacks->internalFreeNotificationFunction != NULL ) {
            pAllocationCallbacks->internalFreeNotificationFunction (
                    pAllocationCallbacks->pUserData,
                    & allocationNotification
            );
        }

        pAllocationCallbacks->freeFunction (
                pAllocationCallbacks->pUserData,
                * ppNewNode
        );

        * ppNewNode = NULL;
        return result;
    }

    ( * ppNewNode )->pNextInstanceNode = pControl->pInstanceList;
    pControl->pInstanceList = * ppNewNode;

    __MeshLoader_removeModuleLock();
    return MeshLoader_Result_Success;
}

static void __MeshLoader_Instance_removeInstanceNode (
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

    __MeshLoader_applyModuleLock();

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
    } else {

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
            }

            pHead = pHead->pNextInstanceNode;
        }
    }

    __MeshLoader_removeModuleLock();
}
