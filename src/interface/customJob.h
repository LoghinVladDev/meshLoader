//
// Created by loghin on 5/8/22.
//

#ifndef __MESH_LOADER_CUSTOM_JOB_H__
#define __MESH_LOADER_CUSTOM_JOB_H__

#include <meshLoader/publicTypes>

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    void                              * pUserData;
    MeshLoader_MeshLoadModeFlags        loadMode;
    MeshLoader_StringLiteral            inputPath;
    float                               progress;
    MeshLoader_JobStatus                status;
} MeshLoader_JobData;

extern MeshLoader_Result MeshLoader_Job_getProgress (
        MeshLoader_Job,
        float *
);

typedef MeshLoader_Result ( * MeshLoader_JobMainFunction ) (
        MeshLoader_JobData *
);

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    void                              * pUserData;
    MeshLoader_JobMainFunction          jobFunction;
} MeshLoader_CustomJobInfo;

#endif // __MESH_LOADER_CUSTOM_JOB_H__
