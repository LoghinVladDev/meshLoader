//
// Created by loghin on 5/8/22.
//

#include <stdalign.h>
#include "objWorker.h"

static MeshLoader_CustomJobInfo const __MeshLoader_JobWorker_ObjWorker_info = {
        .structureType      = MeshLoader_StructureType_CustomJobInfo,
        .pNext              = NULL,
        .pUserData          = NULL,
        .jobFunction        = & __MeshLoader_Worker_ObjWorker_mainFunction
};

MeshLoader_CustomJobInfo const * __MeshLoader_Worker_ObjWorker_getInfo () {
    return & __MeshLoader_JobWorker_ObjWorker_info;
}

static inline __MeshLoader_Worker_ObjWorker_StateFunction __MeshLoader_Worker_ObjWorker_getStateFunction (
        __MeshLoader_Worker_ObjWorker_State state
) {

    switch ( state ) {
        case __MeshLoader_Worker_ObjWorker_State_Initialization:    return & __MeshLoader_Worker_ObjWorker_initializationState;
        case __MeshLoader_Worker_ObjWorker_State_Finished:          return & __MeshLoader_Worker_ObjWorker_finishedState;
        case __MeshLoader_Worker_ObjWorker_State_Error:             return & __MeshLoader_Worker_ObjWorker_errorState;
        default:                                                    return NULL;
    }
}

MeshLoader_Result __MeshLoader_Worker_ObjWorker_mainFunction (
        MeshLoader_Job_Context                  context
) {

    MeshLoader_Result result;
    __MeshLoader_Worker_ObjWorker_Control control;

    result = MeshLoader_Job_getDataFromPreviousCall (
            context,
            (void **) & control
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    if ( control == NULL ) {
        result = __MeshLoader_Worker_ObjWorker_allocateResources (
                context,
                & control
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }

        control->state = __MeshLoader_Worker_ObjWorker_State_Initialization;
    }

    control->stateFunction = __MeshLoader_Worker_ObjWorker_getStateFunction (
            control->state
    );

    if ( control->stateFunction == NULL ) {
        control->state          = __MeshLoader_Worker_ObjWorker_State_Error;
        control->stateFunction  = __MeshLoader_Worker_ObjWorker_getStateFunction (
                control->state
        );
    }

    result = control->stateFunction (
            context,
            control
    );

    if ( result != MeshLoader_Result_Success ) {
        __MeshLoader_Worker_ObjWorker_freeResources (
                context,
                control
        );

        return result;
    }

    if ( control->state == __MeshLoader_Worker_ObjWorker_State_Finished ) {
        __MeshLoader_Worker_ObjWorker_freeResources (
                context,
                control
        );

        result = MeshLoader_Job_finish (
                context
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }

        result = MeshLoader_Job_setProgress (
                context,
                1.0f
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }
    } else {

        result = MeshLoader_Job_setDataForNextCall (
                context,
                control
        );

        if ( result != MeshLoader_Result_Success ) {
            __MeshLoader_Worker_ObjWorker_freeResources (
                    context,
                    control
            );

            return result;
        }
    }

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_allocateResources (
        MeshLoader_Job_Context                      context,
        __MeshLoader_Worker_ObjWorker_Control     * pControl
) {

    MeshLoader_Result result;

    result = MeshLoader_Job_allocateMemory2 (
            context,
            sizeof ( struct __MeshLoader_Worker_ObjWorker_Control ),
            alignof ( struct __MeshLoader_Worker_ObjWorker_Control ),
            (void **) pControl
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    return result;
}

static void __MeshLoader_Worker_ObjWorker_freeResources (
        MeshLoader_Job_Context                      context,
        __MeshLoader_Worker_ObjWorker_Control       control
) {

    (void) MeshLoader_Job_freeMemory (
            context,
            control
    );
}

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_initializationState (
        MeshLoader_Job_Context                  context,
        __MeshLoader_Worker_ObjWorker_Control   control
) {

    control->state = __MeshLoader_Worker_ObjWorker_State_Finished;
    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_finishedState (
        MeshLoader_Job_Context                  context,
        __MeshLoader_Worker_ObjWorker_Control   control
) {

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_errorState (
        MeshLoader_Job_Context                  context,
        __MeshLoader_Worker_ObjWorker_Control   control
) {

    return MeshLoader_Result_Success;
}