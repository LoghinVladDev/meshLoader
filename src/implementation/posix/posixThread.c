//
// Created by loghin on 5/4/22.
//

#include <pthread.h>
#include <meshLoader/publicTypes>
#include <stdalign.h>
#include "../internalAllocation.h"

#if defined(__linux__)

struct __MeshLoader_Posix_Thread {
    pthread_t thread;
};

#endif
