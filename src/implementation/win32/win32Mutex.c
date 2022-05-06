//
// Created by loghin on 5/5/2022.
//

#if defined(WIN32)

#include <windows.h>
#include <meshLoader/publicTypes>
#include <stdalign.h>
#include "../internalAllocation.h"

static CRITICAL_SECTION __MeshLoader_Win32Mutex_moduleLock;
static MeshLoader_bool  __MeshLoader_Win32Mutex_firstApplyModuleLockCall = MeshLoader_true;

struct __MeshLoader_Win32_Mutex {
    CRITICAL_SECTION mutex;
};

MeshLoader_Result __MeshLoader_Mutex_applyModuleLock () {
    if ( __MeshLoader_Win32Mutex_firstApplyModuleLockCall ) {
        InitializeCriticalSection ( & __MeshLoader_Win32Mutex_moduleLock );
        __MeshLoader_Win32Mutex_firstApplyModuleLockCall = MeshLoader_false;
    }

    EnterCriticalSection ( & __MeshLoader_Win32Mutex_moduleLock );
    return MeshLoader_Result_Success;
}

void __MeshLoader_Mutex_removeModuleLock () {
    LeaveCriticalSection ( & __MeshLoader_Win32Mutex_moduleLock );
}

MeshLoader_Result __MeshLoader_Mutex_create (
        struct __MeshLoader_Win32_Mutex              ** ppMutex,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    int mutexRetVal;

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = sizeof ( struct __MeshLoader_Win32_Mutex ),
            .alignment              = alignof ( struct __MeshLoader_Win32_Mutex ),
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

    InitializeCriticalSection (
            & ( ( struct __MeshLoader_Win32_Mutex * ) allocationNotification.pMemory )->mutex
    );

    * ppMutex = ( struct __MeshLoader_Win32_Mutex * ) allocationNotification.pMemory;
    return MeshLoader_Result_Success;
}

void __MeshLoader_Mutex_destroy (
        struct __MeshLoader_Win32_Mutex               * pMutex,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {
    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = pMutex,
            .pOldMemory             = NULL,
            .size                   = sizeof ( struct __MeshLoader_Win32_Mutex ),
            .alignment              = alignof ( struct __MeshLoader_Win32_Mutex ),
            .allocationScope        = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose  = "Destroys a Mutex type Object at end of life ( posix )"
    };

    DeleteCriticalSection ( & pMutex->mutex );

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
        struct __MeshLoader_Win32_Mutex * pMutex
) {

    EnterCriticalSection ( & pMutex->mutex );
    return MeshLoader_Result_Success;
}

void __MeshLoader_Mutex_unlock (
        struct __MeshLoader_Win32_Mutex * pMutex
) {

    LeaveCriticalSection ( & pMutex->mutex );
}

void __MeshLoader_Mutex_clearModuleLock () {

    DeleteCriticalSection ( & __MeshLoader_Win32Mutex_moduleLock );
    __MeshLoader_Win32Mutex_firstApplyModuleLockCall = MeshLoader_false;
}

#endif
