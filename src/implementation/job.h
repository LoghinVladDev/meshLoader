//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_JOB_H__
#define __MESH_LOADER_JOB_H__

#include <meshLoader/publicTypes>
#include "thread.h"
#include "mutex.h"
#include "string.h"
#include <stdatomic.h>
#include "../config/instanceCnf.h"

typedef struct {
    MeshLoader_MeshLoadModeFlags    loadMode;
    __MeshLoader_String             inputPath;
    atomic_uint_fast32_t            jobProgress;
} __MeshLoader_Job_RuntimeContext;

struct __MeshLoader_Job {
    __MeshLoader_Mutex                  jobLock;
    float                               priority;

    __MeshLoader_Job_RuntimeContext     context;
};

extern MeshLoader_Result __MeshLoader_Job_construct (
        MeshLoader_Job,
        MeshLoader_CreateJobInfo        const *,
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
        __MeshLoader_Job_RuntimeContext   * pContext,
        float                             * pProgress
) {

    * pProgress = ( float ) pContext->jobProgress / ( float ) MESH_LOADER_JOB_PROGRESS_MAX_VALUE;

    return MeshLoader_Result_Success;
}

#endif // __MESH_LOADER_JOB_H__
