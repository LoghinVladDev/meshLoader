//
// Created by loghin on 5/6/22.
//

#ifndef __MESH_LOADER_JOB_PRIORITY_QUEUE_H__
#define __MESH_LOADER_JOB_PRIORITY_QUEUE_H__

#include <meshLoader/publicTypes>
#include "job.h"
#include "internalAllocation.h"
#include "mutex.h"

typedef struct {
    __MeshLoader_Job_RuntimeContext   * pContext;
    float                               priority;
} __MeshLoader_JobPriorityQueue_Entry;

typedef struct {
    __MeshLoader_JobPriorityQueue_Entry   * ppEntries;
    MeshLoader_uint32                       length;
    MeshLoader_uint32                       capacity;

    __MeshLoader_Mutex                      lock;
} __MeshLoader_JobPriorityQueue;

extern MeshLoader_Result __MeshLoader_JobPriorityQueue_construct (
        __MeshLoader_JobPriorityQueue                 *,
        MeshLoader_uint32,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern void __MeshLoader_JobPriorityQueue_destruct (
        __MeshLoader_JobPriorityQueue           const *,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern MeshLoader_Result __MeshLoader_JobPriorityQueue_push (
        __MeshLoader_JobPriorityQueue                 *,
        __MeshLoader_Job_RuntimeContext               *,
        float
);

extern MeshLoader_Result __MeshLoader_JobPriorityQueue_pop (
        __MeshLoader_JobPriorityQueue                 *,
        __MeshLoader_Job_RuntimeContext              **
);

extern MeshLoader_Result __MeshLoader_JobPriorityQueue_peek (
        __MeshLoader_JobPriorityQueue           const *,
        __MeshLoader_JobPriorityQueue_Entry    const **
);

static inline MeshLoader_bool __MeshLoader_JobPriorityQueue_empty (
        __MeshLoader_JobPriorityQueue           const * pQueue
) {
    return pQueue->length == 0U;
}

#endif // __MESH_LOADER_JOB_PRIORITY_QUEUE_H__
