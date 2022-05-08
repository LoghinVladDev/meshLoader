//
// Created by loghin on 5/4/22.
//

#ifndef __MESH_LOADER_PUBLIC_TYPES_H__
#define __MESH_LOADER_PUBLIC_TYPES_H__

#define MeshLoader_true                 0x01U
#define MeshLoader_false                0x00U

#ifndef NULL
#define NULL                            ( ( void * ) 0x00000000U )
#endif

#define MeshLoader_invalidHandle        ( ( MeshLoader_Handle ) 0x00000000U )
#define MeshLoader_nullFlags            ( ( MeshLoader_Flags )  0x00000000U )


typedef unsigned char                   MeshLoader_uint8;
typedef unsigned short int              MeshLoader_uint16;
typedef unsigned int                    MeshLoader_uint32;
typedef unsigned long long int          MeshLoader_uint64;

typedef signed char                     MeshLoader_sint8;
typedef signed short int                MeshLoader_sint16;
typedef signed int                      MeshLoader_sint32;
typedef signed long long int            MeshLoader_sint64;

typedef MeshLoader_uint8                MeshLoader_bool;
typedef MeshLoader_uint64               MeshLoader_size;
typedef char const *                    MeshLoader_StringLiteral;
typedef MeshLoader_uint32               MeshLoader_Flags;

typedef MeshLoader_Flags                MeshLoader_InstanceCreateFlags;
typedef MeshLoader_Flags                MeshLoader_JobsCreateFlags;
typedef MeshLoader_Flags                MeshLoader_JobsStartFlags;
typedef MeshLoader_Flags                MeshLoader_JobsPauseFlags;
typedef MeshLoader_Flags                MeshLoader_JobsResumeFlags;
typedef MeshLoader_Flags                MeshLoader_JobsStopFlags;
typedef MeshLoader_Flags                MeshLoader_JobsTerminateFlags;
typedef MeshLoader_Flags                MeshLoader_JobsQueryFlags;
typedef MeshLoader_Flags                MeshLoader_MeshLoadModeFlags;

typedef void                          * MeshLoader_Handle;
typedef struct __MeshLoader_Instance  * MeshLoader_Instance;
typedef struct __MeshLoader_Job       * MeshLoader_Job;
typedef struct __MeshLoader_Mesh      * MeshLoader_Mesh;


typedef enum {
    MeshLoader_StructureType_Unknown                = 0x00000000U,

    MeshLoader_StructureType_AllocationCallbacks    = 0x00000001U,

    MeshLoader_StructureType_InstanceCreateInfo     = 0x00000002U,

    MeshLoader_StructureType_JobsCreateInfo         = 0x00000003U,
    MeshLoader_StructureType_JobsStartInfo          = 0x00000004U,
    MeshLoader_StructureType_JobsPauseInfo          = 0x00000005U,
    MeshLoader_StructureType_JobsResumeInfo         = 0x00000006U,
    MeshLoader_StructureType_JobsStopInfo           = 0x00000007U,
    MeshLoader_StructureType_JobsTerminateInfo      = 0x00000008U,
    MeshLoader_StructureType_JobsQueryInfo          = 0x00000009U,
    MeshLoader_StructureType_JobsGetResultInfo      = 0x0000000AU,

    MeshLoader_StructureType_CreateJobInfo          = 0x0000000BU,
    MeshLoader_StructureType_QueryJobInfo           = 0x00000011U,
    MeshLoader_StructureType_JobResult              = 0x00000012U,

    MeshLoader_StructureType_AllocationNotification = 0x00000013U,

    MeshLoader_StructureType_JobData                = 0x00001000U,
    MeshLoader_StructureType_CustomJobInfo          = 0x00001001U,
} MeshLoader_StructureType;

