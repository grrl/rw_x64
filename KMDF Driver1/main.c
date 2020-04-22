
#include <ntdef.h>
#include <ntifs.h>
#include "macro.h"
#include "imports.h"
#include "structs.h"

PDEVICE_OBJECT pDeviceObject; // our driver object
UNICODE_STRING dev, dos; // Driver registry paths
DWORD csgoId;
QWORD ClientAddress;

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject);
NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP irp);
NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp);

NTSTATUS KeReadVirtualMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
{
	PSIZE_T Bytes;
	if (NT_SUCCESS(MmCopyVirtualMemory(Process, SourceAddress, PsGetCurrentProcess(),
		TargetAddress, Size, KernelMode, &Bytes)))
		return STATUS_SUCCESS;
	else
		return STATUS_ACCESS_DENIED;
}

NTSTATUS KeWriteVirtualMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
{
	PSIZE_T Bytes;
	if (NT_SUCCESS(MmCopyVirtualMemory(PsGetCurrentProcess(), SourceAddress, Process,
		TargetAddress, Size, KernelMode, &Bytes)))
		return STATUS_SUCCESS;
	else
		return STATUS_ACCESS_DENIED;
}


QWORD GetSectionBaseAddress(HANDLE w_pid)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
	PEPROCESS targetProcess;
	PVOID value = 0;

	ntStatus = PsLookupProcessByProcessId((HANDLE)w_pid, &targetProcess);

	if (ntStatus != STATUS_SUCCESS || !targetProcess)
		return 0;

	__try
	{
		KeAttachProcess((PKPROCESS)targetProcess);
		value = PsGetProcessSectionBaseAddress(targetProcess);
		KeDetachProcess();
	}
	__except (GetExceptionCode())
	{
		return 0;
	}

	return (QWORD)value;
}

// set a callback for every PE image loaded to user memory
// then find the client.dll & csgo.exe using the callback
PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName,
	HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	// Compare our string to input
	if (wcsstr(FullImageName->Buffer, L"\\r5apex.exe")) {
		// if it matches
		DbgPrintEx(0, 0, "Loaded Name: %ls \n", FullImageName->Buffer);
		DbgPrintEx(0, 0, "Loaded To Process: %d \n", ProcessId);
		csgoId = ProcessId;

		ClientAddress = GetSectionBaseAddress(ProcessId);
	}
}


// IOCTL Call Handler function
NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	NTSTATUS Status;
	DWORD BytesIO = 0;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	// Code received from user space
	DWORD ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	if (ControlCode == READ)
	{
		// Get the input buffer & format it to our struct
		PKERNEL_MEMORY ReadInput = (PKERNEL_MEMORY)Irp->AssociatedIrp.SystemBuffer;
		PKERNEL_MEMORY ReadOutput = (PKERNEL_MEMORY)Irp->AssociatedIrp.SystemBuffer;

		PEPROCESS Process;
		// Get our process
		if (NT_SUCCESS(PsLookupProcessByProcessId(ReadInput->ProcessId, &Process)))
			KeReadVirtualMemory(Process, ReadInput->Address,
				&ReadInput->Buffer, ReadInput->Size);

		DbgPrintEx(0, 0, "Read Params:  %lu, %#010x \n", ReadInput->ProcessId, ReadInput->Address);
		DbgPrintEx(0, 0, "Value: %lu \n", ReadOutput->Buffer);

		Status = STATUS_SUCCESS;
		BytesIO = sizeof(KERNEL_MEMORY);
	}
	else if (ControlCode == WRITE)
	{
		// Get the input buffer & format it to our struct
		PKERNEL_MEMORY WriteInput = (PKERNEL_MEMORY)Irp->AssociatedIrp.SystemBuffer;

		PEPROCESS Process;
		// Get our process
		if (NT_SUCCESS(PsLookupProcessByProcessId(WriteInput->ProcessId, &Process)))
			KeWriteVirtualMemory(Process, &WriteInput->Buffer,
				WriteInput->Address, WriteInput->Size);

		DbgPrintEx(0, 0, "Write Params:  %lu, %#010x \n", WriteInput->Buffer, WriteInput->Address);

		Status = STATUS_SUCCESS;
		BytesIO = sizeof(KERNEL_MEMORY);
	}
	else if (ControlCode == ID)
	{
		PULONG OutPut = (PULONG)Irp->AssociatedIrp.SystemBuffer;
		*OutPut = csgoId;

		DbgPrintEx(0, 0, "id get %#010x", csgoId);
		Status = STATUS_SUCCESS;
		BytesIO = sizeof(*OutPut);
	}
	else if (ControlCode == MODULE)
	{
		PULONGLONG OutPut = (PULONGLONG)Irp->AssociatedIrp.SystemBuffer;
		*OutPut = ClientAddress;

		DbgPrintEx(0, 0, "Module get %#010x", ClientAddress);
		Status = STATUS_SUCCESS;
		BytesIO = sizeof(*OutPut);
	}
	else
	{
		// if the code is unknown
		Status = STATUS_INVALID_PARAMETER;
		BytesIO = 0;
	}

	// Complete the request
	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = BytesIO;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}

// Driver Entrypoint
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject,
	PUNICODE_STRING pRegistryPath)
{
	DbgPrintEx(0, 0, "Driver Loaded\n");

	PsSetLoadImageNotifyRoutine(ImageLoadCallback);

	RtlInitUnicodeString(&dev, L"\\Device\\neet");
	RtlInitUnicodeString(&dos, L"\\DosDevices\\neet");

	IoCreateDevice(pDriverObject, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	IoCreateSymbolicLink(&dos, &dev);

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;
	pDriverObject->DriverUnload = UnloadDriver;

	pDeviceObject->Flags |= DO_DIRECT_IO;
	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}



NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Unload routine called.\n");

	PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);
	IoDeleteSymbolicLink(&dos);
	IoDeleteDevice(pDriverObject->DeviceObject);
}

NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
