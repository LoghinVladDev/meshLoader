//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_UTILITY_H__
#define __MESH_LOADER_UTILITY_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <meshLoader/publicTypes>

extern MeshLoader_StringLiteral MeshLoader_StructureType_toString ( MeshLoader_StructureType );
extern MeshLoader_StringLiteral MeshLoader_SystemAllocationScope_toString ( MeshLoader_SystemAllocationScope );
extern MeshLoader_StringLiteral MeshLoader_Result_toString ( MeshLoader_Result );
extern MeshLoader_StringLiteral MeshLoader_JobState_toString ( MeshLoader_JobState );
extern MeshLoader_StringLiteral MeshLoader_JobsCreateFlagBits_toString ( MeshLoader_JobsCreateFlagBits );

extern MeshLoader_size MeshLoader_AllocationCallbacks_toString ( MeshLoader_AllocationCallbacks const * );
extern MeshLoader_size MeshLoader_InstanceCreateInfo_toString ( MeshLoader_InstanceCreateInfo const * );
extern MeshLoader_size MeshLoader_JobsCreateInfo_toString ( MeshLoader_JobsCreateInfo const * );
extern MeshLoader_size MeshLoader_JobsStartInfo_toString ( MeshLoader_JobsStartInfo const * );
extern MeshLoader_size MeshLoader_JobsPauseInfo_toString ( MeshLoader_JobsPauseInfo const * );
extern MeshLoader_size MeshLoader_JobsResumeInfo_toString ( MeshLoader_JobsResumeInfo const * );
extern MeshLoader_size MeshLoader_JobsStopInfo_toString ( MeshLoader_JobsStopInfo const * );
extern MeshLoader_size MeshLoader_JobsTerminateInfo_toString ( MeshLoader_JobsTerminateInfo const * );
extern MeshLoader_size MeshLoader_JobsQueryInfo_toString ( MeshLoader_JobsQueryInfo const * );
extern MeshLoader_size MeshLoader_CreateJobInfo_toString ( MeshLoader_CreateJobInfo const * );
extern MeshLoader_size MeshLoader_QueryJobInfo_toString ( MeshLoader_QueryJobInfo const * );
extern MeshLoader_size MeshLoader_AllocationNotification_toString ( MeshLoader_AllocationNotification const * );

#if defined(__cplusplus)
}
#endif

#endif // __MESH_LOADER_UTILITY_H__
