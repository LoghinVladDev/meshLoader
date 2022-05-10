cmake_minimum_required(VERSION 3.22)
project(meshLoader C)

include_directories ( include )

set(CMAKE_C_STANDARD 11)

set (
        MESH_LOADER_IMPLEMENTATION_POSIX_SOURCES

        src/implementation/posix/posixMutex.c
        src/implementation/posix/posixMutex.h

        src/implementation/posix/posixThread.c
        src/implementation/posix/posixThread.h
)

set (
        MESH_LOADER_IMPLEMENTATION_WIN32_SOURCES

        src/implementation/win32/win32Mutex.c
        src/implementation/win32/win32Mutex.h

        src/implementation/win32/win32Thread.c
        src/implementation/win32/win32Thread.h
)

set (
        MESH_LOADER_IMPLEMENTATION_WORKERS_SOURCES

        src/implementation/workers/specializedWorker.c
        src/implementation/workers/specializedWorker.h

        src/implementation/workers/objWorker.c
        src/implementation/workers/objWorker.h
)

set (
        MESH_LOADER_IMPLEMENTATION_SOURCES

        ${MESH_LOADER_IMPLEMENTATION_POSIX_SOURCES}
        ${MESH_LOADER_IMPLEMENTATION_WIN32_SOURCES}
        ${MESH_LOADER_IMPLEMENTATION_WORKERS_SOURCES}

        src/implementation/instance.h
        src/implementation/instance.c

        src/implementation/internalAllocation.h
        src/implementation/internalAllocation.c

        src/implementation/utility.c
        src/implementation/privateUtility.h
        src/implementation/privateTypes.h

        src/implementation/mesh.c
        src/implementation/mesh.h

        src/implementation/mutex.h
        src/implementation/thread.h

        src/implementation/job.c
        src/implementation/job.h

        src/implementation/jobMemoryAllocator.c
        src/implementation/jobMemoryAllocator.h

        src/implementation/jobDispatcher.c
        src/implementation/jobDispatcher.h

        src/implementation/jobWorker.c
        src/implementation/jobWorker.h

        src/implementation/jobPriorityQueue.c
        src/implementation/jobPriorityQueue.h

        src/implementation/string.c
        src/implementation/string.h
)

set (
        MESH_LOADER_CONFIG_SOURCES

        src/config/instanceCnf.h
)

set (
        MESH_LOADER_INTERFACE_SOURCES

        src/interface/publicTypes.h
        src/interface/meshLoader.h
        src/interface/utility.h
)

set (
        MESH_LOADER_SOURCES

        ${MESH_LOADER_IMPLEMENTATION_SOURCES}
        ${MESH_LOADER_INTERFACE_SOURCES}
        ${MESH_LOADER_CONFIG_SOURCES}
)

add_executable (
        mesh_loader_basic_test

        ${MESH_LOADER_SOURCES}

        src/tests/baseTest.c
)
