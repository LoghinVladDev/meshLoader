//
// Created by loghin on 5/6/22.
//

#include "jobDispatch.h"
#include "instance.h"
#include "job.h"

extern MeshLoader_Result MeshLoader_startJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsStartInfo        const * pStartInfo
) {
    (void) instance;
    (void) pStartInfo;

    return MeshLoader_Result_Success;
}

extern MeshLoader_Result MeshLoader_queryJobs (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsQueryInfo              * pQueryInfo
) {
    (void) instance;
    (void) pQueryInfo;

    return MeshLoader_Result_Success;
}

extern MeshLoader_Result MeshLoader_getResults (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsGetResultInfo          * pResults
) {
    (void) instance;
    (void) pResults;

    return MeshLoader_Result_Success;
}

extern MeshLoader_Result MeshLoader_anyJobsRunning (
        MeshLoader_Instance                     instance,
        MeshLoader_bool                       * pAnyRunning
) {

    (void) instance;
    (void) pAnyRunning;

    return MeshLoader_Result_Success;
}
