//
// Created by loghin on 5/4/22.
//

#include "job.h"
#include <memory.h>
#include <meshLoader/customJob>
#include <stdalign.h>
#include "workers/specializedWorker.h"

MeshLoader_Result __MeshLoader_Job_construct (
        MeshLoader_Job                          job,
        MeshLoader_CreateJobInfo        const * pCreateInfo,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    MeshLoader_Result                   result;
    MeshLoader_BaseInStructure  const * pNextStructure = ( MeshLoader_BaseInStructure const * ) pCreateInfo->pNext;

    __MeshLoader_ScopedAllocationCallbacks scopedAllocationCallbacks = {
            .pAllocationCallbacks       = pAllocationCallbacks,
            .allocationScope            = MeshLoader_SystemAllocationScope_Object,
            .explicitAllocationPurpose  = NULL
    };

    result = __MeshLoader_String_createFromStringLiteral (
            & job->context.inputPath,
            pCreateInfo->inputPath,
            & scopedAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    result = __MeshLoader_Mutex_create (
            & job->jobLock,
            & scopedAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        __MeshLoader_String_destroy (
                & job->context.inputPath,
                & scopedAllocationCallbacks
        );

        return result;
    }

    result = __MeshLoader_Mesh_construct (
            & job->context.mesh,
            pAllocationCallbacks
    );

    if ( result != MeshLoader_Result_Success ) {
        __MeshLoader_Mutex_destroy (
                job->jobLock,
                & scopedAllocationCallbacks
        );

        __MeshLoader_String_destroy (
                & job->context.inputPath,
                & scopedAllocationCallbacks
        );
    }

    while ( pNextStructure != NULL ) {

        switch ( pNextStructure->structureType ) {
            case MeshLoader_StructureType_CustomJobInfo:
                if ( pCreateInfo->jobType == MeshLoader_JobType_Custom ) {
                    result = __MeshLoader_Job_constructCustomJobInfo (
                            job,
                            ( MeshLoader_CustomJobInfo const * ) pNextStructure,
                            pAllocationCallbacks
                    );
                }

                break;

            default:
                break;
        }

        if ( result != MeshLoader_Result_Success ) {
            __MeshLoader_Mesh_destruct (
                    & job->context.mesh,
                    pAllocationCallbacks
            );

            __MeshLoader_Mutex_destroy (
                    job->jobLock,
                    & scopedAllocationCallbacks
            );

            __MeshLoader_String_destroy (
                    & job->context.inputPath,
                    & scopedAllocationCallbacks
            );

            return result;
        }

        pNextStructure = pNextStructure->pNext;
    }

    job->priority               = pCreateInfo->priority;
    job->context.loadMode       = pCreateInfo->loadMode;
    job->context.jobProgress    = MESH_LOADER_JOB_PROGRESS_MIN_VALUE;
    job->context.jobState       = ( MeshLoader_uint8 ) MeshLoader_JobState_Ready;
    job->context.pNextCallData  = NULL;
    job->jobType                = pCreateInfo->jobType;

    if ( job->jobType != MeshLoader_JobType_Custom ) {
        job->context.pCustomJobInfo = __MeshLoader_SpecializedWorker_getInfo ( job->jobType );
    }

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Job_constructCustomJobInfo (
        MeshLoader_Job                          job,
        MeshLoader_CustomJobInfo        const * pCustomJobInfo,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = sizeof ( MeshLoader_CustomJobInfo ),
            .alignment              = alignof ( MeshLoader_CustomJobInfo ),
            .allocationScope        = MeshLoader_SystemAllocationScope_Object,
            .explicitMemoryPurpose  = "Creates a Custom Job Info addon for a Job Object"
    };

    allocationNotification.pMemory = pAllocationCallbacks->allocationFunction (
            pAllocationCallbacks->pUserData,
            allocationNotification.size,
            allocationNotification.alignment,
            allocationNotification.allocationScope
    );

    if ( allocationNotification.pMemory == NULL ) {
        return MeshLoader_Result_OutOfMemory;
    }

    if ( pAllocationCallbacks->internalAllocationNotificationFunction != NULL ) {
        pAllocationCallbacks->internalAllocationNotificationFunction (
                pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    * ( ( MeshLoader_CustomJobInfo * ) allocationNotification.pMemory ) = ( MeshLoader_CustomJobInfo ) {
        .structureType              = MeshLoader_StructureType_CustomJobInfo,
        .pNext                      = NULL,
        .pUserData                  = pCustomJobInfo->pUserData,
        .jobFunction                = pCustomJobInfo->jobFunction
    };

    return MeshLoader_Result_Success;
}

void __MeshLoader_Job_destruct (
        MeshLoader_Job                          job,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {
    __MeshLoader_ScopedAllocationCallbacks scopedAllocationCallbacks = {
            .pAllocationCallbacks       = pAllocationCallbacks,
            .allocationScope            = MeshLoader_SystemAllocationScope_Object,
            .explicitAllocationPurpose  = NULL
    };

    if ( job == MeshLoader_invalidHandle ) {
        return;
    }

    if (
            job->jobType                == MeshLoader_JobType_Custom &&
            job->context.pCustomJobInfo != NULL
    ) {
        MeshLoader_AllocationNotification allocationNotification = {
                .structureType          = MeshLoader_StructureType_AllocationNotification,
                .pNext                  = NULL,
                .pMemory                = (void *) ( ( MeshLoader_size ) job->context.pCustomJobInfo ),
                .pOldMemory             = NULL,
                .size                   = sizeof ( MeshLoader_CustomJobInfo ),
                .alignment              = alignof ( MeshLoader_CustomJobInfo ),
                .allocationScope        = MeshLoader_SystemAllocationScope_Object,
                .explicitMemoryPurpose  = "Destroys a Custom Job Info addon for a Job Object"
        };

        if ( pAllocationCallbacks->internalFreeNotificationFunction != NULL ) {
            pAllocationCallbacks->internalFreeNotificationFunction (
                    pAllocationCallbacks->pUserData,
                    & allocationNotification
            );
        }

        pAllocationCallbacks->freeFunction (
                pAllocationCallbacks->pUserData,
                allocationNotification.pMemory
        );
    }

    __MeshLoader_Mesh_destruct (
            & job->context.mesh,
            pAllocationCallbacks
    );

    __MeshLoader_Mutex_destroy (
            job->jobLock,
            & scopedAllocationCallbacks
    );

    __MeshLoader_String_destroy (
            & job->context.inputPath,
            & scopedAllocationCallbacks
    );
}

MeshLoader_Result MeshLoader_Job_getProgress (
        MeshLoader_Job_Context  jobContext,
        float                 * pProgress
) {

    * pProgress = jobContext->progress;
    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_Job_setProgress (
        MeshLoader_Job_Context  jobContext,
        float                   progress
) {

    jobContext->progress = progress;
    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_Job_finish (
        MeshLoader_Job_Context  jobContext
) {

    jobContext->state = MeshLoader_JobState_Finished;
    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_Job_getUserData (
        MeshLoader_Job_Context    jobContext,
        void                   ** ppUserData
) {

    * ppUserData = jobContext->pUserData;
    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_Job_getDataFromPreviousCall (
        MeshLoader_Job_Context    jobContext,
        void                   ** ppData
) {

    * ppData = jobContext->pNextCallData;
    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_Job_setDataForNextCall (
        MeshLoader_Job_Context    jobContext,
        void                    * pData
) {

    jobContext->pNextCallData = pData;
    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_Job_getLoadMode (
        MeshLoader_Job_Context          jobContext,
        MeshLoader_MeshLoadModeFlags  * pLoadMode
) {

    * pLoadMode = jobContext->loadMode;
    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_Job_getInputPath (
        MeshLoader_Job_Context        jobContext,
        MeshLoader_StringLiteral    * pInputPath
) {

    * pInputPath = jobContext->inputPath;
    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_Job_setMeshVertexData (
        MeshLoader_Job_Context          jobContext,
        MeshLoader_uint32               vertexCount,
        MeshLoader_VertexData   const * pVertices
) {

    jobContext->mesh->data.pVertices    = pVertices;
    jobContext->mesh->data.vertexCount  = vertexCount;

    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_Job_setMeshFaceData (
        MeshLoader_Job_Context          jobContext,
        MeshLoader_uint32               faceCount,
        MeshLoader_FaceData     const * pFaces
) {

    jobContext->mesh->data.pFaces       = pFaces;
    jobContext->mesh->data.faceCount    = faceCount;

    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_Job_setMeshIndexData (
        MeshLoader_Job_Context          jobContext,
        MeshLoader_IndexData    const * pIndexData
) {

    jobContext->mesh->data.pIndexData   = pIndexData;

    return MeshLoader_Result_Success;
}
