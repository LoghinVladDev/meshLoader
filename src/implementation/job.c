//
// Created by loghin on 5/4/22.
//

#include "job.h"
#include <memory.h>

MeshLoader_Result __MeshLoader_Job_construct (
        MeshLoader_Job                          job,
        MeshLoader_CreateJobInfo        const * pCreateInfo,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    MeshLoader_Result result;
    __MeshLoader_ScopedAllocationCallbacks scopedAllocationCallbacks = {
            .pAllocationCallbacks       = pAllocationCallbacks,
            .allocationScope            = MeshLoader_SystemAllocationScope_Object,
            .explicitAllocationPurpose  = NULL
    };

    result = __MeshLoader_String_createFromStringLiteral (
            & job->inputPath,
            pCreateInfo->inputPath,
            & scopedAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    result = __MeshLoader_Mutex_create (
            & job->jobLock,
            & scopedAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        __MeshLoader_String_destroy (
                & job->inputPath,
                & scopedAllocationCallbacks
        );

        return result;
    }

    job->priority   = pCreateInfo->priority;
    job->loadMode   = pCreateInfo->loadMode;

    (void) memcpy ( & job->allocationCallbacks, pAllocationCallbacks, sizeof ( MeshLoader_AllocationCallbacks ) );

    return MeshLoader_Result_Success;
}

void __MeshLoader_Job_destruct (
        MeshLoader_Job                          job,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    __MeshLoader_ScopedAllocationCallbacks scopedAllocationCallbacks = {
            .pAllocationCallbacks       = pAllocationCallbacks,
            .allocationScope            = MeshLoader_SystemAllocationScope_Object,
            .explicitAllocationPurpose  = NULL
    };

    __MeshLoader_Mutex_destroy (
            job->jobLock,
            & scopedAllocationCallbacks
    );

    __MeshLoader_String_destroy (
            & job->inputPath,
            & scopedAllocationCallbacks
    );
}
