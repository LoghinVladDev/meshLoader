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
        MeshLoader_Job_Context    jobData,
        float                   * pProgress
);

extern MeshLoader_Result MeshLoader_Job_setProgress (
        MeshLoader_Job_Context    jobData,
        float                     progress
);

extern MeshLoader_Result MeshLoader_Job_finish (
        MeshLoader_Job_Context
);

#endif // __MESH_LOADER_CUSTOM_JOB_H__
