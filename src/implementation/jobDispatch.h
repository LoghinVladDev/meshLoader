//
// Created by loghin on 5/6/22.
//

#ifndef __MESH_LOADER_JOB_DISPATCH_H__
#define __MESH_LOADER_JOB_DISPATCH_H__

#include <meshLoader/publicTypes>
#include "string.h"

typedef struct {
    MeshLoader_AllocationCallbacks  allocationCallbacks;
    MeshLoader_MeshLoadModeFlags    loadMode;
    __MeshLoader_String             inputPath;
} __MeshLoader_JobDispatch_Context;

#endif // __MESH_LOADER_JOB_DISPATCH_H__
