//
// Created by loghin on 5/4/22.
//

#include "job.h"

MeshLoader_Result __MeshLoader_Job_construct (
        MeshLoader_Job                          job,
        MeshLoader_CreateJobInfo        const * pCreateInfo,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    (void) job;
    (void) pCreateInfo;
    (void) pAllocationCallbacks;

    return MeshLoader_Result_Success;
}

void __MeshLoader_Job_destruct (
        MeshLoader_Job                          job,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    (void) job;
    (void) pAllocationCallbacks;
}
