//
// Created by loghin on 5/5/22.
//

#ifndef __MESH_LOADER_STRING_H__
#define __MESH_LOADER_STRING_H__

#include "internalAllocation.h"

typedef struct {
    char            * string;
    MeshLoader_size   length;
} __MeshLoader_String;

extern MeshLoader_Result __MeshLoader_String_createFromStringLiteral (
        __MeshLoader_String                           *,
        MeshLoader_StringLiteral,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

extern void __MeshLoader_String_destroy (
        __MeshLoader_String                     const *,
        __MeshLoader_ScopedAllocationCallbacks  const *
);

#endif // __MESH_LOADER_STRING_H__
