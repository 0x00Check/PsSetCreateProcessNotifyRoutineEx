#include <ntifs.h>
#include <ntddk.h>
#include <wdm.h>

void createProcessNotifyRoutineEx(PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo) {
	PUNICODE_STRING processName = NULL;
	PsLookupProcessByProcessId(ProcessId, &Process);
	SeLocateProcessImageName(Process, &processName);

	if (CreateInfo != NULL) {
		PEPROCESS parentProcess = NULL;
		PUNICODE_STRING parentProcessName = NULL;
		PsLookupProcessByProcessId(CreateInfo->ParentProcessId, &parentProcess);
		SeLocateProcessImageName(parentProcess, &parentProcessName);

		DbgPrint("[Process Created] '%wZ'(%p)\n", processName, ProcessId);
		DbgPrint("\tParent Process => '%wZ'(%p)", parentProcessName, CreateInfo->ParentProcessId);
		DbgPrint("\tImageFileName => %wZ\n", CreateInfo->ImageFileName);
		DbgPrint("\tCommandLine   => %wZ\n", CreateInfo->CommandLine);
	} else {	
		DbgPrint("[Process Terminated] '%wZ'(%p)\n", processName, ProcessId);
	}
}

void DriverUnload(PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);

	DbgPrint("[PsSetCreateProcessNotifyRoutineEx] Unloading ..\n");
	PsSetCreateProcessNotifyRoutineEx(createProcessNotifyRoutineEx, TRUE);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	DbgPrint("[PsSetCreateProcessNotifyRoutineEx] Loading ..\n");
	NTSTATUS ntStatus = PsSetCreateProcessNotifyRoutineEx(createProcessNotifyRoutineEx, FALSE);

	DriverObject->DriverUnload = DriverUnload;

	return ntStatus;
}