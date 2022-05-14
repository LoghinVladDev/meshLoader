//
// Created by loghin on 5/6/22.
//

#ifndef __MESH_LOADER_JOB_DISPATCHER_H__
#define __MESH_LOADER_JOB_DISPATCHER_H__

#include <meshLoader/publicTypes>
#include "jobPriorityQueue.h"
#include "jobPauseTable.h"
#include "mutex.h"
#include "jobMemoryAllocator.h"

typedef struct {
    MeshLoader_AllocationCallbacks  allocationCallbacks;
    __MeshLoader_JobPriorityQueue   jobQueue;

    MeshLoader_uint32               jobCount;
    MeshLoader_uint32               finishedJobCount;

    __MeshLoader_JobPauseTable      jobPauseTable;
} __MeshLoader_JobDispatcher_Context;

typedef struct __MeshLoader_JobDispatcher_ContextNode {
    struct __MeshLoader_JobDispatcher_ContextNode * pNext;
    __MeshLoader_JobDispatcher_Context              context;
} __MeshLoader_JobDispatcher_ContextNode;

typedef __MeshLoader_JobDispatcher_ContextNode * __MeshLoader_JobDispatcher_ContextList;

typedef struct {
    __MeshLoader_JobDispatcher_ContextList  contextList;
    __MeshLoader_Mutex                      lock;
} __MeshLoader_JobDispatcher;

extern MeshLoader_Result __MeshLoader_JobDispatcher_construct (
        __MeshLoader_JobDispatcher            *,
        MeshLoader_AllocationCallbacks  const *
);

extern void __MeshLoader_JobDispatcher_destruct (
        __MeshLoader_JobDispatcher            *,
        MeshLoader_AllocationCallbacks  const *
);

static MeshLoader_Result __MeshLoader_JobDispatcher_createContext (
        __MeshLoader_JobDispatcher        *,
        MeshLoader_JobsStartInfo    const *
);

static MeshLoader_Result __MeshLoader_JobDispatcher_allocateContext (
        __MeshLoader_JobDispatcher_ContextNode       **,
        MeshLoader_JobsStartInfo                const *
);

static void __MeshLoader_JobDispatcher_freeContext (
        __MeshLoader_JobDispatcher_ContextNode *
);

extern MeshLoader_Result __MeshLoader_JobDispatcher_acquireJob (
        __MeshLoader_JobDispatcher              *,
        __MeshLoader_JobDispatcher_ContextNode **,
        __MeshLoader_Job_RuntimeContext        **,
        float                                   *
);

extern MeshLoader_Result __MeshLoader_JobDispatcher_releaseJob (
        __MeshLoader_JobDispatcher              *,
        __MeshLoader_JobDispatcher_ContextNode  *,
        __MeshLoader_Job_RuntimeContext         *,
        float
);

static void __MeshLoader_JobDispatcher_removeContextNode (
        __MeshLoader_JobDispatcher              *,
        __MeshLoader_JobDispatcher_ContextNode  *
);

#endif // __MESH_LOADER_JOB_DISPATCHER_H__
