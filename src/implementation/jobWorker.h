//
// Created by loghin on 5/7/22.
//

#ifndef __MESH_LOADER_JOB_WORKER_H__
#define __MESH_LOADER_JOB_WORKER_H__

#include <meshLoader/publicTypes>
#include "thread.h"
#include "jobDispatcher.h"
#include <stdatomic.h>

typedef enum {
    __MeshLoader_JobWorker_State_NotStarted             = 0x00000001U,
    __MeshLoader_JobWorker_State_Initializing           = 0x00000002U,
    __MeshLoader_JobWorker_State_AcquiringJobContext    = 0x00000003U,
    __MeshLoader_JobWorker_State_ReleasingJobContext    = 0x00000100U,
    __MeshLoader_JobWorker_State_Error                  = 0x00000200U,
    __MeshLoader_JobWorker_State_Cleanup                = 0x00000400U,
} __MeshLoader_JobWorker_State;

typedef struct {
    __MeshLoader_Mutex                          lock;
    __MeshLoader_JobDispatcher_ContextNode    * pCurrentDispatcherContextNode;
    __MeshLoader_Job_RuntimeContext           * pRuntimeContext;
    float                                       activeJobPriority;
    __MeshLoader_Thread                         thread;
    __MeshLoader_JobWorker_State                state;
    atomic_bool                                 keepAlive;
    MeshLoader_StringLiteral                    errorReason;
} __MeshLoader_JobWorker;

typedef struct {
    __MeshLoader_JobWorker    * pWorkers;
    MeshLoader_uint32           length;
    __MeshLoader_Mutex          lock;
} __MeshLoader_JobWorker_Manager;

extern MeshLoader_Result __MeshLoader_JobWorker_Manager_construct (
        MeshLoader_Instance,
        __MeshLoader_JobWorker_Manager        *,
        MeshLoader_uint32,
        MeshLoader_AllocationCallbacks  const *
);

extern void __MeshLoader_JobWorker_Manager_destruct (
        __MeshLoader_JobWorker_Manager        *,
        MeshLoader_AllocationCallbacks  const *
);

static void __MeshLoader_JobWorker_main (
        __MeshLoader_Thread_Parameters const *
);

extern MeshLoader_Result __MeshLoader_JobWorker_Manager_newJobsAddedNotification (
        __MeshLoader_JobWorker_Manager *,
        MeshLoader_uint32
);

extern MeshLoader_Result __MeshLoader_JobWorker_Manager_anyWorkersRunning (
        __MeshLoader_JobWorker_Manager *,
        MeshLoader_bool                *
);

#endif // __MESH_LOADER_JOB_WORKER_H__
