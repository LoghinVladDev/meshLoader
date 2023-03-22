//
// Created by loghin on 3/22/2023.
//

#ifndef __MESH_LOADER_JOB_PAUSE_TABLE
#define __MESH_LOADER_JOB_PAUSE_TABLE

#include <meshLoader/publicTypes>
#include "job.h"
#include "internalAllocation.h"
#include "mutex.h"

typedef struct __MeshLoader_JobPauseTable_Node {
    struct __MeshLoader_JobPauseTable_Node    * pNext;
    __MeshLoader_Job_RuntimeContext           * pContext;
    float                                       priorityInQueue;
} __MeshLoader_JobPauseTable_Node;


typedef __MeshLoader_JobPauseTable_Node * __MeshLoader_JobPauseTable_List;
typedef __MeshLoader_JobPauseTable_List * __MeshLoader_JobPauseTable_BucketArray;

typedef struct {
    __MeshLoader_JobPauseTable_BucketArray  bucketArray;
    MeshLoader_size                         size;
    MeshLoader_size                         bucketCount;
    MeshLoader_size                         growthIndex;
} __MeshLoader_JobPauseTable;


typedef struct {
    MeshLoader_size                             bucketIndex;
    MeshLoader_size                             bucketCount;
    __MeshLoader_JobPauseTable_Node           * pCurrentNode;
    __MeshLoader_JobPauseTable_Node           * pPreviousNode;
    __MeshLoader_JobPauseTable_BucketArray      bucketArray;
} __MeshLoader_JobPauseTable_Iterator;


extern MeshLoader_Result __MeshLoader_JobPauseTable_construct (
        __MeshLoader_JobPauseTable                    *,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern void __MeshLoader_JobPauseTable_destruct (
        __MeshLoader_JobPauseTable              const *,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern MeshLoader_Result __MeshLoader_JobPauseTable_emplace (
        __MeshLoader_JobPauseTable                    *,
        __MeshLoader_Job_RuntimeContext               *,
        float,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern MeshLoader_Result __MeshLoader_JobPauseTable_remove (
        __MeshLoader_JobPauseTable                    *,
        __MeshLoader_Job_RuntimeContext         const *,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern MeshLoader_Result __MeshLoader_JobPauseTable_begin (
        __MeshLoader_JobPauseTable                    *,
        __MeshLoader_JobPauseTable_Iterator           *
);

extern MeshLoader_Result __MeshLoader_JobPauseTable_end (
        __MeshLoader_JobPauseTable                    *,
        __MeshLoader_JobPauseTable_Iterator           *
);

extern MeshLoader_Result __MeshLoader_JobPauseTable_Iterator_get (
        __MeshLoader_JobPauseTable_Iterator     const *,
        __MeshLoader_Job_RuntimeContext              **,
        float                                         *
);

extern MeshLoader_Result __MeshLoader_JobPauseTable_Iterator_next (
        __MeshLoader_JobPauseTable_Iterator           *
);

extern MeshLoader_bool __MeshLoader_JobPauseTable_Iterator_equal (
        __MeshLoader_JobPauseTable_Iterator     const *,
        __MeshLoader_JobPauseTable_Iterator     const *
);


#endif /* __MESH_LOADER_JOB_PAUSE_TABLE */
