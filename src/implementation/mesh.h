//
// Created by loghin on 5/10/22.
//

#ifndef __MESH_LOADER_MESH_H__
#define __MESH_LOADER_MESH_H__

#include <meshLoader/publicTypes>

struct __MeshLoader_Mesh {
    MeshLoader_MeshData data;
};

extern MeshLoader_Result __MeshLoader_Mesh_construct (
        MeshLoader_Mesh,
        MeshLoader_AllocationCallbacks  const *
);

extern void __MeshLoader_Mesh_destruct (
        MeshLoader_Mesh,
        MeshLoader_AllocationCallbacks  const *
);

#endif // __MESH_LOADER_MESH_H__
