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
    MeshLoader_StructureType_JobsGetResult          = 0x0000000AU,

    MeshLoader_StructureType_CreateJobInfo          = 0x0000000BU,
    MeshLoader_StructureType_StartJobInfo           = 0x0000000CU,
    MeshLoader_StructureType_PauseJobInfo           = 0x0000000DU,
    MeshLoader_StructureType_ResumeJobInfo          = 0x0000000EU,
    MeshLoader_StructureType_StopJobInfo            = 0x0000000FU,
    MeshLoader_StructureType_TerminateJobInfo       = 0x00000010U,
    MeshLoader_StructureType_QueryJobInfo           = 0x00000011U,
    MeshLoader_StructureType_GetJobResult           = 0x00000012U,

    MeshLoader_StructureType_AllocationNotification = 0x00000013U,
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
    MeshLoader_StringLiteral            inputPath;
    float                               priority;
} MeshLoader_CreateJobInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_JobsCreateFlags          flags;
    MeshLoader_MeshLoadModeFlags        loadMode;
    MeshLoader_uint32                   jobCount;
    MeshLoader_Job                    * pJobs;
    MeshLoader_CreateJobInfo    const * pCreateJobInfos;
} MeshLoader_JobsCreateInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
} MeshLoader_StartJobInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_JobsStartFlags           flags;
    MeshLoader_uint32                   jobCount;
    MeshLoader_StartJobInfo     const * pStartJobInfos;
} MeshLoader_JobsStartInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
} MeshLoader_PauseJobInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_JobsPauseFlags           flags;
    MeshLoader_uint32                   jobCount;
    MeshLoader_PauseJobInfo     const * pPauseJobInfos;
} MeshLoader_JobsPauseInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
} MeshLoader_ResumeJobInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_JobsResumeFlags          flags;
    MeshLoader_uint32                   jobCount;
    MeshLoader_ResumeJobInfo    const * pResumeJobInfos;
} MeshLoader_JobsResumeInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
} MeshLoader_StopJobInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_JobsStopFlags            flags;
    MeshLoader_uint32                   jobCount;
    MeshLoader_StopJobInfo      const * pStopJobInfos;
} MeshLoader_JobsStopInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
} MeshLoader_TerminateJobInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                        const * pNext;
    MeshLoader_JobsTerminateFlags       flags;
    MeshLoader_uint32                   jobCount;
    MeshLoader_TerminateJobInfo const * pTerminateJobsInfos;
} MeshLoader_JobsTerminateInfo;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                              * pNext;
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
} MeshLoader_GetJobResult;

typedef struct {
    MeshLoader_StructureType            structureType;
    void                              * pNext;
    MeshLoader_uint32                   jobCount;
    MeshLoader_GetJobResult           * pGetJobResults;
} MeshLoader_JobsGetResult;

typedef MeshLoader_Result ( * MeshLoader_CreateInstanceFunction ) (
        MeshLoader_InstanceCreateInfo   const * pCreateInfo,
        MeshLoader_Instance                   * pInstance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

typedef void ( * MeshLoader_DestroyInstanceFunction ) (
        MeshLoader_Instance                     instance,
        MeshLoader_AllocationCallbacks  const * pAllocationCallbacks
);

#endif // __MESH_LOADER_PUBLIC_TYPES_H__
