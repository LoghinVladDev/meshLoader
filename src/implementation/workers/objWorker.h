//
// Created by loghin on 5/8/22.
//

#ifndef __MESH_LOADER_OBJ_WORKER_H__
#define __MESH_LOADER_OBJ_WORKER_H__

#include <meshLoader/publicTypes>
#include <meshLoader/customJob>

extern MeshLoader_CustomJobInfo const * __MeshLoader_JobWorker_getDefaultObjWorkerInfo ();

extern MeshLoader_Result __MeshLoader_JobWorker_objWorkerMainFunction (
        MeshLoader_Job_Context
);

#endif // __MESH_LOADER_OBJ_WORKER_H__
