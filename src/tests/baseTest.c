#include <meshLoader/meshLoader>
#include <stdio.h>

int main() {

    MeshLoader_Result               result;

    MeshLoader_Instance             instance;
    MeshLoader_InstanceCreateInfo   instanceCreateInfo;

    instanceCreateInfo.structureType            = MeshLoader_StructureType_InstanceCreateInfo;
    instanceCreateInfo.pNext                    = NULL;
    instanceCreateInfo.maxWorkerThreadCount     = 8U;
    instanceCreateInfo.flags                    = MeshLoader_nullFlags;

    result = MeshLoader_createInstance (
            & instanceCreateInfo,
            & instance,
            NULL
    );

    if ( result != MeshLoader_Result_Success ) {
        fprintf ( stderr, "Failed to create MeshLoader Instance" );
        return 1;
    }

    MeshLoader_destroyInstance ( instance, NULL );

    return 0;
}
