//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_JOB_H__
#define __MESH_LOADER_JOB_H__

#include <meshLoader/publicTypes>
#include <meshLoader/customJob>
#include "thread.h"
#include "mutex.h"
#include "string.h"
#include <stdatomic.h>
#include "../config/instanceCnf.h"
#include "jobMemoryAllocator.h"

typedef struct {
    MeshLoader_MeshLoadModeFlags        loadMode;
    __MeshLoader_String                 inputPath;
    atomic_uint_fast32_t                jobProgress;
    atomic_uint_fast8_t                 jobStatus;
    MeshLoader_CustomJobInfo    const * pCustomJobInfo;
    __MeshLoader_JobMemoryAllocator     jobMemoryAllocator;
    void                              * pNextCallData;
} __MeshLoader_Job_RuntimeContext;

struct __MeshLoader_Job_Context {
    MeshLoader_StructureType                    structureType;
    void                                const * pNext;
    void                                      * pUserData;
    MeshLoader_MeshLoadModeFlags                loadMode;
    MeshLoader_StringLiteral                    inputPath;
    float                                       progress;
    MeshLoader_JobStatus                        status;
    __MeshLoader_JobMemoryAllocator           * pMemoryAllocator;
    void                                      * pNextCallData;
};

struct __MeshLoader_Job {
    __MeshLoader_Mutex                  jobLock;
    float                               priority;

    __MeshLoader_Job_RuntimeContext     context;
    MeshLoader_JobType                  jobType;
};

extern MeshLoader_Result __MeshLoader_Job_construct (
        MeshLoader_Job,
        MeshLoader_CreateJobInfo        const *,
        MeshLoader_AllocationCallbacks  const *
);

static MeshLoader_Result __MeshLoader_Job_constructCustomJobInfo (
        MeshLoader_Job,
        MeshLoader_CustomJobInfo        const *,
        MeshLoader_AllocationCallbacks  const *
);

extern void __MeshLoader_Job_destruct (
        MeshLoader_Job,
        MeshLoader_AllocationCallbacks  const *
);

static inline MeshLoader_Result __MeshLoader_Job_setProgress (
        __MeshLoader_Job_RuntimeContext   * pContext,
        float                               progress
) {

    pContext->jobProgress = ( MeshLoader_uint32 ) ( progress * ( ( float ) MESH_LOADER_JOB_PROGRESS_MAX_VALUE ) );

    return MeshLoader_Result_Success;
}

static inline MeshLoader_Result __MeshLoader_Job_getProgress (
        __MeshLoader_Job_RuntimeContext   const * pContext,
        float                                   * pProgress
) {

    * pProgress = ( float ) pContext->jobProgress / ( float ) MESH_LOADER_JOB_PROGRESS_MAX_VALUE;

    return MeshLoader_Result_Success;
}

static inline MeshLoader_Result __MeshLoader_Job_setStatus (
        __MeshLoader_Job_RuntimeContext   * pContext,
        MeshLoader_JobStatus                status
) {

    pContext->jobStatus = ( MeshLoader_uint8 ) status;
    return MeshLoader_Result_Success;
}

static inline MeshLoader_Result __MeshLoader_Job_getStatus (
        __MeshLoader_Job_RuntimeContext   const * pContext,
        MeshLoader_JobStatus                    * pStatus
) {

    * pStatus = ( MeshLoader_JobStatus ) pContext->jobStatus;
    return MeshLoader_Result_Success;
}

#endif // __MESH_LOADER_JOB_H__
