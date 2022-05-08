//
// Created by loghin on 5/8/22.
//

#ifndef __MESH_LOADER_JOB_MEMORY_ALLOCATOR_H__
#define __MESH_LOADER_JOB_MEMORY_ALLOCATOR_H__

#include <meshLoader/publicTypes>
#include "mutex.h"

typedef struct {
    void              * pMemory;
    MeshLoader_size     alignment;
} __MeshLoader_JobMemoryAllocator_TrackedEntry;

typedef struct {
    __MeshLoader_JobMemoryAllocator_TrackedEntry  * pEntries;
    MeshLoader_size                                 length;
    MeshLoader_size                                 capacity;
} __MeshLoader_JobMemoryAllocator_TrackingList;

typedef struct {
    __MeshLoader_Mutex                              allocatorLock;
    MeshLoader_AllocationCallbacks                  storedAllocationCallbacks;
    __MeshLoader_JobMemoryAllocator_TrackingList    trackingList;
    MeshLoader_size                                 freeCount;
} __MeshLoader_JobMemoryAllocator;

extern MeshLoader_Result __MeshLoader_JobMemoryAllocator_construct (
        __MeshLoader_JobMemoryAllocator       *,
        MeshLoader_AllocationCallbacks  const *,
        MeshLoader_AllocationCallbacks  const *
);

extern void __MeshLoader_JobMemoryAllocator_destruct (
        __MeshLoader_JobMemoryAllocator       *,
        MeshLoader_AllocationCallbacks  const *
);

static MeshLoader_Result __MeshLoader_JobMemoryAllocator_trackMemory (
        __MeshLoader_JobMemoryAllocator       *,
        void                                  *,
        MeshLoader_size
);

static MeshLoader_Result __MeshLoader_JobMemoryAllocator_reTrackMemory (
        __MeshLoader_JobMemoryAllocator       *,
        void                                  *,
        void                                  *,
        MeshLoader_size
);

static MeshLoader_Result __MeshLoader_JobMemoryAllocator_unTrackMemory (
        __MeshLoader_JobMemoryAllocator       *,
        void                                  *
);

#endif // __MESH_LOADER_JOB_MEMORY_ALLOCATOR_H__
