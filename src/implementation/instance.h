//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_INSTANCE_H__
#define __MESH_LOADER_INSTANCE_H__

#include "mutex.h"
#include "jobPriorityQueue.h"

typedef struct __MeshLoader_Instance_JobNode {
    struct __MeshLoader_Instance_JobNode  * pNextJobNode;
    struct __MeshLoader_Job                 jobs[32];
    MeshLoader_uint32                       nodeUsage;
} __MeshLoader_Instance_JobNode;

typedef __MeshLoader_Instance_JobNode * __MeshLoader_Instance_JobList;

struct __MeshLoader_Instance {
    MeshLoader_uint32               maxThreadCount;
    __MeshLoader_Mutex              instanceLock;
    __MeshLoader_Instance_JobList   jobList;

#if MESH_LOADER_DEBUG_MODE

    MeshLoader_uint32               totalJobCount;
    MeshLoader_uint32               readyJobCount;
    MeshLoader_uint32               runningJobCount;
    MeshLoader_uint32               stoppedJobCount;
    MeshLoader_uint32               finishedJobCount;
    MeshLoader_uint32               finishedErrorJobCount;

#endif
};

typedef struct __MeshLoader_Instance_Node {
    struct __MeshLoader_Instance_Node * pNextInstanceNode;
    struct __MeshLoader_Instance        instanceData;
} __MeshLoader_Instance_Node;

typedef struct {
    __MeshLoader_Instance_Node      * pInstanceList;
} __MeshLoader_Instance_Control;

static inline MeshLoader_bool __MeshLoader_Instance_noInstanceExists (
        __MeshLoader_Instance_Control const * pControl
) {
    return pControl->pInstanceList == NULL;
}

static MeshLoader_Result __MeshLoader_Instance_createControl (
        __MeshLoader_Instance_Control         *,
        MeshLoader_AllocationCallbacks  const *
);

static void __MeshLoader_Instance_destroyControl (
        __MeshLoader_Instance_Control   const *,
        MeshLoader_AllocationCallbacks  const *
);

static MeshLoader_Result __MeshLoader_Instance_construct (
        MeshLoader_Instance,
        MeshLoader_InstanceCreateInfo   const *,
        MeshLoader_AllocationCallbacks  const *
);

static void __MeshLoader_Instance_destruct (
        MeshLoader_Instance,
        MeshLoader_AllocationCallbacks  const *
);

static MeshLoader_Result __MeshLoader_Instance_allocateInstance (
        __MeshLoader_Instance_Control           *,
        MeshLoader_Instance                     *,
        MeshLoader_AllocationCallbacks    const *
);

static void __MeshLoader_Instance_freeInstance (
        __MeshLoader_Instance_Control           *,
        MeshLoader_Instance,
        MeshLoader_AllocationCallbacks    const *
);

static MeshLoader_Result __MeshLoader_Instance_allocateJobsFillExisting (
        MeshLoader_Instance,
        MeshLoader_uint32,
        MeshLoader_Job                        *,
        MeshLoader_uint32                     *
);

static MeshLoader_Result __MeshLoader_Instance_allocateJobsNewNodes (
        MeshLoader_Instance,
        MeshLoader_uint32,
        MeshLoader_Job                        *,
        MeshLoader_AllocationCallbacks  const *,
        MeshLoader_uint32
);

static MeshLoader_Result __MeshLoader_Instance_allocateJobNode (
        MeshLoader_Instance,
        __MeshLoader_Instance_JobNode        **,
        MeshLoader_AllocationCallbacks  const *
);

static void __MeshLoader_Instance_cleanupAllocatedJobNodes (
        MeshLoader_Instance,
        __MeshLoader_Instance_JobNode   const *,
        MeshLoader_AllocationCallbacks  const *
);

static MeshLoader_Result __MeshLoader_Instance_allocateJobs (
        MeshLoader_Instance,
        MeshLoader_uint32,
        MeshLoader_Job                        *,
        MeshLoader_AllocationCallbacks  const *
);

static void __MeshLoader_Instance_freeJobs (
        MeshLoader_Instance,
        MeshLoader_uint32,
        MeshLoader_Job                  const *,
        MeshLoader_AllocationCallbacks  const *
);

static void __MeshLoader_Instance_freeJobsInFirstNode (
        MeshLoader_Instance,
        MeshLoader_uint32,
        MeshLoader_Job                  const *,
        MeshLoader_AllocationCallbacks  const *
);

static void __MeshLoader_Instance_freeFirstJobNode (
        MeshLoader_Instance,
        MeshLoader_AllocationCallbacks  const *
);

static void __MeshLoader_Instance_freeJobsNextInNode (
        __MeshLoader_Instance_JobNode         *,
        MeshLoader_uint32,
        MeshLoader_Job                  const *,
        MeshLoader_AllocationCallbacks  const *
);

static void __MeshLoader_Instance_freeNextJobNode (
        __MeshLoader_Instance_JobNode         *,
        MeshLoader_AllocationCallbacks  const *
);

static void __MeshLoader_Instance_freeAllJobs (
        MeshLoader_Instance,
        MeshLoader_AllocationCallbacks  const *
);

static void __MeshLoader_Instance_freeAllJobsInNode (
        __MeshLoader_Instance_JobNode         *,
        MeshLoader_AllocationCallbacks  const *
);

#endif // __MESH_LOADER_INSTANCE_H__
