//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_LINUX_MUTEX_H__
#define __MESH_LOADER_LINUX_MUTEX_H__

#include <meshLoader/publicTypes>

extern MeshLoader_Result __MeshLoader_applyModuleLock ();
extern void __MeshLoader_removeModuleLock ();

extern MeshLoader_Result __MeshLoader_Mutex_init (
        struct __MeshLoader_Linux_Mutex *
);

#endif // __MESH_LOADER_LINUX_MUTEX_H__
