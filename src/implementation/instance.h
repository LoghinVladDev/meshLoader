//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_INSTANCE_H__
#define __MESH_LOADER_INSTANCE_H__

#include "privateTypes.h"
#include "mutex.h"

typedef struct __MeshLoader_Instance_Node {
    struct __MeshLoader_Instance_Node * pNextInstanceNode;
    struct __MeshLoader_Instance        instanceData;
} __MeshLoader_Instance_Node;

typedef struct {
    __MeshLoader_Instance_Node      * pInstanceList;
} __MeshLoader_Instance_Control;

static MeshLoader_Result __MeshLoader_Instance_acquireNewNode (
        __MeshLoader_Instance_Control           *,
        __MeshLoader_Instance_Node             **,
        MeshLoader_AllocationCallbacks    const *
);

static void __MeshLoader_Instance_destroy (
        MeshLoader_Instance
);

static void __MeshLoader_Instance_removeInstanceNode (
        __MeshLoader_Instance_Control           *,
        MeshLoader_Instance,
        MeshLoader_AllocationCallbacks    const *
);

#endif // __MESH_LOADER_INSTANCE_H__
