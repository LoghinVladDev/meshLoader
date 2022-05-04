//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_JOB_H__
#define __MESH_LOADER_JOB_H__

#include <meshLoader/publicTypes>
#include "thread.h"
#include "mutex.h"
#include "string.h"

struct __MeshLoader_Job {
    __MeshLoader_Thread             thread;
    __MeshLoader_Mutex              jobLock;

    MeshLoader_MeshLoadModeFlags    loadMode;
    __MeshLoader_String             inputPath;
    float                           priority;
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

#endif // __MESH_LOADER_JOB_H__
