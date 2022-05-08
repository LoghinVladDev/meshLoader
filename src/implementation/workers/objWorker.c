//
// Created by loghin on 5/8/22.
//

#include "objWorker.h"

static MeshLoader_CustomJobInfo const __MeshLoader_JobWorker_defaultObjWorkerInfo = {
        .structureType      = MeshLoader_StructureType_CustomJobInfo,
        .pNext              = NULL,
        .pUserData          = NULL,
        .jobFunction        = & __MeshLoader_JobWorker_objWorkerMainFunction
};

MeshLoader_CustomJobInfo const * __MeshLoader_JobWorker_getDefaultObjWorkerInfo () {
    return & __MeshLoader_JobWorker_defaultObjWorkerInfo;
}

MeshLoader_Result __MeshLoader_JobWorker_objWorkerMainFunction (
        MeshLoader_Job_Context                  context
) {

    MeshLoader_Result result;

    float progress;

    result = MeshLoader_Job_getProgress (
            context,
            & progress
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    if ( progress < .9999995f ) {
        progress += .0000005f;
    } else {
        progress = 1.0f;

        result = MeshLoader_Job_finish (
                context
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }
    }

    result = MeshLoader_Job_setProgress (
            context,
            progress
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    return MeshLoader_Result_Success;
}