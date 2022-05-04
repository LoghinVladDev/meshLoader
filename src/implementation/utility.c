//
// Created by loghin on 5/4/22.
//

#include <meshLoader/utility>

MeshLoader_StringLiteral MeshLoader_SystemAllocationScope_toString ( MeshLoader_SystemAllocationScope scope ) {
    switch ( scope ) {
        case MeshLoader_SystemAllocationScope_Instance:     return "Instance";
        case MeshLoader_SystemAllocationScope_Worker:       return "Worker";
        case MeshLoader_SystemAllocationScope_Object:       return "Object";
        case MeshLoader_SystemAllocationScope_Component:    return "Component";

        default:                                            return "Unknown";
    }
}
