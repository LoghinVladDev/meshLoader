#include <meshLoader/meshLoader>
#include <stdio.h>

int main() {

    MeshLoader_Result               result;

    MeshLoader_Instance             instance;
    MeshLoader_InstanceCreateInfo   instanceCreateInfo;

    instanceCreateInfo.structureType            = MeshLoader_StructureType_InstanceCreateInfo;
    instanceCreateInfo.pNext                    = NULL;
    instanceCreateInfo.maxWorkerThreadCount     = 8U;
    instanceCreateInfo.flags                    = MeshLoader_nullFlags;

    result = MeshLoader_createInstance (
            & instanceCreateInfo,
            & instance,
            NULL
    );

    if ( result != MeshLoader_Result_Success ) {
        fprintf ( stderr, "Failed to create MeshLoader Instance" );
        return 1;
    }

    MeshLoader_CreateJobInfo jobInfos [] = {
            {
                    .structureType  = MeshLoader_StructureType_CreateJobInfo,
                    .pNext          = NULL,
                    .inputPath      = "../tests/data/baseTest/cow.obj",
                    .priority       = 1.0f
            }, {
                    .structureType  = MeshLoader_StructureType_CreateJobInfo,
                    .pNext          = NULL,
                    .inputPath      = "../tests/data/baseTest/teapot.obj",
                    .priority       = .5f
            }, {
                    .structureType  = MeshLoader_StructureType_CreateJobInfo,
                    .pNext          = NULL,
                    .inputPath      = "../tests/data/baseTest/teapot.obj",
                    .priority       = .5f
            }
    };

    int const jobCount = 3;
    MeshLoader_Job jobs[jobCount];

    MeshLoader_JobsCreateInfo jobsCreateInfo = {
            .structureType      = MeshLoader_StructureType_JobsCreateInfo,
            .pNext              = NULL,
            .flags              = MeshLoader_nullFlags,
            .loadMode           = MeshLoader_MeshLoadModeFlag_LoadFaces | MeshLoader_MeshLoadModeFlag_LoadIndices,
            .jobCount           = jobCount,
            .pJobs              = & jobs[0],
            .pCreateJobInfos    = & jobInfos[0]
    };

    result = MeshLoader_createJobs (
            instance,
            & jobsCreateInfo,
            NULL
    );

    if ( result != MeshLoader_Result_Success ) {
        MeshLoader_destroyInstance ( instance, NULL );
        fprintf ( stderr, "Failed to create MeshLoader Jobs" );
        return 1;
    }

    MeshLoader_destroyJobs ( instance, jobCount, & jobs[0], NULL );
    MeshLoader_destroyInstance ( instance, NULL );

    return 0;
}
