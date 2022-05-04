//
// Created by loghin on 5/5/22.
//

#include "string.h"
#include <string.h>

MeshLoader_Result __MeshLoader_String_createFromStringLiteral (
        __MeshLoader_String                           * pDestination,
        MeshLoader_StringLiteral                        source,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    pDestination->length = strlen ( source );

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = NULL,
            .pOldMemory             = NULL,
            .size                   = pDestination->length + 1U,
            .alignment              = 1U,
            .allocationScope        = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose  = "Creates a String from a source StringLiteral"
    };

    allocationNotification.pMemory = pAllocationCallbacks->pAllocationCallbacks->allocationFunction (
            pAllocationCallbacks->pAllocationCallbacks->pUserData,
            allocationNotification.size,
            allocationNotification.alignment,
            allocationNotification.allocationScope
    );

    if ( allocationNotification.pMemory == NULL ) {
        pDestination->length = 0U;
        pDestination->string = NULL;

        return MeshLoader_Result_OutOfMemory;
    }

    if ( pAllocationCallbacks->pAllocationCallbacks->internalAllocationNotificationFunction != NULL ) {
        pAllocationCallbacks->pAllocationCallbacks->internalAllocationNotificationFunction (
                pAllocationCallbacks->pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    memcpy ( allocationNotification.pMemory, & source[0], pDestination->length + 1U );
    pDestination->string = ( char * ) allocationNotification.pMemory;

    return MeshLoader_Result_Success;
}

void __MeshLoader_String_destroy (
        __MeshLoader_String                     const * pString,
        __MeshLoader_ScopedAllocationCallbacks  const * pAllocationCallbacks
) {

    MeshLoader_AllocationNotification allocationNotification = {
            .structureType          = MeshLoader_StructureType_AllocationNotification,
            .pNext                  = NULL,
            .pMemory                = pString->string,
            .pOldMemory             = NULL,
            .size                   = pString->length + 1U,
            .alignment              = 1U,
            .allocationScope        = pAllocationCallbacks->allocationScope,
            .explicitMemoryPurpose  = "Destroys a String"
    };

    if ( pAllocationCallbacks->pAllocationCallbacks->internalFreeNotificationFunction != NULL ) {
        pAllocationCallbacks->pAllocationCallbacks->internalFreeNotificationFunction (
                pAllocationCallbacks->pAllocationCallbacks->pUserData,
                & allocationNotification
        );
    }

    pAllocationCallbacks->pAllocationCallbacks->freeFunction (
            pAllocationCallbacks->pAllocationCallbacks->pUserData,
            pString->string
    );
}