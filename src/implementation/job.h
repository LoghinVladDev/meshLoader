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
#include "mesh.h"

typedef enum {
    __MeshLoader_Job_ChangeRequestType_None,
    __MeshLoader_Job_ChangeRequestType_Pause,
    __MeshLoader_Job_ChangeRequestType_Stop,
    __MeshLoader_Job_ChangeRequestType_Resume
} __MeshLoader_Job_ChangeRequestType;

typedef struct {
    MeshLoader_MeshLoadModeFlags        loadMode;
    __MeshLoader_String                 inputPath;
    atomic_uint_fast32_t                jobProgress;
    atomic_uint_fast8_t                 jobState;
    MeshLoader_CustomJobInfo    const * pCustomJobInfo;
    __MeshLoader_JobMemoryAllocator     jobMemoryAllocator;
    void                              * pNextCallData;

    struct __MeshLoader_Mesh            mesh;

    atomic_uint_fast8_t                 requestedChange;
} __MeshLoader_Job_RuntimeContext;

struct __MeshLoader_Job_Context {
    MeshLoader_StructureType                    structureType;
    void                                const * pNext;
    void                                      * pUserData;
    MeshLoader_MeshLoadModeFlags                loadMode;
    MeshLoader_StringLiteral                    inputPath;
    float                                       progress;
    MeshLoader_JobState                         state;
    __MeshLoader_JobMemoryAllocator           * pMemoryAllocator;
    void                                      * pNextCallData;

    MeshLoader_Mesh                             mesh;
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

static inline MeshLoader_Result __MeshLoader_Job_setState (
        __MeshLoader_Job_RuntimeContext   * pContext,
        MeshLoader_JobState                 status
) {

    pContext->jobState = ( MeshLoader_uint8 ) status;
    return MeshLoader_Result_Success;
}

static inline MeshLoader_Result __MeshLoader_Job_getState (
        __MeshLoader_Job_RuntimeContext   const * pContext,
        MeshLoader_JobState                     * pState
) {

    * pState = ( MeshLoader_JobState ) pContext->jobState;
    return MeshLoader_Result_Success;
}

#endif // __MESH_LOADER_JOB_H__
