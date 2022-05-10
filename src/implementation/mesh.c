//
// Created by loghin on 5/10/22.
//

#include "mesh.h"

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
