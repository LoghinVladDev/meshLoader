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

extern MeshLoader_Result MeshLoader_Job_finish (
        MeshLoader_Job_Context
);

extern MeshLoader_Result MeshLoader_Job_allocateMemory2 (
        MeshLoader_Job_Context,
        MeshLoader_size,
        MeshLoader_size,
        void                   **
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
        MeshLoader_Job_Context,
        void                    *,
        MeshLoader_size,
        MeshLoader_size,
        void                   **
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
        MeshLoader_Job_Context,
        void                    *
);

#endif // __MESH_LOADER_CUSTOM_JOB_H__
