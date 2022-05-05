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
                    .loadMode       = MeshLoader_MeshLoadModeFlag_LoadFaces | MeshLoader_MeshLoadModeFlag_LoadIndices,
                    .inputPath      = "../tests/data/baseTest/cow.obj",
                    .priority       = 1.0f
            }, {
                    .structureType  = MeshLoader_StructureType_CreateJobInfo,
                    .pNext          = NULL,
                    .loadMode       = MeshLoader_MeshLoadModeFlag_LoadFaces | MeshLoader_MeshLoadModeFlag_LoadIndices,
                    .inputPath      = "../tests/data/baseTest/teapot.obj",
                    .priority       = .5f
            }, {
                    .structureType  = MeshLoader_StructureType_CreateJobInfo,
                    .pNext          = NULL,
                    .loadMode       = MeshLoader_MeshLoadModeFlag_LoadFaces | MeshLoader_MeshLoadModeFlag_LoadIndices,
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

    MeshLoader_JobsStartInfo startInfo = {
            .structureType  = MeshLoader_StructureType_JobsStartInfo,
            .pNext          = NULL,
            .flags          = MeshLoader_nullFlags,
            .jobCount       = jobCount,
            .pJobs          = & jobs[0]
    };

    MeshLoader_QueryJobInfo queryInfos [jobCount];
    for ( int i = 0; i < jobCount; ++ i ) {
        queryInfos[i].structureType = MeshLoader_StructureType_QueryJobInfo;
        queryInfos[i].pNext         = NULL;
    }

    MeshLoader_JobsQueryInfo queryInfo = {
            .structureType  = MeshLoader_StructureType_JobsQueryInfo,
            .pNext          = NULL,
            .flags          = MeshLoader_nullFlags,
            .jobCount       = jobCount,
            .pQueryJobInfos = & queryInfos[0]
    };

    MeshLoader_bool anyRunning = MeshLoader_false;

    result = MeshLoader_startJobs (
            instance,
            & startInfo
    );

    if ( result != MeshLoader_Result_Success ) {
        fprintf ( stderr, "Failed to start jobs" );
        goto end;
    }

    do {

        result = MeshLoader_queryJobs ( instance, & queryInfo );

        if ( result != MeshLoader_Result_Success ) {
            fprintf ( stderr, "Error occurred while querying jobs status" );
            break;
        }

        result = MeshLoader_anyJobsRunning ( instance, & anyRunning );

        if ( result != MeshLoader_Result_Success ) {
            fprintf ( stderr, "Error occurred while waiting for jobs to finish" );
            break;
        }

    } while ( anyRunning );

end:
    MeshLoader_destroyJobs ( instance, jobCount, & jobs[0], NULL );
    MeshLoader_destroyInstance ( instance, NULL );

    return 0;
}
