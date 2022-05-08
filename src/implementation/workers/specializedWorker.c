//
// Created by loghin on 5/8/22.
//

#include "specializedWorker.h"
#include "objWorker.h"

MeshLoader_CustomJobInfo const * __MeshLoader_SpecializedWorker_getInfo (
        MeshLoader_JobType type
) {

    switch ( type ) {
        case MeshLoader_JobType_Obj:
            return __MeshLoader_JobWorker_getDefaultObjWorkerInfo ();

        default:
            return NULL;
    }
}
