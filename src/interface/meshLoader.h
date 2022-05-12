//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_MESH_LOADER_H__
#define __MESH_LOADER_MESH_LOADER_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <meshLoader/publicTypes>

extern MeshLoader_Result MeshLoader_createInstance (
        MeshLoader_InstanceCreateInfo   const * pCreateInfo,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks,
        MeshLoader_Instance                   * pInstance
);

extern void MeshLoader_destroyInstance (
        MeshLoader_Instance                     instance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

extern MeshLoader_Result MeshLoader_createJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsCreateInfo       const * pCreateInfo,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

extern void MeshLoader_destroyJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_uint32                       jobCount,
        MeshLoader_Job                  const * pJobs,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

extern MeshLoader_Result MeshLoader_startJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsStartInfo        const * pStartInfo
);

extern MeshLoader_Result MeshLoader_queryJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsQueryInfo              * pQueryInfo
);

extern MeshLoader_Result MeshLoader_anyJobsRunning (
        MeshLoader_Instance                     instance,
        MeshLoader_bool                       * pAnyRunning
);

extern MeshLoader_Result MeshLoader_takeMesh (
        MeshLoader_Job                          job,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks,
        MeshLoader_Mesh                       * pMesh
);

extern MeshLoader_Result MeshLoader_getMesh (
        MeshLoader_Job                          job,
        MeshLoader_Mesh                       * pMesh
);

extern void MeshLoader_destroyMesh (
        MeshLoader_Mesh                         mesh,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

extern MeshLoader_Result MeshLoader_enumerateMeshVertices (
        MeshLoader_Mesh                         mesh,
        MeshLoader_uint32                     * pVertexCount,
        MeshLoader_VertexData                 * pVertices
);

extern MeshLoader_Result MeshLoader_enumerateMeshFaces (
        MeshLoader_Mesh                         mesh,
        MeshLoader_uint32                     * pFaceCount,
        MeshLoader_FaceData                   * pFaces
);

extern MeshLoader_Result MeshLoader_enumerateIndices (
        MeshLoader_Mesh                         mesh,
        MeshLoader_uint32                     * pIndexCount,
        MeshLoader_uint32                     * pIndices
);

extern MeshLoader_Result MeshLoader_getMeshData (
        MeshLoader_Mesh                         mesh,
        MeshLoader_MeshData                   * pMeshData
);

#if defined(__cplusplus)
}
#endif

#endif // __MESH_LOADER_MESH_LOADER_H__
