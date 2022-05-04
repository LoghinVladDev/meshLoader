//
// Created by loghin on 5/4/22.
//

#include <pthread.h>
#include <meshLoader/publicTypes>
#include <stdalign.h>
#include "../internalAllocation.h"

#if defined(__linux__)

static pthread_mutex_t  __MeshLoader_PosixMutex_moduleLock;
static MeshLoader_bool  __MeshLoader_PosixMutex_firstApplyModuleLockCall = MeshLoader_true;

struct __MeshLoader_Posix_Mutex {
    pthread_mutex_t mutex;
};

MeshLoader_Result __MeshLoader_applyModuleLock () {
    int mutexRetVal;

    if ( __MeshLoader_PosixMutex_firstApplyModuleLockCall ) {

        mutexRetVal = pthread_mutex_init ( & __MeshLoader_PosixMutex_moduleLock, NULL );
        if ( mutexRetVal != 0 ) {
            return MeshLoader_Result_MutexError;
        }

        __MeshLoader_PosixMutex_firstApplyModuleLockCall = MeshLoader_false;
    }

    pthread_mutex_lock ( & __MeshLoader_PosixMutex_moduleLock );
    return MeshLoader_Result_Success;
}

void __MeshLoader_removeModuleLock () {
    pthread_mutex_unlock ( & __MeshLoader_PosixMutex_moduleLock );
}

MeshLoader_Result __MeshLoader_Mutex_create (
        struct __MeshLoader_Posix_Mutex              ** ppMutex,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    int mutexRetVal;

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = sizeof ( struct __MeshLoader_Posix_Mutex ),
            .alignment              = alignof ( struct __MeshLoader_Posix_Mutex ),
            .allocationScope        = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose  = "Creates a Mutex type Object ( posix )"
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

    mutexRetVal = pthread_mutex_init (
            & ( ( struct __MeshLoader_Posix_Mutex * ) allocationNotification.pMemory )->mutex,
            NULL
    );

    if ( mutexRetVal != 0 ) {
        if ( pAllocationCallbacks->pAllocationCallbacks->internalFreeNotificationFunction != NULL ) {
            allocationNotification.explicitMemoryPurpose = "Deletes a Mutex type Object ( pthread_mutex_init failure )";
            pAllocationCallbacks->pAllocationCallbacks->internalFreeNotificationFunction (
                    pAllocationCallbacks->pAllocationCallbacks->pUserData,
                    & allocationNotification
            );
        }

        pAllocationCallbacks->pAllocationCallbacks->freeFunction (
                pAllocationCallbacks->pAllocationCallbacks->freeFunction,
                allocationNotification.pMemory
        );

        return MeshLoader_Result_MutexError;
    }

    * ppMutex = ( struct __MeshLoader_Posix_Mutex * ) allocationNotification.pMemory;
    return MeshLoader_Result_Success;
}

void __MeshLoader_Mutex_destroy (
        struct __MeshLoader_Posix_Mutex               * pMutex,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {
    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = pMutex,
            .pOldMemory             = NULL,
            .size                   = sizeof ( struct __MeshLoader_Posix_Mutex ),
            .alignment              = alignof ( struct __MeshLoader_Posix_Mutex ),
            .allocationScope        = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose  = "Destroys a Mutex type Object at end of life ( posix )"
    };

    pthread_mutex_destroy ( & pMutex->mutex );

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

MeshLoader_Result __MeshLoader_Mutex_lock (
        struct __MeshLoader_Posix_Mutex * pMutex
) {

    if ( 0 != pthread_mutex_lock ( & pMutex->mutex ) ) {
        return MeshLoader_Result_MutexError;
    }

    return MeshLoader_Result_Success;
}

void __MeshLoader_Mutex_unlock (
        struct __MeshLoader_Posix_Mutex * pMutex
) {

    pthread_mutex_unlock ( & pMutex->mutex );
}

#endif