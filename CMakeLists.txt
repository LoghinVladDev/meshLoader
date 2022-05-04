cmake_minimum_required(VERSION 3.22)
project(meshLoader C)

include_directories ( include )

set(CMAKE_C_STANDARD 99)

set (
        MESH_LOADER_IMPLEMENTATION_POSIX_SOURCES

        src/implementation/posix/posixThread.c
        src/implementation/posix/posixMutex.c

        src/implementation/posix/posixThread.h
        src/implementation/posix/posixMutex.h
)

set (
        MESH_LOADER_IMPLEMENTATION_SOURCES

        ${MESH_LOADER_IMPLEMENTATION_POSIX_SOURCES}

        src/implementation/instance.h
        src/implementation/instance.c
        src/implementation/internalAllocation.h
        src/implementation/internalAllocation.c
        src/implementation/utility.c
        src/implementation/privateUtility.h
        src/implementation/privateTypes.h
        src/implementation/mutex.h
        src/implementation/thread.h
        src/implementation/job.c
        src/implementation/job.h
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