typedef enum {
    MeshLoader_SystemAllocationScope_Unknown        = 0x00000000U,
    MeshLoader_SystemAllocationScope_Instance       = 0x00000001U,
    MeshLoader_SystemAllocationScope_Worker         = 0x00000002U,
    MeshLoader_SystemAllocationScope_Object         = 0x00000003U,
    MeshLoader_SystemAllocationScope_Component      = 0x00000004U,
} MeshLoader_SystemAllocationScope;

typedef enum {
    MeshLoader_Result_Success                       = 0x00000000U,
    MeshLoader_Result_ErrorUnknown                  = 0x00000001U,
    MeshLoader_Result_IllegalArgument               = 0x00000002U,
    MeshLoader_Result_TooSmall                      = 0x00000003U,
    MeshLoader_Result_TooManyObjects                = 0x00000004U,
    MeshLoader_Result_OutOfMemory                   = 0x00000005U,
    MeshLoader_Result_MutexError                    = 0x00000006U,
    MeshLoader_Result_PriorityQueueEmpty            = 0x00000007U,
    MeshLoader_Result_PriorityQueueFull             = 0x00000008U,
} MeshLoader_Result;

typedef enum {
    MeshLoader_JobStatus_Unknown                    = 0x00000000U,
    MeshLoader_JobStatus_Ready                      = 0x00000001U,
    MeshLoader_JobStatus_Running                    = 0x00000002U,
    MeshLoader_JobStatus_FinishedError              = 0x00000003U,
    MeshLoader_JobStatus_Finished                   = 0x00000004U,
} MeshLoader_JobStatus;

typedef enum {
    MeshLoader_MeshLoadModeFlag_LoadFaces           = 0x00000001U,
    MeshLoader_MeshLoadModeFlag_LoadIndices         = 0x00000002U,
} MeshLoader_MeshLoadModeFlagBits;

typedef enum {
    MeshLoader_JobsCreateFlag_ContinueIfError       = 0x00000001U,
} MeshLoader_JobsCreateFlagBits;

typedef void * ( * MeshLoader_AllocationFunction ) (
        void                              * pUserData,
        MeshLoader_size                     size,
        MeshLoader_size                     alignment,
        MeshLoader_SystemAllocationScope    allocationScope
);

typedef void * ( * MeshLoader_ReallocationFunction ) (
        void                              * pUserData,
        void                              * pOriginal,
        MeshLoader_size                     size,
        MeshLoader_size                     alignment,
        MeshLoader_SystemAllocationScope    allocationScope
);

typedef void ( * MeshLoader_FreeFunction ) (
        void                              * pUserData,
        void                              * pMemory
);

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    void                              * pMemory;
    void                              * pOldMemory;
    MeshLoader_size                     size;
    MeshLoader_size                     alignment;
    MeshLoader_SystemAllocationScope    allocationScope;
    MeshLoader_StringLiteral            explicitMemoryPurpose;
} MeshLoader_AllocationNotification;

typedef void ( * MeshLoader_InternalAllocationNotificationFunction ) (
        void                                      * pUserData,
        MeshLoader_AllocationNotification   const * pNotification
);

typedef struct {
    MeshLoader_StructureType                            structureType;
    void                                              * pNext;
    void                                              * pUserData;
    MeshLoader_AllocationFunction                       allocationFunction;
    MeshLoader_ReallocationFunction                     reallocationFunction;
    MeshLoader_FreeFunction                             freeFunction;
    MeshLoader_InternalAllocationNotificationFunction   internalAllocationNotificationFunction;
    MeshLoader_InternalAllocationNotificationFunction   internalReallocationNotificationFunction;
    MeshLoader_InternalAllocationNotificationFunction   internalFreeNotificationFunction;
} MeshLoader_AllocationCallbacks;


typedef struct {
    float x;
    float y;
    float z;
} MeshLoader_VertexData;

typedef struct {
    MeshLoader_uint32 u;
    MeshLoader_uint32 v;
    MeshLoader_uint32 w;
} MeshLoader_FaceData;

typedef struct {
    MeshLoader_uint32           indexCount;
    MeshLoader_uint32   const * pIndices;
} MeshLoader_IndexData;

