//
// Created by loghin on 5/10/22.
//

#include <stdalign.h>
#include <memory.h>
#include "mesh.h"
#include "job.h"

MeshLoader_Result __MeshLoader_Mesh_construct (
        MeshLoader_Mesh                         mesh,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    (void) pAllocationCallbacks;

    mesh->data.vertexCount  = 0U;
    mesh->data.pVertices    = NULL;
    mesh->data.faceCount    = 0U;
    mesh->data.pFaces       = NULL;
    mesh->data.pIndexData   = NULL;

    return MeshLoader_Result_Success;
}

void __MeshLoader_Mesh_destruct (
        MeshLoader_Mesh                         mesh,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = 0U,
            .alignment              = 0U,
            .allocationScope        = MeshLoader_SystemAllocationScope_Object,
            .explicitMemoryPurpose  = NULL
    };

    if ( mesh->data.pVertices != NULL ) {

        allocationNotification.pMemory                  = ((void *)(MeshLoader_size)mesh->data.pVertices);
        allocationNotification.explicitMemoryPurpose    = "Destroys a mesh's vertices array";

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

    if ( mesh->data.pFaces != NULL ) {

        allocationNotification.pMemory                  = ((void *)(MeshLoader_size)mesh->data.pFaces);
        allocationNotification.explicitMemoryPurpose    = "Destroys a mesh's faces array";

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

    if ( mesh->data.pIndexData != NULL ) {
        if ( mesh->data.pIndexData->pIndices != NULL ) {

            allocationNotification.pMemory                  = ((void *)(MeshLoader_size)mesh->data.pIndexData->pIndices);
            allocationNotification.explicitMemoryPurpose    = "Destroys a mesh's indices array";

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

        allocationNotification.pMemory                  = ((void *)(MeshLoader_size)mesh->data.pIndexData);
        allocationNotification.explicitMemoryPurpose    = "Destroys a mesh's index data";

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
}

MeshLoader_Result MeshLoader_getMesh (
        MeshLoader_Job      job,
        MeshLoader_Mesh   * pMesh
) {

    switch ( ( MeshLoader_JobState ) job->context.jobState ) {
        case MeshLoader_JobState_Ready:
            return MeshLoader_Result_JobNotStarted;
        case MeshLoader_JobState_Running:
            return MeshLoader_Result_NotReady;
        case MeshLoader_JobState_FinishedError:
            return MeshLoader_Result_JobExecutionFailed;
        case MeshLoader_JobState_Finished:
            break;

        default:
            return MeshLoader_Result_ErrorUnknown;
    }

    * pMesh = & job->context.mesh;
    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_takeMesh (
        MeshLoader_Job                          job,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks,
        MeshLoader_Mesh                       * pMesh
) {

    switch ( ( MeshLoader_JobState ) job->context.jobState ) {
        case MeshLoader_JobState_Ready:
            return MeshLoader_Result_JobNotStarted;
        case MeshLoader_JobState_Running:
            return MeshLoader_Result_NotReady;
        case MeshLoader_JobState_FinishedError:
            return MeshLoader_Result_JobExecutionFailed;
        case MeshLoader_JobState_Finished:
            break;

        default:
            return MeshLoader_Result_ErrorUnknown;
    }

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = sizeof ( struct __MeshLoader_Mesh ),
            .alignment              = alignof ( struct __MeshLoader_Mesh ),
            .allocationScope        = MeshLoader_SystemAllocationScope_Object,
            .explicitMemoryPurpose  = "Copies a Mesh due to different used allocators"
    };

    MeshLoader_Mesh mesh = NULL;

    pAllocationCallbacks = __MeshLoader_InternalAllocation_getCallbacks (pAllocationCallbacks );

    allocationNotification.pMemory = pAllocationCallbacks->allocationFunction (
            pAllocationCallbacks->pUserData,
            allocationNotification.size,
            allocationNotification.alignment,
            allocationNotification.allocationScope
    );

    if ( allocationNotification.pMemory == NULL ) {
        return MeshLoader_Result_OutOfMemory;
    }

    if (pAllocationCallbacks->internalAllocationNotificationFunction != NULL ) {
        pAllocationCallbacks->internalAllocationNotificationFunction (
                pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    mesh = ( MeshLoader_Mesh ) allocationNotification.pMemory;

    mesh->data.vertexCount  = 0U;
    mesh->data.faceCount    = 0U;
    mesh->data.pVertices    = NULL;
    mesh->data.pFaces       = NULL;
    mesh->data.pIndexData   = NULL;

    if (
            __MeshLoader_InternalAllocation_allocationCallbacksEqual (
                    pAllocationCallbacks,
                    & job->context.jobMemoryAllocator.storedAllocationCallbacks
            )
    ) {

        mesh->data.vertexCount   = job->context.mesh.data.vertexCount;
        mesh->data.faceCount     = job->context.mesh.data.faceCount;

        job->context.mesh.data.vertexCount  = 0U;
        job->context.mesh.data.faceCount    = 0U;

        mesh->data.pVertices     = job->context.mesh.data.pVertices;
        mesh->data.pFaces        = job->context.mesh.data.pFaces;
        mesh->data.pIndexData    = job->context.mesh.data.pIndexData;

        job->context.mesh.data.pVertices    = NULL;
        job->context.mesh.data.pFaces       = NULL;
        job->context.mesh.data.pIndexData   = NULL;

        * pMesh = mesh;

        return MeshLoader_Result_Success;
    }

    if ( job->context.mesh.data.pVertices != NULL ) {

        allocationNotification.size         = job->context.mesh.data.vertexCount * sizeof ( MeshLoader_VertexData );
        allocationNotification.alignment    = alignof ( MeshLoader_VertexData );

        allocationNotification.pMemory      = pAllocationCallbacks->allocationFunction (
                pAllocationCallbacks->pUserData,
                allocationNotification.size,
                allocationNotification.alignment,
                allocationNotification.allocationScope
        );

        if ( allocationNotification.pMemory == NULL ) {
            allocationNotification.pMemory = mesh;

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

            return MeshLoader_Result_OutOfMemory;
        }

        if ( pAllocationCallbacks->internalAllocationNotificationFunction != NULL ) {
            pAllocationCallbacks->internalAllocationNotificationFunction (
                    pAllocationCallbacks->pUserData,
                    & allocationNotification
            );
        }

        mesh->data.pVertices = ( MeshLoader_VertexData * ) memcpy (
                allocationNotification.pMemory,
                job->context.mesh.data.pVertices,
                sizeof ( MeshLoader_VertexData ) * job->context.mesh.data.vertexCount
        );
    }

    if ( job->context.mesh.data.pFaces != NULL ) {

        allocationNotification.size         = job->context.mesh.data.faceCount * sizeof ( MeshLoader_FaceData );
        allocationNotification.alignment    = alignof ( MeshLoader_FaceData );

        allocationNotification.pMemory      = pAllocationCallbacks->allocationFunction (
                pAllocationCallbacks->pUserData,
                allocationNotification.size,
                allocationNotification.alignment,
                allocationNotification.allocationScope
        );

        if ( allocationNotification.pMemory == NULL ) {
            allocationNotification.pMemory = ((void *) (MeshLoader_size) mesh->data.pVertices);

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

            allocationNotification.pMemory = mesh;

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

            return MeshLoader_Result_OutOfMemory;
        }

        if ( pAllocationCallbacks->internalAllocationNotificationFunction != NULL ) {
            pAllocationCallbacks->internalAllocationNotificationFunction (
                    pAllocationCallbacks->pUserData,
                    & allocationNotification
            );
        }

        mesh->data.pFaces = ( MeshLoader_FaceData * ) memcpy (
                allocationNotification.pMemory,
                job->context.mesh.data.pFaces,
                sizeof ( MeshLoader_FaceData ) * job->context.mesh.data.faceCount
        );
    }

    if ( job->context.mesh.data.pIndexData != NULL ) {

        allocationNotification.size         = sizeof ( MeshLoader_IndexData );
        allocationNotification.alignment    = alignof ( MeshLoader_IndexData );

        allocationNotification.pMemory      = pAllocationCallbacks->allocationFunction (
                pAllocationCallbacks->pUserData,
                allocationNotification.size,
                allocationNotification.alignment,
                allocationNotification.allocationScope
        );

        if ( allocationNotification.pMemory == NULL ) {
            allocationNotification.pMemory = ((void *) (MeshLoader_size) mesh->data.pFaces);

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

            allocationNotification.pMemory = ((void *) (MeshLoader_size) mesh->data.pVertices);

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

            allocationNotification.pMemory = mesh;

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

            return MeshLoader_Result_OutOfMemory;
        }

        ( ( MeshLoader_IndexData * ) allocationNotification.pMemory )->pIndices     = NULL;
        ( ( MeshLoader_IndexData * ) allocationNotification.pMemory )->indexCount   = 0U;

        mesh->data.pIndexData = ( MeshLoader_IndexData * ) allocationNotification.pMemory;

        if ( job->context.mesh.data.pIndexData->pIndices != NULL ) {

            allocationNotification.size         = job->context.mesh.data.pIndexData->indexCount * sizeof ( MeshLoader_uint32 );
            allocationNotification.alignment    = alignof ( MeshLoader_uint32 );

            allocationNotification.pMemory      = pAllocationCallbacks->allocationFunction (
                    pAllocationCallbacks->pUserData,
                    allocationNotification.size,
                    allocationNotification.alignment,
                    allocationNotification.allocationScope
            );

            if ( allocationNotification.pMemory == NULL ) {
                allocationNotification.pMemory = ((void *) (MeshLoader_size) mesh->data.pIndexData);

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

                allocationNotification.pMemory = ((void *) (MeshLoader_size) mesh->data.pFaces);

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

                allocationNotification.pMemory = ((void *) (MeshLoader_size) mesh->data.pVertices);

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

                allocationNotification.pMemory = mesh;

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

                return MeshLoader_Result_OutOfMemory;
            }

            if ( pAllocationCallbacks->internalAllocationNotificationFunction != NULL ) {
                pAllocationCallbacks->internalAllocationNotificationFunction (
                        pAllocationCallbacks->pUserData,
                        & allocationNotification
                );
            }

            ( ( MeshLoader_IndexData * ) ( MeshLoader_size ) mesh->data.pIndexData )->pIndices = ( MeshLoader_uint32 * ) memcpy (
                    allocationNotification.pMemory,
                    job->context.mesh.data.pIndexData->pIndices,
                    sizeof ( MeshLoader_uint32 ) * job->context.mesh.data.pIndexData->indexCount
            );
        }
    }

    mesh->data.vertexCount              = job->context.mesh.data.vertexCount;
    mesh->data.faceCount                = job->context.mesh.data.faceCount;

    job->context.mesh.data.vertexCount  = 0U;
    job->context.mesh.data.faceCount    = 0U;

    if ( job->context.mesh.data.pVertices != NULL ) {
        allocationNotification.pMemory = ((void *) (MeshLoader_size) job->context.mesh.data.pVertices);

        if ( job->context.jobMemoryAllocator.storedAllocationCallbacks.internalFreeNotificationFunction != NULL ) {
            job->context.jobMemoryAllocator.storedAllocationCallbacks.internalFreeNotificationFunction (
                    job->context.jobMemoryAllocator.storedAllocationCallbacks.pUserData,
                    & allocationNotification
            );
        }

        job->context.jobMemoryAllocator.storedAllocationCallbacks.freeFunction (
                job->context.jobMemoryAllocator.storedAllocationCallbacks.pUserData,
                allocationNotification.pMemory
        );
    }

    if ( job->context.mesh.data.pFaces != NULL ) {
        allocationNotification.pMemory = ((void *) (MeshLoader_size) job->context.mesh.data.pFaces);

        if ( job->context.jobMemoryAllocator.storedAllocationCallbacks.internalFreeNotificationFunction != NULL ) {
            job->context.jobMemoryAllocator.storedAllocationCallbacks.internalFreeNotificationFunction (
                    job->context.jobMemoryAllocator.storedAllocationCallbacks.pUserData,
                    & allocationNotification
            );
        }

        job->context.jobMemoryAllocator.storedAllocationCallbacks.freeFunction (
                job->context.jobMemoryAllocator.storedAllocationCallbacks.pUserData,
                allocationNotification.pMemory
        );
    }

    if ( job->context.mesh.data.pIndexData != NULL ) {
        if ( job->context.mesh.data.pIndexData->pIndices != NULL ) {
            allocationNotification.pMemory = ((void *) (MeshLoader_size) job->context.mesh.data.pIndexData->pIndices);

            if ( job->context.jobMemoryAllocator.storedAllocationCallbacks.internalFreeNotificationFunction != NULL ) {
                pAllocationCallbacks->internalFreeNotificationFunction (
                        job->context.jobMemoryAllocator.storedAllocationCallbacks.pUserData,
                        & allocationNotification
                );
            }

            job->context.jobMemoryAllocator.storedAllocationCallbacks.freeFunction (
                    job->context.jobMemoryAllocator.storedAllocationCallbacks.pUserData,
                    allocationNotification.pMemory
            );
        }

        allocationNotification.pMemory = ((void *) (MeshLoader_size) job->context.mesh.data.pIndexData);

        if ( job->context.jobMemoryAllocator.storedAllocationCallbacks.internalFreeNotificationFunction != NULL ) {
            job->context.jobMemoryAllocator.storedAllocationCallbacks.internalFreeNotificationFunction (
                    job->context.jobMemoryAllocator.storedAllocationCallbacks.pUserData,
                    & allocationNotification
            );
        }

        job->context.jobMemoryAllocator.storedAllocationCallbacks.freeFunction (
                job->context.jobMemoryAllocator.storedAllocationCallbacks.pUserData,
                allocationNotification.pMemory
        );
    }

    job->context.mesh.data.pIndexData   = NULL;
    job->context.mesh.data.pVertices    = NULL;
    job->context.mesh.data.pFaces       = NULL;

    * pMesh = mesh;

    return MeshLoader_Result_Success;
}

void MeshLoader_destroyMesh (
        MeshLoader_Mesh                         mesh,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = 0U,
            .alignment              = 0U,
            .allocationScope        = MeshLoader_SystemAllocationScope_Object,
            .explicitMemoryPurpose  = "Destroys a Mesh Object obtained from a completed Job"
    };

    pAllocationCallbacks = __MeshLoader_InternalAllocation_getCallbacks (pAllocationCallbacks );

    if ( mesh->data.pVertices != NULL ) {
        allocationNotification.pMemory = ((void *) (MeshLoader_size) mesh->data.pVertices);

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

    if ( mesh->data.pFaces != NULL ) {
        allocationNotification.pMemory = ((void *) (MeshLoader_size) mesh->data.pFaces);

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

    if ( mesh->data.pIndexData != NULL ) {
        if ( mesh->data.pIndexData->pIndices != NULL ) {
            allocationNotification.pMemory = ((void *) (MeshLoader_size) mesh->data.pIndexData->pIndices);

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

        allocationNotification.pMemory = ((void *) (MeshLoader_size) mesh->data.pIndexData);

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

    allocationNotification.pMemory = mesh;

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

MeshLoader_Result MeshLoader_enumerateMeshVertices (
        MeshLoader_Mesh             mesh,
        MeshLoader_uint32         * pVertexCount,
        MeshLoader_VertexData     * pVertices
) {

    if ( mesh->data.pVertices == NULL ) {
        return MeshLoader_Result_ResourceNotFound;
    }

    if ( pVertices == NULL ) {
        * pVertexCount = mesh->data.vertexCount;
        return MeshLoader_Result_Success;
    }

    if ( * pVertexCount < mesh->data.vertexCount ) {
        (void) memcpy ( pVertices, mesh->data.pVertices, sizeof ( MeshLoader_VertexData ) * ( * pVertexCount ) );
        return MeshLoader_Result_TooSmall;
    }

    (void) memcpy ( pVertices, mesh->data.pVertices, sizeof ( MeshLoader_VertexData ) * mesh->data.vertexCount );
    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_enumerateMeshFaces (
        MeshLoader_Mesh             mesh,
        MeshLoader_uint32         * pFaceCount,
        MeshLoader_FaceData       * pFaces
) {

    if ( mesh->data.pFaces == NULL ) {
        return MeshLoader_Result_ResourceNotFound;
    }

    if ( pFaces == NULL ) {
        * pFaceCount = mesh->data.faceCount;
        return MeshLoader_Result_Success;
    }

    if ( * pFaceCount < mesh->data.faceCount ) {
        (void) memcpy ( pFaces, mesh->data.pFaces, sizeof ( MeshLoader_FaceData ) * ( * pFaceCount ) );
        return MeshLoader_Result_TooSmall;
    }

    (void) memcpy ( pFaces, mesh->data.pFaces, sizeof ( MeshLoader_FaceData ) * mesh->data.faceCount );
    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_enumerateIndices (
        MeshLoader_Mesh                         mesh,
        MeshLoader_uint32                     * pIndexCount,
        MeshLoader_uint32                     * pIndices
) {

    if ( mesh->data.pIndexData == NULL ) {
        return MeshLoader_Result_ResourceNotFound;
    }

    if ( pIndices == NULL ) {
        * pIndexCount = mesh->data.pIndexData->indexCount;
        return MeshLoader_Result_Success;
    }

    if ( * pIndexCount < mesh->data.pIndexData->indexCount ) {
        (void) memcpy ( pIndices, mesh->data.pIndexData->pIndices, sizeof ( MeshLoader_uint32 ) * ( * pIndexCount ) );
        return MeshLoader_Result_TooSmall;
    }

    (void) memcpy ( pIndices, mesh->data.pIndexData->pIndices, sizeof ( MeshLoader_uint32 ) * mesh->data.pIndexData->indexCount );
    return MeshLoader_Result_Success;
}

MeshLoader_Result MeshLoader_getMeshData (
        MeshLoader_Mesh         mesh,
        MeshLoader_MeshData   * pMeshData
) {

    pMeshData->vertexCount  = mesh->data.vertexCount;
    pMeshData->faceCount    = mesh->data.faceCount;
    pMeshData->pVertices    = mesh->data.pVertices;
    pMeshData->pFaces       = mesh->data.pFaces;
    pMeshData->pIndexData   = mesh->data.pIndexData;

    return MeshLoader_Result_Success;
}
