//
// Created by loghin on 5/8/22.
//

#ifndef __MESH_LOADER_OBJ_WORKER_H__
#define __MESH_LOADER_OBJ_WORKER_H__

#include <meshLoader/publicTypes>
#include <meshLoader/customJob>

typedef enum {
    __MeshLoader_Worker_ObjWorker_State_Initialization  = 0x00000001U,
    __MeshLoader_Worker_ObjWorker_State_Finished        = 0x00001000U,
    __MeshLoader_Worker_ObjWorker_State_Error           = 0x00002000U,
} __MeshLoader_Worker_ObjWorker_State;

typedef struct __MeshLoader_Worker_ObjWorker_Control * __MeshLoader_Worker_ObjWorker_Control;

typedef MeshLoader_Result ( * __MeshLoader_Worker_ObjWorker_StateFunction ) (
        MeshLoader_Job_Context,
        __MeshLoader_Worker_ObjWorker_Control
);

struct __MeshLoader_Worker_ObjWorker_Control {
    __MeshLoader_Worker_ObjWorker_State         state;
    __MeshLoader_Worker_ObjWorker_StateFunction stateFunction;
};

extern MeshLoader_CustomJobInfo const * __MeshLoader_Worker_ObjWorker_getInfo ();

extern MeshLoader_Result __MeshLoader_Worker_ObjWorker_mainFunction (
        MeshLoader_Job_Context
);

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_allocateResources (
        MeshLoader_Job_Context,
        __MeshLoader_Worker_ObjWorker_Control *
);

static void __MeshLoader_Worker_ObjWorker_freeResources (
        MeshLoader_Job_Context,
        __MeshLoader_Worker_ObjWorker_Control
);

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_initializationState (
        MeshLoader_Job_Context,
        __MeshLoader_Worker_ObjWorker_Control
);

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_finishedState (
        MeshLoader_Job_Context,
        __MeshLoader_Worker_ObjWorker_Control
);

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_errorState (
        MeshLoader_Job_Context,
        __MeshLoader_Worker_ObjWorker_Control
);

#endif // __MESH_LOADER_OBJ_WORKER_H__
