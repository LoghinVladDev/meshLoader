#include <meshLoader/meshLoader>
#include <stdio.h>

static void dumpMesh (
        MeshLoader_MeshData const * pMeshData,
        MeshLoader_StringLiteral    fileName
) {

    FILE * outFile = fopen ( fileName, "w" );

    if ( outFile == NULL ) {
        return;
    }

    for ( unsigned int i = 0; i < pMeshData->vertexCount; ++ i ) {
        fprintf ( outFile, "v %lf %lf %lf\n", pMeshData->pVertices[i].x, pMeshData->pVertices[i].y, pMeshData->pVertices[i].z );
    }

    for ( unsigned int i = 0; i < pMeshData->faceCount; ++ i ) {
        fprintf ( outFile, "f %u %u %u\n", pMeshData->pFaces[i].u, pMeshData->pFaces[i].v, pMeshData->pFaces[i].w );
    }

    fclose ( outFile );
}

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
            NULL,
            & instance
    );

    if ( result != MeshLoader_Result_Success ) {
        fprintf ( stderr, "Failed to create MeshLoader Instance" );
        return 1;
    }

    MeshLoader_CreateJobInfo jobInfos [] = {
            {
                    .structureType  = MeshLoader_StructureType_CreateJobInfo,
                    .pNext          = NULL,
                    .jobType        = MeshLoader_JobType_Obj,
                    .loadMode       = MeshLoader_MeshLoadModeFlag_LoadFaces | MeshLoader_MeshLoadModeFlag_LoadIndices,
                    .inputPath      = "../src/tests/data/baseTest/cow.obj",
                    .priority       = 1.0f
            }, {
                    .structureType  = MeshLoader_StructureType_CreateJobInfo,
                    .pNext          = NULL,
                    .jobType        = MeshLoader_JobType_Obj,
                    .loadMode       = MeshLoader_MeshLoadModeFlag_LoadFaces | MeshLoader_MeshLoadModeFlag_LoadIndices,
                    .inputPath      = "../src/tests/data/baseTest/teapot.obj",
                    .priority       = .7f
            }, {
                    .structureType  = MeshLoader_StructureType_CreateJobInfo,
                    .pNext          = NULL,
                    .jobType        = MeshLoader_JobType_Obj,
                    .loadMode       = MeshLoader_MeshLoadModeFlag_LoadFaces | MeshLoader_MeshLoadModeFlag_LoadIndices,
                    .inputPath      = "../src/tests/data/baseTest/pumpkin.obj",
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
            .structureType          = MeshLoader_StructureType_JobsStartInfo,
            .pNext                  = NULL,
            .flags                  = MeshLoader_nullFlags,
            .jobCount               = jobCount,
            .pJobs                  = & jobs[0],
            .pAllocationCallbacks   = NULL
    };

    MeshLoader_QueryJobInfo queryInfos [jobCount];

    for ( int i = 0; i < jobCount; ++ i ) {
        queryInfos[i].structureType = MeshLoader_StructureType_QueryJobInfo;
        queryInfos[i].pNext         = NULL;
        queryInfos[i].job           = jobs [i];
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

    MeshLoader_Mesh         meshes [jobCount];
    MeshLoader_MeshData     meshDataArray [jobCount];

    if ( result != MeshLoader_Result_Success ) {
        fprintf ( stderr, "Failed to start jobs" );
        goto end;
    }

    int pausedJobCount = 0;
    int pausedJobs [] = {0, 0, 0};
    do {

        result = MeshLoader_queryJobs ( instance, & queryInfo );

        if ( result != MeshLoader_Result_Success ) {
            fprintf ( stderr, "Error occurred while querying jobs status" );
            break;
        }

        for ( int i = 0; i < jobCount; ++ i ) {

            if (queryInfos [i].progress >= 0.5f && queryInfos->state != MeshLoader_JobState_Paused) {
                MeshLoader_JobsPauseInfo pauseInfo;
                pauseInfo.jobCount = 1U;
                pauseInfo.pJobs = & queryInfos[i].job;
                pauseInfo.structureType = MeshLoader_StructureType_JobsPauseInfo;
                pauseInfo.pNext = NULL;
                pauseInfo.flags = MeshLoader_nullFlags;

                result = MeshLoader_pauseJobs ( instance, & pauseInfo );
                if ( result != MeshLoader_Result_Success ) {
                    fprintf (stderr, "Error Occurred while pausing jobs\n");
                    goto end;
                }

                pausedJobCount ++;
            }

            fprintf (
                    stdout,
                    "Job %d progress : %.2f%%\n",
                    i,
                    queryInfos[i].progress * 100.0f
            );

            fflush(stdout);
        }

        if (pausedJobCount == jobCount) {
            break;
        }

        result = MeshLoader_anyJobsRunning ( instance, & anyRunning );

        if ( result != MeshLoader_Result_Success ) {
            fprintf ( stderr, "Error occurred while waiting for jobs to finish" );
            break;
        }

    } while ( anyRunning );

    MeshLoader_JobsResumeInfo resumeInfo = {
            .structureType  = MeshLoader_StructureType_JobsResumeInfo,
            .pNext          = NULL,
            .flags          = MeshLoader_nullFlags,
            .jobCount       = jobCount,
            .pJobs          = & jobs [0U]
    };

    result = MeshLoader_resumeJobs (instance, & resumeInfo);

    if (result != MeshLoader_Result_Success) {
        fprintf ( stderr, "Error occurred while resuming jobs" );
        goto end;
    }

    do {

        result = MeshLoader_queryJobs ( instance, & queryInfo );

        if ( result != MeshLoader_Result_Success ) {
            fprintf ( stderr, "Error occurred while querying jobs status" );
            break;
        }

        for ( int i = 0; i < jobCount; ++ i ) {

            fprintf (
                    stdout,
                    "Job %d progress : %.2f%%\n",
                    i,
                    queryInfos[i].progress * 100.0f
            );

            fflush(stdout);
        }

        result = MeshLoader_anyJobsRunning ( instance, & anyRunning );

        if ( result != MeshLoader_Result_Success ) {
            fprintf ( stderr, "Error occurred while waiting for jobs to finish" );
            break;
        }

    } while ( anyRunning );

    for ( int i = 0; i < jobCount; ++ i ) {
        result = MeshLoader_takeMesh (
                jobs [i],
                NULL,
                & meshes [i]
        );

        if ( result != MeshLoader_Result_Success ) {
            fprintf ( stderr, "Failed to Acquire Mesh from Job" );
            goto end;
        }

        meshDataArray->structureType    = MeshLoader_StructureType_MeshData;
        meshDataArray[i].pNext = NULL;

        result = MeshLoader_getMeshData (
                meshes [i],
                & meshDataArray [i]
        );

        if ( result != MeshLoader_Result_Success ) {
            fprintf ( stderr, "Failed to Acquire Mesh Data from Mesh" );
            goto end;
        }
    }

    dumpMesh ( & meshDataArray[0], "cow.obj" );
    dumpMesh ( & meshDataArray[1], "teapot.obj" );
    dumpMesh ( & meshDataArray[2], "pumpkin.obj" );

    for ( int i = 0; i < jobCount; ++ i ) {
        MeshLoader_destroyMesh ( meshes [i], NULL );
    }

end:
    MeshLoader_destroyJobs ( instance, jobCount, & jobs[0], NULL );
    MeshLoader_destroyInstance ( instance, NULL );

    return 0;
}
