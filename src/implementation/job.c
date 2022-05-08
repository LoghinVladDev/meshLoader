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
            & job->context.inputPath,
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
                & job->context.inputPath,
                & scopedAllocationCallbacks
        );

        return result;
    }

    job->priority               = pCreateInfo->priority;
    job->context.loadMode       = pCreateInfo->loadMode;
    job->context.jobProgress    = MESH_LOADER_JOB_PROGRESS_MIN_VALUE;
    job->context.jobStatus      = ( MeshLoader_uint8 ) MeshLoader_JobStatus_Ready;

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
            & job->context.inputPath,
            & scopedAllocationCallbacks
    );
}

MeshLoader_Result MeshLoader_Job_getProgress (
        MeshLoader_Job      job,
        float             * pProgress
) {

    return __MeshLoader_Job_getProgress (
            & job->context,
            pProgress
    );
}
