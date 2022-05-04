cmake_minimum_required(VERSION 3.22)
project(meshLoader C)

include_directories ( include )

set(CMAKE_C_STANDARD 99)

set (
        MESH_LOADER_IMPLEMENTATION_LINUX_SOURCES

        src/implementation/linux/linuxThread.c
        src/implementation/linux/linuxMutex.c

        src/implementation/linux/linuxThread.h
        src/implementation/linux/linuxMutex.h
)

set (
        MESH_LOADER_IMPLEMENTATION_SOURCES

        ${MESH_LOADER_IMPLEMENTATION_LINUX_SOURCES}

        src/implementation/privateTypes.h
        src/implementation/privateUtility.h
        src/implementation/instance.c
        src/implementation/internalAllocation.h
        src/implementation/internalAllocation.c
        src/implementation/utility.c
        src/implementation/mutex.h
        src/implementation/thread.h
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
