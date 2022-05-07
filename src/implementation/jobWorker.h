//
// Created by loghin on 5/7/22.
//

#ifndef __MESH_LOADER_JOB_WORKER_H__
#define __MESH_LOADER_JOB_WORKER_H__

#include <meshLoader/publicTypes>
#include "thread.h"
#include "jobDispatcher.h"

typedef struct {
    __MeshLoader_Mutex                          lock;
    __MeshLoader_JobDispatcher_ContextNode    * pCurrentDispatcherContextNode;
    __MeshLoader_Thread                         thread;
} __MeshLoader_JobWorker;

typedef struct {
    __MeshLoader_JobWorker    * pWorkers;
    MeshLoader_uint32           length;
    __MeshLoader_Mutex          lock;
} __MeshLoader_JobWorker_Manager;

extern MeshLoader_Result __MeshLoader_JobWorker_Manager_construct (
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

extern MeshLoader_Result __MeshLoader_JobWorker_newJobsAddedNotification (
        __MeshLoader_JobWorker_Manager *,
        MeshLoader_uint32
);

#endif // __MESH_LOADER_JOB_WORKER_H__
