//
// Created by loghin on 5/8/22.
//

#include <stdalign.h>
#include "objWorker.h"
#include <string.h>
#include <stdlib.h>
#include "../privateUtility.h"

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
        case __MeshLoader_Worker_ObjWorker_State_ReadAndDigest:     return & __MeshLoader_Worker_ObjWorker_readAndDigestState;
        case __MeshLoader_Worker_ObjWorker_State_StoreMeshData:     return & __MeshLoader_Worker_ObjWorker_storeMeshDataState;
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

        control->nextState = __MeshLoader_Worker_ObjWorker_State_Initialization;
    }

    control->state = control->nextState;
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
            ( void ** ) pControl
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

#if MESH_LOADER_JOB_STARTING_VERTEX_BUFFER_SIZE > 0U

    result = MeshLoader_Job_allocateMemory2 (
            context,
            MESH_LOADER_JOB_STARTING_VERTEX_BUFFER_SIZE * sizeof ( MeshLoader_VertexData ),
            alignof ( MeshLoader_VertexData ),
            ( void ** ) & ( * pControl )->pVertices
    );

    if ( result != MeshLoader_Result_Success ) {
        (void) MeshLoader_Job_freeMemory (
                context,
                pControl
        );

        return result;
    }

    ( * pControl )->vertexCapacity  = MESH_LOADER_JOB_STARTING_VERTEX_BUFFER_SIZE;
    ( * pControl )->vertexCount     = 0U;

#else

    ( * pControl )->pVertices       = NULL;
    ( * pControl )->vertexCapacity  = 0U;
    ( * pControl )->vertexCount     = 0U;

#endif

#if MESH_LOADER_JOB_STARTING_FACE_BUFFER_SIZE > 0U

    result = MeshLoader_Job_allocateMemory2 (
            context,
            MESH_LOADER_JOB_STARTING_FACE_BUFFER_SIZE * sizeof ( MeshLoader_FaceData ),
            alignof ( MeshLoader_FaceData ),
            ( void ** ) & ( * pControl )->pFaces
    );

    if ( result != MeshLoader_Result_Success ) {

        if ( ( * pControl )->pVertices != NULL ) {
            (void) MeshLoader_Job_freeMemory (
                    context,
                    ( * pControl )->pVertices
            );
        }

        (void) MeshLoader_Job_freeMemory (
                context,
                pControl
        );

        return result;
    }

    ( * pControl )->faceCapacity    = MESH_LOADER_JOB_STARTING_FACE_BUFFER_SIZE;
    ( * pControl )->faceCount       = 0U;

#else

    ( * pControl )->faceCapacity    = 0U;
    ( * pControl )->faceCount       = 0U;
    ( * pControl )->pFaces          = NULL;

#endif

    return result;
}

