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