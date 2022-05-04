#include <meshLoader/meshLoader>
#include <stdio.h>

int main() {

    MeshLoader_Result               result;

    MeshLoader_Instance             instance1;
    MeshLoader_Instance             instance2;
    MeshLoader_Instance             instance3;
    MeshLoader_InstanceCreateInfo   instanceCreateInfo;

    instanceCreateInfo.structureType            = MeshLoader_StructureType_InstanceCreateInfo;
    instanceCreateInfo.pNext                    = NULL;
    instanceCreateInfo.maxWorkerThreadCount     = 8U;
    instanceCreateInfo.flags                    = MeshLoader_nullFlags;

    result = MeshLoader_createInstance (
            & instanceCreateInfo,
            & instance1,
            NULL
    );

    if ( result != MeshLoader_Result_Success ) {
        fprintf ( stderr, "Failed to create MeshLoader Instance" );
        return 1;
    }

    result = MeshLoader_createInstance (
            & instanceCreateInfo,
            & instance2,
            NULL
    );

    if ( result != MeshLoader_Result_Success ) {
        fprintf ( stderr, "Failed to create MeshLoader Instance" );
        return 1;
    }

    result = MeshLoader_createInstance (
            & instanceCreateInfo,
            & instance3,
            NULL
    );

    if ( result != MeshLoader_Result_Success ) {
        fprintf ( stderr, "Failed to create MeshLoader Instance" );
        return 1;
    }

    MeshLoader_destroyInstance ( instance1, NULL );
    MeshLoader_destroyInstance ( instance3, NULL );
    MeshLoader_destroyInstance ( instance2, NULL );

    return 0;
}
