//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_PRIVATE_TYPES__
#define __MESH_LOADER_PRIVATE_TYPES__

#include <meshLoader/publicTypes>
#include "mutex.h"

struct __MeshLoader_Instance {
    MeshLoader_uint32   maxThreadCount;
    __MeshLoader_Mutex  instanceLock;
};

struct __MeshLoader_Job {

};

struct __MeshLoader_Mesh {

};

#endif // __MESH_LOADER_PRIVATE_TYPES__
