/* Cheat that uses a driver for reading / writing virtual memory,
instead of using Win32API Functions. Written By Zer0Mem0ry,
https://www.youtube.com/watch?v=sJdBtPosWQs */

#include <Windows.h>
#include "macro.h"
#include "structs.h"
/* IOCTL Codes needed for our driver */


// interface for our driver
class Driver
{
public:
	HANDLE hDriver; // Handle to driver
	DWORD ProcessId;
	QWORD base;
	template <class T>
	T RPM(uintptr_t address)
	{
		//T read = ReadVirtualMemory<T>(ProcessId, address, sizeof(T));
		return ReadVirtualMemory<T>(ProcessId, address, sizeof(T));
	}

	template <typename type>
	type ReadVirtualMemory(DWORD ProcessId, uintptr_t ReadAddress,
		SIZE_T Size)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return *(type*)false;

		QWORD Return, Bytes;
		KERNEL_MEMORY ReadRequest;

		ReadRequest.ProcessId = ProcessId;
		ReadRequest.Address = ReadAddress;
		ReadRequest.Size = Size;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, READ, &ReadRequest,
			sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), 0, 0))
			return *(type*)&ReadRequest.Buffer;
		else
			return *(type*)false;
	}

	template <class T>
	bool WPM(uintptr_t address, T value)
	{
		return WriteVirtualMemory(ProcessId, address, value, sizeof(T));
	}

	bool WriteVirtualMemory(DWORD ProcessId, QWORD WriteAddress,
		QWORD WriteValue, SIZE_T WriteSize)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;
		DWORD Bytes;

		KERNEL_MEMORY  WriteRequest;
		WriteRequest.ProcessId = ProcessId;
		WriteRequest.Address = WriteAddress;
		WriteRequest.Buffer = WriteValue;
		WriteRequest.Size = WriteSize;

		if (DeviceIoControl(hDriver, WRITE, &WriteRequest, sizeof(WriteRequest),
			0, 0, &Bytes, NULL))
			return true;
		else
			return false;
	}

	DWORD GetTargetPid()
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		DWORD Id;
		DWORD Bytes;

		if (DeviceIoControl(hDriver, ID, &Id, sizeof(Id),
			&Id, sizeof(Id), &Bytes, NULL))
			return Id;
		else
			return false;
	}

	QWORD GetClientModule()
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		QWORD Address;
		DWORD Bytes;

		if (DeviceIoControl(hDriver, MODULE, &Address, sizeof(Address),
			&Address, sizeof(Address), &Bytes, NULL))
			return Address;
		else
			return false;
	}
};