static void __MeshLoader_Worker_ObjWorker_freeResources (
        MeshLoader_Job_Context                      context,
        __MeshLoader_Worker_ObjWorker_Control       control
) {

    if ( control->pIndexData != NULL ) {

        (void) MeshLoader_Job_freeMemory (
                context,
                control->pIndexData
        );
    }

    if ( control->pIndices != NULL ) {
        (void) MeshLoader_Job_freeMemory (
                context,
                control->pIndices
        );
    }

    if ( control->pFaces != NULL ) {
        (void) MeshLoader_Job_freeMemory (
                context,
                control->pFaces
        );
    }

    if ( control->pVertices != NULL ) {
        (void) MeshLoader_Job_freeMemory (
                context,
                control->pVertices
        );
    }

    (void) MeshLoader_Job_freeMemory (
            context,
            control
    );
}

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_initializationState (
        MeshLoader_Job_Context                  context,
        __MeshLoader_Worker_ObjWorker_Control   control
) {

    MeshLoader_Result result;

    result = MeshLoader_Job_getInputPath (
            context,
            & control->inputPath
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    result = MeshLoader_Job_getLoadMode (
            context,
            & control->loadMode
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    control->pFile = fopen ( control->inputPath, "r" );

    if ( control->pFile == NULL ) {
        return MeshLoader_Result_ResourceNotFound;
    }

    fseek ( control->pFile, 0, SEEK_END );
    control->fileSize       = ftell ( control->pFile );
    control->filePosition   = 0U;
    fseek ( control->pFile, 0, SEEK_SET );

    control->pIndexData     = NULL;
    control->indexCapacity  = 0U;

    if ( ( control->loadMode & MeshLoader_MeshLoadModeFlag_LoadIndices ) != 0U ) {
        result = MeshLoader_Job_allocateMemory2 (
                context,
                sizeof ( MeshLoader_IndexData ),
                alignof ( MeshLoader_IndexData ),
                ( void ** ) & control->pIndexData
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }

#if MESH_LOADER_JOB_STARTING_INDEX_BUFFER_SIZE > 0U

        result = MeshLoader_Job_allocateMemory2 (
                context,
                MESH_LOADER_JOB_STARTING_INDEX_BUFFER_SIZE * sizeof ( MeshLoader_uint32 ),
                alignof ( MeshLoader_uint32 ),
                ( void ** ) & ( * control ).pIndices
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }

        control->pIndexData->indexCount = 0U;
        control->indexCapacity          = MESH_LOADER_JOB_STARTING_INDEX_BUFFER_SIZE;

#else

        control->pIndexData->indexCount = 0U;
        control->indexCapacity          = 0U;
        control->pIndices               = NULL;

#endif

    }

    control->nextState = __MeshLoader_Worker_ObjWorker_State_ReadAndDigest;
    return MeshLoader_Result_Success;
}

static inline MeshLoader_bool __MeshLoader_Worker_ObjWorker_isComment (
        MeshLoader_StringLiteral    string,
        MeshLoader_size             maxLength
) {

    MeshLoader_size index = 0U;
    while ( index < maxLength ) {
        if ( strchr ( " \t", string[index] ) != NULL ) {
            continue;
        }

        if ( string [index] == '#' ) {
            return MeshLoader_true;
        }

        return MeshLoader_false;
    }

    return MeshLoader_false;
}

static inline __MeshLoader_ObjFormat_LineDataType __MeshLoader_Worker_ObjWorker_detectDataType (
        MeshLoader_StringLiteral    dataBuffer
) {

    if ( dataBuffer[0] == 'v' && dataBuffer[1] == ' ' ) {
        return __MeshLoader_ObjFormat_LineDataType_Vertex;
    }

    if ( dataBuffer[0] == 'f' && dataBuffer[1] == ' ' ) {
        return __MeshLoader_ObjFormat_LineDataType_Face;
    }

    return __MeshLoader_ObjFormat_LineDataType_Unknown;
}

static inline MeshLoader_Result __MeshLoader_Worker_ObjWorker_insertData (
        MeshLoader_Job_Context                  context,
        __MeshLoader_Worker_ObjWorker_Control   control
) {

    switch ( __MeshLoader_Worker_ObjWorker_detectDataType ( & control->readBuffer [0] ) ) {
        case __MeshLoader_ObjFormat_LineDataType_Vertex:    return __MeshLoader_Worker_ObjWorker_insertVertexData ( context, control );
        case __MeshLoader_ObjFormat_LineDataType_Face:
            if ( ( control->loadMode & MeshLoader_MeshLoadModeFlag_LoadFaces ) != 0U ) {
                return __MeshLoader_Worker_ObjWorker_insertFaceData ( context, control );
            } else {
                return __MeshLoader_Worker_ObjWorker_insertIndexData ( context, control );
            }

        default:                                            return MeshLoader_Result_ResourceNotFound;
    }
}

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_readAndDigestState (
        MeshLoader_Job_Context                  context,
        __MeshLoader_Worker_ObjWorker_Control   control
) {

    MeshLoader_size startOfProcessFilePos   = control->filePosition;
    MeshLoader_size currentlyProcessed;

    MeshLoader_Result result;

    while ( ! feof ( control->pFile ) ) {
        (void) fgets (
                & control->readBuffer[0],
                MESH_LOADER_JOB_MAXIMUM_OBJ_FILE_LENGTH,
                control->pFile
        );

        control->filePosition = ftell ( control->pFile );

        if ( ! __MeshLoader_Worker_ObjWorker_isComment ( control->readBuffer, MESH_LOADER_JOB_MAXIMUM_OBJ_FILE_LENGTH ) ) {

            result = __MeshLoader_Worker_ObjWorker_insertData (
                    context,
                    control
            );

            if ( result != MeshLoader_Result_Success ) {
                return result;
            }
        }

        currentlyProcessed = control->filePosition - startOfProcessFilePos;
        if ( currentlyProcessed > MESH_LOADER_JOB_MAXIMUM_CYCLE_COUNT ) {
            break;
        }
    }

    result = MeshLoader_Job_setProgress (
            context,
            ( float ) control->filePosition / ( float ) control->fileSize
    );

    if ( result != MeshLoader_Result_Success ) {
        return result;
    }

    if ( control->fileSize == control->filePosition ) {
        control->nextState = __MeshLoader_Worker_ObjWorker_State_StoreMeshData;
    }

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_storeMeshDataState (
        MeshLoader_Job_Context                  context,
        __MeshLoader_Worker_ObjWorker_Control   control
) {

    MeshLoader_Result result;

    if ( control->pVertices != NULL ) {

        result = MeshLoader_Job_releaseMemory (
                context,
                control->pVertices
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }

        result = MeshLoader_Job_setMeshVertexData (
                context,
                (MeshLoader_uint32) control->vertexCount,
                control->pVertices
        );

        control->pVertices = NULL;

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }
    }

    if ( control->pFaces != NULL ) {

        result = MeshLoader_Job_releaseMemory (
                context,
                control->pFaces
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }

        result = MeshLoader_Job_setMeshFaceData (
                context,
                (MeshLoader_uint32) control->faceCount,
                control->pFaces
        );

        control->pFaces = NULL;

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }
    }

    if ( control->pIndexData != NULL ) {

        result = MeshLoader_Job_releaseMemory (
                context,
                control->pIndices
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }

        result = MeshLoader_Job_releaseMemory (
                context,
                control->pIndexData
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }

        control->pIndexData->pIndices = control->pIndices;

        result = MeshLoader_Job_setMeshIndexData (
                context,
                control->pIndexData
        );

        control->pIndexData = NULL;
        control->pIndices   = NULL;

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }
    }

    control->nextState = __MeshLoader_Worker_ObjWorker_State_Finished;
    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_finishedState (
        MeshLoader_Job_Context                  context,
        __MeshLoader_Worker_ObjWorker_Control   control
) {

    (void) context;
    fclose ( control->pFile );

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_errorState (
        MeshLoader_Job_Context                  context,
        __MeshLoader_Worker_ObjWorker_Control   control
) {

    (void) control;
    (void) context;

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_insertVertexData (
        MeshLoader_Job_Context                  context,
        __MeshLoader_Worker_ObjWorker_Control   control
) {

    MeshLoader_Result   result = MeshLoader_Result_Success;
    MeshLoader_size     newCapacity;

    if ( control->vertexCount >= control->vertexCapacity ) {
        newCapacity = __MeshLoader_Utility_maxSize ( control->vertexCapacity * 2, control->vertexCount + 1 );

        result = MeshLoader_Job_reallocateMemory2 (
                context,
                control->pVertices,
                newCapacity * sizeof ( MeshLoader_VertexData ),
                alignof ( MeshLoader_VertexData ),
                ( void ** ) & control->pVertices
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }

        control->vertexCapacity = newCapacity;
    }

    MeshLoader_VertexData     * pVertexData     = & control->pVertices [ control->vertexCount ++ ];
    char * pCurrentlyAt                         = control->readBuffer + 2;

    pVertexData->x = strtod ( pCurrentlyAt, & pCurrentlyAt );
    pVertexData->y = strtod ( pCurrentlyAt, & pCurrentlyAt );
    pVertexData->z = strtod ( pCurrentlyAt, & pCurrentlyAt );

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_insertFaceData (
        MeshLoader_Job_Context                  context,
        __MeshLoader_Worker_ObjWorker_Control   control
) {

    MeshLoader_Result   result;
    MeshLoader_size     newCapacity;

    if ( control->faceCount >= control->faceCapacity ) {
        newCapacity = __MeshLoader_Utility_maxSize ( control->faceCapacity * 2, control->faceCount + 1 );

        result = MeshLoader_Job_reallocateMemory2 (
                context,
                control->pFaces,
                newCapacity * sizeof ( MeshLoader_FaceData ),
                alignof ( MeshLoader_FaceData ),
                ( void ** ) & control->pFaces
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }

        control->faceCapacity = newCapacity;
    }

    MeshLoader_FaceData    * pFaceData          = & control->pFaces [ control->faceCount ++ ];
    char * pCurrentlyAt                         = control->readBuffer + 2;

    pFaceData->u = ( MeshLoader_uint32 ) strtoul ( pCurrentlyAt, & pCurrentlyAt, 10 );
    pFaceData->v = ( MeshLoader_uint32 ) strtoul ( pCurrentlyAt, & pCurrentlyAt, 10 );
    pFaceData->w = ( MeshLoader_uint32 ) strtoul ( pCurrentlyAt, & pCurrentlyAt, 10 );

    if ( ( control->loadMode & MeshLoader_MeshLoadModeFlag_LoadIndices ) != 0U ) {
        return __MeshLoader_Worker_ObjWorker_insertIndexDataFromLastFace (
                context,
                control
        );
    }

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_insertIndexData (
        MeshLoader_Job_Context                  context,
        __MeshLoader_Worker_ObjWorker_Control   control
) {

    MeshLoader_Result   result;
    MeshLoader_size     newCapacity;

    if ( control->pIndexData->indexCount >= control->indexCapacity ) {
        newCapacity = __MeshLoader_Utility_maxSize ( control->indexCapacity * 4U, control->pIndexData->indexCount + 1 );

        result = MeshLoader_Job_reallocateMemory2 (
                context,
                control->pIndices,
                newCapacity * sizeof ( MeshLoader_uint32 ),
                alignof ( MeshLoader_uint32 ),
                ( void ** ) & control->pIndices
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }

        control->faceCapacity = newCapacity;
    }

    char * pCurrentlyAt                         = control->readBuffer + 2;

    control->pIndices [ control->pIndexData->indexCount ++ ] = ( MeshLoader_uint32 ) strtoul ( pCurrentlyAt, & pCurrentlyAt, 10 );
    control->pIndices [ control->pIndexData->indexCount ++ ] = ( MeshLoader_uint32 ) strtoul ( pCurrentlyAt, & pCurrentlyAt, 10 );
    control->pIndices [ control->pIndexData->indexCount ++ ] = ( MeshLoader_uint32 ) strtoul ( pCurrentlyAt, & pCurrentlyAt, 10 );

    return MeshLoader_Result_Success;
}

static MeshLoader_Result __MeshLoader_Worker_ObjWorker_insertIndexDataFromLastFace (
        MeshLoader_Job_Context                  context,
        __MeshLoader_Worker_ObjWorker_Control   control
) {

    MeshLoader_Result   result;
    MeshLoader_size     newCapacity;

    if ( control->pIndexData->indexCount + 2U >= control->indexCapacity ) {
        newCapacity = __MeshLoader_Utility_maxSize ( control->indexCapacity * 4U, control->pIndexData->indexCount + 1 );

        result = MeshLoader_Job_reallocateMemory2 (
                context,
                control->pIndices,
                newCapacity * sizeof ( MeshLoader_uint32 ),
                alignof ( MeshLoader_uint32 ),
                ( void ** ) & control->pIndices
        );

        if ( result != MeshLoader_Result_Success ) {
            return result;
        }

        control->indexCapacity = newCapacity;
    }

    control->pIndices [ control->pIndexData->indexCount ++ ] = control->pFaces [ control->faceCount ].u;
    control->pIndices [ control->pIndexData->indexCount ++ ] = control->pFaces [ control->faceCount ].v;
    control->pIndices [ control->pIndexData->indexCount ++ ] = control->pFaces [ control->faceCount ].w;

    return MeshLoader_Result_Success;
}
