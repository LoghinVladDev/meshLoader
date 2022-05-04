//
// Created by loghin on 5/4/22.
//

#include <pthread.h>
#include <meshLoader/publicTypes>

static pthread_mutex_t  __MeshLoader_LinuxMutex_moduleLock;
static MeshLoader_bool  __MeshLoader_LinuxMutex_firstApplyModuleLockCall = MeshLoader_true;

MeshLoader_Result __MeshLoader_applyModuleLock () {
    if ( __MeshLoader_LinuxMutex_firstApplyModuleLockCall ) {

        pthread_mutex_init ( & __MeshLoader_LinuxMutex_moduleLock, NULL );
        __MeshLoader_LinuxMutex_firstApplyModuleLockCall = MeshLoader_false;
    }

    pthread_mutex_lock ( & __MeshLoader_LinuxMutex_moduleLock );
    return MeshLoader_Result_Success;
}

void __MeshLoader_removeModuleLock () {
    pthread_mutex_unlock ( & __MeshLoader_LinuxMutex_moduleLock );
}