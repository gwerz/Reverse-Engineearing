#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include <ntddk.h>
#ifdef __cplusplus
}
#endif

#define PAGEDCODE	code_seg("PAGE")
#define LOCKEDCODE	code_seg()
#define INITCODE	code_seg("INIT")

#define PAGEDDATA	data_seg("PAGE")
#define LOCKEDDATA	data_seg()
#define INITDATA	data_seg("INIT")

typedef struct _MYOBJECT_NAME_INFORMATION//sizeof=1024
{
	UNICODE_STRING	Name;
	WCHAR			NameBuffer[(1024-sizeof(UNICODE_STRING))/sizeof(WCHAR)];//508
}MYOBJECT_NAME_INFORMATION, *MYPOBJECT_NAME_INFORMATION;

NTSTATUS DeviceCreateOrClose(IN PDEVICE_OBJECT pDevObj,IN PIRP pIrp);
NTSTATUS DefaultDispatchRoutine(IN PDEVICE_OBJECT pDevObj,IN PIRP pIrp);
NTSTATUS DeviceQueryInfomation(IN PDEVICE_OBJECT pDevObj,IN PIRP pIrp);
NTSTATUS DeviceWrite(IN PDEVICE_OBJECT pDevObj,IN PIRP pIrp);
NTSTATUS DeviceRead(IN PDEVICE_OBJECT pDevObj,IN PIRP pIrp);
NTSTATUS UnloadRoutine(IN PDRIVER_OBJECT pDriverObject);

HANDLE Handle;
PFILE_OBJECT hGlobalFileObject;