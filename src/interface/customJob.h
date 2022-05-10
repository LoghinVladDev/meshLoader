//
// Created by loghin on 5/8/22.
//

#ifndef __MESH_LOADER_CUSTOM_JOB_H__
#define __MESH_LOADER_CUSTOM_JOB_H__

#include <meshLoader/publicTypes>

typedef struct __MeshLoader_Job_Context * MeshLoader_Job_Context;

typedef MeshLoader_Result ( * MeshLoader_Job_MainFunction ) (
        MeshLoader_Job_Context
);

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    void                              * pUserData;
    MeshLoader_Job_MainFunction         jobFunction;
} MeshLoader_CustomJobInfo;

extern MeshLoader_Result MeshLoader_Job_getProgress (
        MeshLoader_Job_Context    jobContext,
        float                   * pProgress
);

extern MeshLoader_Result MeshLoader_Job_setProgress (
        MeshLoader_Job_Context    jobContext,
        float                     progress
);

extern MeshLoader_Result MeshLoader_Job_getUserData (
        MeshLoader_Job_Context    jobContext,
        void                   ** ppUserData
);

extern MeshLoader_Result MeshLoader_Job_getDataFromPreviousCall (
        MeshLoader_Job_Context    jobContext,
        void                   ** ppData
);

extern MeshLoader_Result MeshLoader_Job_setDataForNextCall (
        MeshLoader_Job_Context    jobContext,
        void                    * pData
);

extern MeshLoader_Result MeshLoader_Job_getLoadMode (
        MeshLoader_Job_Context          jobContext,
        MeshLoader_MeshLoadModeFlags  * pLoadMode
);

extern MeshLoader_Result MeshLoader_Job_getInputPath (
        MeshLoader_Job_Context      jobContext,
        MeshLoader_StringLiteral  * pPath
);

extern MeshLoader_Result MeshLoader_Job_finish (
        MeshLoader_Job_Context      jobContext
);

extern MeshLoader_Result MeshLoader_Job_allocateMemory2 (
        MeshLoader_Job_Context    jobContext,
        MeshLoader_size           size,
        MeshLoader_size           alignment,
        void                   ** ppMemory
);

static inline MeshLoader_Result MeshLoader_Job_allocateMemory (
        MeshLoader_Job_Context      context,
        MeshLoader_size             size,
        void                     ** ppMemory
) {
    return MeshLoader_Job_allocateMemory2 (
            context,
            size,
            1U,
            ppMemory
    );
}

extern MeshLoader_Result MeshLoader_Job_reallocateMemory2 (
        MeshLoader_Job_Context    jobContext,
        void                    * pOldMemory,
        MeshLoader_size           size,
        MeshLoader_size           alignment,
        void                   ** ppMemory
);

static inline MeshLoader_Result MeshLoader_Job_reallocateMemory (
        MeshLoader_Job_Context    context,
        void                    * pOldMemory,
        MeshLoader_size           newSize,
        void                   ** ppMemory
) {
    return MeshLoader_Job_reallocateMemory2 (
            context,
            pOldMemory,
            newSize,
            1U,
            ppMemory
    );
}

extern MeshLoader_Result MeshLoader_Job_freeMemory (
        MeshLoader_Job_Context    jobContext,
        void                    * pMemory
);

extern MeshLoader_Result MeshLoader_Job_releaseMemory (
        MeshLoader_Job_Context    jobContext,
        void                    * pMemory
);

extern MeshLoader_Result MeshLoader_Job_setMeshVertexData (
        MeshLoader_Job_Context          jobContext,
        MeshLoader_uint32               vertexCount,
        MeshLoader_VertexData   const * pVertices
);

extern MeshLoader_Result MeshLoader_Job_setMeshFaceData (
        MeshLoader_Job_Context          jobContext,
        MeshLoader_uint32               faceCount,
        MeshLoader_FaceData     const * pFaces
);

extern MeshLoader_Result MeshLoader_Job_setMeshIndexData (
        MeshLoader_Job_Context          jobContext,
        MeshLoader_IndexData    const * pIndexData
);

typedef MeshLoader_Result ( * MeshLoader_Job_GetProgressFunction ) (
        MeshLoader_Job_Context    jobContext,
        float                   * pProgress
);

typedef MeshLoader_Result ( * MeshLoader_Job_SetProgressFunction ) (
        MeshLoader_Job_Context    jobContext,
        float                     progress
);

typedef MeshLoader_Result ( * MeshLoader_Job_GetUserDataFunction ) (
        MeshLoader_Job_Context    jobContext,
        void                   ** ppUserData
);

typedef MeshLoader_Result ( * MeshLoader_Job_GetDataFromPreviousCallFunction ) (
        MeshLoader_Job_Context    jobContext,
        void                   ** ppData
);

typedef MeshLoader_Result ( * MeshLoader_Job_SetDataForNextCallFunction ) (
        MeshLoader_Job_Context    jobContext,
        void                    * pData
);

typedef MeshLoader_Result ( * MeshLoader_Job_GetLoadModeFunction ) (
        MeshLoader_Job_Context          jobContext,
        MeshLoader_MeshLoadModeFlags  * pLoadMode
);

typedef MeshLoader_Result ( * MeshLoader_Job_GetInputPathFunction ) (
        MeshLoader_Job_Context      jobContext,
        MeshLoader_StringLiteral  * pPath
);

typedef MeshLoader_Result ( * MeshLoader_Job_FinishFunction ) (
        MeshLoader_Job_Context      jobContext
);

typedef MeshLoader_Result ( * MeshLoader_Job_AllocateMemory2Function ) (
        MeshLoader_Job_Context    jobContext,
        MeshLoader_size           size,
        MeshLoader_size           alignment,
        void                   ** ppMemory
);

typedef MeshLoader_Result ( * MeshLoader_Job_AllocateMemoryFunction ) (
        MeshLoader_Job_Context      context,
        MeshLoader_size             size,
        void                     ** ppMemory
);

typedef MeshLoader_Result ( * MeshLoader_Job_ReallocateMemory2Function ) (
        MeshLoader_Job_Context    jobContext,
        void                    * pOldMemory,
        MeshLoader_size           size,
        MeshLoader_size           alignment,
        void                   ** ppMemory
);

typedef MeshLoader_Result ( * MeshLoader_Job_ReallocateMemoryFunction ) (
        MeshLoader_Job_Context    context,
        void                    * pOldMemory,
        MeshLoader_size           newSize,
        void                   ** ppMemory
);

typedef MeshLoader_Result ( * MeshLoader_Job_FreeMemoryFunction ) (
        MeshLoader_Job_Context    jobContext,
        void                    * pMemory
);

typedef MeshLoader_Result ( * MeshLoader_Job_ReleaseMemoryFunction ) (
        MeshLoader_Job_Context    jobContext,
        void                    * pMemory
);

typedef MeshLoader_Result ( * MeshLoader_Job_SetMeshVertexDataFunction ) (
        MeshLoader_Job_Context          jobContext,
        MeshLoader_uint32               vertexCount,
        MeshLoader_VertexData   const * pVertices
);

typedef MeshLoader_Result ( * MeshLoader_Job_SetMeshFaceDataFunction ) (
        MeshLoader_Job_Context          jobContext,
        MeshLoader_uint32               faceCount,
        MeshLoader_FaceData     const * pFaces
);

typedef MeshLoader_Result ( * MeshLoader_Job_SetMeshIndexDataFunction ) (
        MeshLoader_Job_Context          jobContext,
        MeshLoader_IndexData    const * pIndexData
);

#endif // __MESH_LOADER_CUSTOM_JOB_H__
