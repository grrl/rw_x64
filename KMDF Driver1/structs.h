#pragma once
// datatype for read request
#include "macro.h"
typedef struct _KERNEL_MEMORY
{
	DWORD ProcessId;

	QWORD Address;
	QWORD Buffer;
	QWORD Size;

} KERNEL_MEMORY, *PKERNEL_MEMORY;

typedef struct _SYSTEM_PROCESS_INFO
{
	ULONG                   NextEntryOffset;
	ULONG                   NumberOfThreads;
	LARGE_INTEGER           Reserved[3];
	LARGE_INTEGER           CreateTime;
	LARGE_INTEGER           UserTime;
	LARGE_INTEGER           KernelTime;
	UNICODE_STRING          ImageName;
	ULONG                   BasePriority;
	HANDLE                  ProcessId;
	HANDLE                  InheritedFromProcessId;
}SYSTEM_PROCESS_INFO, *PSYSTEM_PROCESS_INFO;