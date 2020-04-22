#pragma once
//typedef unsigned __int64 QWORD; //unsigned long long	__int64 == long long
#define QWORD unsigned __int64
typedef unsigned long DWORD;


// Request to read virtual user memory (memory of a program) from kernel space
#define READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0801 /* Our Custom Code */, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

// Request to write virtual user memory (memory of a program) from kernel space
#define WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0802 /* Our Custom Code */, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

// Request to retrieve the process id of csgo process, from kernel space
#define ID CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0803 /* Our Custom Code */, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

// Request to retrieve the base address of client.dll in csgo.exe from kernel space
#define MODULE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0804 /* Our Custom Code */, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)