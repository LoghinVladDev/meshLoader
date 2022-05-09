//
// Created by loghin on 5/4/22.
//

#if defined(__linux__)

#include <pthread.h>
#include <meshLoader/publicTypes>
#include <stdalign.h>
#include <stdatomic.h>
#include "../internalAllocation.h"
#include "../thread.h"

#define __MeshLoader_Thread_InactiveThread 0x0000000000000000U

struct __MeshLoader_Posix_Thread {
    pthread_t                       thread;
    __MeshLoader_Thread_Function    function;
    __MeshLoader_Thread_Parameters  parameters;
    atomic_int                      running;
};

MeshLoader_Result __MeshLoader_Thread_create (
        struct __MeshLoader_Posix_Thread             ** pThread,
        __MeshLoader_Thread_Function                    function,
        __MeshLoader_Thread_Parameters          const * pParameters,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType              = MeshLoader_StructureType_AllocationNotification,
            .pNext                      = NULL,
            .pMemory                    = NULL,
            .pOldMemory                 = NULL,
            .size                       = sizeof ( struct __MeshLoader_Posix_Thread ),
            .alignment                  = alignof ( struct __MeshLoader_Posix_Thread ),
            .allocationScope            = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose      = "Creates a Thread Object ( posix )"
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

    * pThread = ( struct __MeshLoader_Posix_Thread * ) allocationNotification.pMemory;

    ( * pThread )->thread                     = __MeshLoader_Thread_InactiveThread;
    ( * pThread )->running                    = MeshLoader_false;
    ( * pThread )->parameters.parameterCount  = 0U;
    ( * pThread )->function                   = function;

    if ( pParameters != NULL ) {

        ( * pThread )->parameters.parameterCount = pParameters->parameterCount;

        allocationNotification.size                     = sizeof ( __MeshLoader_Thread_Parameter ) * ( * pThread )->parameters.parameterCount;
        allocationNotification.alignment                = alignof ( __MeshLoader_Thread_Parameter );
        allocationNotification.explicitMemoryPurpose    = "Creates the Array of Parameters for a Thread";
        allocationNotification.pMemory                  = pAllocationCallbacks->pAllocationCallbacks->allocationFunction (
                pAllocationCallbacks->pAllocationCallbacks->pUserData,
                allocationNotification.size,
                allocationNotification.alignment,
                allocationNotification.allocationScope
        );

        if ( allocationNotification.pMemory == NULL ) {
            allocationNotification.pMemory                  = ( * pThread );
            allocationNotification.size                     = sizeof ( struct __MeshLoader_Posix_Thread );
            allocationNotification.alignment                = alignof ( struct __MeshLoader_Posix_Thread );
            allocationNotification.explicitMemoryPurpose    = "Destroys a Thread Object ( posix )";

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

            return MeshLoader_Result_OutOfMemory;
        }

        ( * pThread )->parameters.pParameters = ( __MeshLoader_Thread_Parameter * ) allocationNotification.pMemory;

        if ( pAllocationCallbacks->pAllocationCallbacks->internalAllocationNotificationFunction != NULL ) {
            pAllocationCallbacks->pAllocationCallbacks->internalAllocationNotificationFunction (
                    pAllocationCallbacks->pAllocationCallbacks->pUserData,
                    & allocationNotification
            );
        }

        for ( MeshLoader_uint32 parameterIndex = 0U; parameterIndex < pParameters->parameterCount; ++ parameterIndex ) {

            ( * pThread )->parameters.pParameters [ parameterIndex ].pData = pParameters->pParameters [ parameterIndex ].pData;
        }
    }

    return MeshLoader_Result_Success;
}

void __MeshLoader_Thread_destroy (
        struct __MeshLoader_Posix_Thread              * pThread,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType              = MeshLoader_StructureType_AllocationNotification,
            .pNext                      = NULL,
            .pMemory                    = pThread->parameters.pParameters,
            .pOldMemory                 = NULL,
            .size                       = sizeof ( __MeshLoader_Thread_Parameter ) * pThread->parameters.parameterCount,
            .alignment                  = alignof ( __MeshLoader_Thread_Parameter ),
            .allocationScope            = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose      = "Destroys an Array of Thread Parameters"
    };

    if ( pThread->thread != __MeshLoader_Thread_InactiveThread ) {
        if ( pThread->running ) {
            __MeshLoader_Thread_kill ( pThread );
        } else {
            __MeshLoader_Thread_join ( pThread );
        }
    }

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

    allocationNotification.pMemory                  = pThread;
    allocationNotification.size                     = sizeof ( struct __MeshLoader_Posix_Thread );
    allocationNotification.alignment                = alignof ( struct __MeshLoader_Posix_Thread );
    allocationNotification.explicitMemoryPurpose    = "Destroys a Thread Object ( posix )";

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

void __MeshLoader_Thread_join (
        struct __MeshLoader_Posix_Thread * pThread
) {
    pthread_join ( pThread->thread, NULL );
    pThread->running = MeshLoader_false;
}

void __MeshLoader_Thread_kill (
        struct __MeshLoader_Posix_Thread * pThread
) {
    pthread_cancel ( pThread->thread );
    pThread->running = MeshLoader_false;
}

static void * __MeshLoader_Thread_privateStart ( void * pPrivateParameter ) {

    struct __MeshLoader_Posix_Thread * pThread = ( struct __MeshLoader_Posix_Thread * ) pPrivateParameter;

    pThread->function ( & pThread->parameters );
    pThread->running = MeshLoader_false;
    return NULL;
}

void __MeshLoader_Thread_start (
        struct __MeshLoader_Posix_Thread * pThread
) {

    pThread->running = MeshLoader_true;
    pthread_create ( & pThread->thread, NULL, & __MeshLoader_Thread_privateStart, (void *) pThread );
}

void __MeshLoader_Thread_isRunning (
        struct __MeshLoader_Posix_Thread * pThread,
        MeshLoader_bool                  * pIsRunning
) {
    * pIsRunning = pThread->running;
}

void __MeshLoader_Thread_detach (
        struct __MeshLoader_Posix_Thread * pThread
) {
    pthread_detach ( pThread->thread );
}

#endif