typedef struct {
    MeshLoader_uint32               vertexCount;
    MeshLoader_VertexData   const * pVertices;
    MeshLoader_uint32               faceCount;
    MeshLoader_FaceData     const * pFaces;
    MeshLoader_IndexData    const * pIndexData;
} MeshLoader_MeshData;


typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_InstanceCreateFlags      flags;
    MeshLoader_uint32                   maxWorkerThreadCount;
} MeshLoader_InstanceCreateInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_MeshLoadModeFlags        loadMode;
    MeshLoader_StringLiteral            inputPath;
    float                               priority;
} MeshLoader_CreateJobInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_JobsCreateFlags          flags;
    MeshLoader_uint32                   jobCount;
    MeshLoader_Job                    * pJobs;
    MeshLoader_CreateJobInfo    const * pCreateJobInfos;
} MeshLoader_JobsCreateInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_JobsStartFlags           flags;
    MeshLoader_uint32                   jobCount;
    MeshLoader_Job              const * pJobs;
    MeshLoader_AllocationCallbacks      pAllocationCallbacks;
} MeshLoader_JobsStartInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_JobsPauseFlags           flags;
    MeshLoader_uint32                   jobCount;
    MeshLoader_Job              const * pJobs;
} MeshLoader_JobsPauseInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_JobsResumeFlags          flags;
    MeshLoader_uint32                   jobCount;
    MeshLoader_Job              const * pJobs;
} MeshLoader_JobsResumeInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_JobsStopFlags            flags;
    MeshLoader_uint32                   jobCount;
    MeshLoader_Job              const * pJobs;
} MeshLoader_JobsStopInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_JobsTerminateFlags       flags;
    MeshLoader_uint32                   jobCount;
    MeshLoader_Job              const * pJobs;
} MeshLoader_JobsTerminateInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                              * pNext;
    MeshLoader_Job                      job;
    MeshLoader_JobStatus                status;
    float                               progress;
} MeshLoader_QueryJobInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                              * pNext;
    MeshLoader_JobsQueryFlags           flags;
    MeshLoader_uint32                   jobCount;
    MeshLoader_QueryJobInfo           * pQueryJobInfos;
} MeshLoader_JobsQueryInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                              * pNext;
    MeshLoader_Mesh                   * pMesh;
} MeshLoader_JobResult;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                              * pNext;
    MeshLoader_uint32                   jobCount;
    MeshLoader_JobResult              * pGetJobResults;
} MeshLoader_JobsGetResultInfo;

typedef MeshLoader_Result ( * MeshLoader_CreateInstanceFunction ) (
        MeshLoader_InstanceCreateInfo   const * pCreateInfo,
        MeshLoader_Instance                   * pInstance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

typedef void ( * MeshLoader_DestroyInstanceFunction ) (
        MeshLoader_Instance                     instance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

typedef MeshLoader_Result ( * MeshLoader_CreateJobsFunction ) (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsCreateInfo       const * pCreateInfo,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

typedef void ( * MeshLoader_DestroyJobsFunction ) (
        MeshLoader_Instance                     instance,
        MeshLoader_uint32                       jobCount,
        MeshLoader_Job                  const * pJobs,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

typedef MeshLoader_Result ( * MeshLoader_StartJobsFunction ) (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsStartInfo        const * pStartInfo
);

typedef MeshLoader_Result ( * MeshLoader_QueryJobsFunction ) (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsQueryInfo              * pQueryInfo
);

typedef MeshLoader_Result ( * MeshLoader_GetResultsFunction ) (
        MeshLoader_Instance                     instance,
        MeshLoader_JobsGetResultInfo          * pResults
);

typedef MeshLoader_Result ( * MeshLoader_AnyJobsRunningFunction ) (
        MeshLoader_Instance                     instance,
        MeshLoader_bool                       * pAnyRunning
);

#endif // __MESH_LOADER_PUBLIC_TYPES_H__
