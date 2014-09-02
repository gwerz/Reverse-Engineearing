#undef _MSC_VER
#define _MSC_VER 1500
#include "UnlockerDriver.h"

#pragma PAGEDCODE
NTSTATUS DeviceCreateOrClose(IN PDEVICE_OBJECT pDevObj,IN PIRP pIrp)
{
	return STATUS_SUCCESS;
}

NTSTATUS DefaultDispatchRoutine(IN PDEVICE_OBJECT pDevObj,IN PIRP pIrp)
{
	return STATUS_NOT_SUPPORTED;
}

NTSTATUS DeviceQueryInfomation(IN PDEVICE_OBJECT pDevObj,IN PIRP pIrp)
{
	pIrp->IoStatus.Status=STATUS_UNSUCCESSFUL;
	pIrp->IoStatus.Information=0;
	IofCompleteRequest(pIrp,IO_NO_INCREMENT);
	return STATUS_UNSUCCESSFUL;
}

NTSTATUS DeviceWrite(IN PDEVICE_OBJECT pDevObj,IN PIRP pIrp)
{ 
	PIO_STACK_LOCATION IrpSp= IoGetCurrentIrpStackLocation(pIrp);
	ULONG transferedbytes=0;
	NTSTATUS statu=STATUS_UNSUCCESSFUL;
	PVOID TheMappedSystemVa;
	if(IrpSp && pIrp->MdlAddress && IrpSp->Parameters.Write.Length == 8)
	{
		if(pIrp->MdlAddress->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA | MDL_SOURCE_IS_NONPAGED_POOL))
			TheMappedSystemVa=pIrp->MdlAddress->MappedSystemVa;
		else
			TheMappedSystemVa=MmMapLockedPagesSpecifyCache(pIrp->MdlAddress,KernelMode,MmCached,NULL,FALSE,NormalPagePriority);
		Handle=*(HANDLE*)TheMappedSystemVa;
		hGlobalFileObject=(PFILE_OBJECT)*((char*)TheMappedSystemVa+4);
		//注：以上2句和unlocker.exe的UnlockProcess函数对照看，可知分别对应TargetHandle和process->Object_Pointer
		statu=STATUS_SUCCESS;
		transferedbytes=8;
	}
	pIrp->IoStatus.Information=transferedbytes;
	pIrp->IoStatus.Status=statu;
	IofCompleteRequest(pIrp,IO_NO_INCREMENT);
	return statu;
}

NTSTATUS DeviceRead(IN PDEVICE_OBJECT pDevObj,IN PIRP pIrp)
{
	PIO_STACK_LOCATION IrpSp= IoGetCurrentIrpStackLocation(pIrp);
	NTSTATUS statu=STATUS_BUFFER_TOO_SMALL;
	PVOID TheMappedSystemVa;
	PFILE_OBJECT FileObject;
	MYPOBJECT_NAME_INFORMATION Allocated;
	ULONG ReturnLength,CopyLength=0;
	USHORT SpareLength;

	if(IrpSp && pIrp->MdlAddress)
	{
		if(pIrp->MdlAddress->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA | MDL_SOURCE_IS_NONPAGED_POOL))
			TheMappedSystemVa=pIrp->MdlAddress->MappedSystemVa;
		else
			TheMappedSystemVa=MmMapLockedPagesSpecifyCache(pIrp->MdlAddress,KernelMode,MmCached,NULL,FALSE,NormalPagePriority);
		if(STATUS_SUCCESS == ObReferenceObjectByHandle(Handle,GENERIC_READ,*IoFileObjectType,KernelMode,(PVOID*)&FileObject,NULL))
		{
			if(FileObject && FileObject == hGlobalFileObject && FileObject->DeviceObject)
			{
				Allocated=(MYPOBJECT_NAME_INFORMATION)ExAllocatePoolWithTag(NonPagedPool,1024,'TLFD');
				if(Allocated)
				{
					if(STATUS_SUCCESS == ObQueryNameString((PVOID)FileObject->DeviceObject,Allocated,sizeof(MYOBJECT_NAME_INFORMATION),&ReturnLength))
					{//注：这句和unlocker.exe的UnlockProcess函数对照看：ReadFile
						if(Allocated->Name.Buffer && Allocated->Name.Buffer == Allocated->NameBuffer)
						{
							SpareLength=sizeof(Allocated->NameBuffer)-Allocated->Name.Length;
							if(SpareLength > 0 && FileObject->FileName.Buffer)
							{
								SpareLength=min(SpareLength,FileObject->FileName.Length);
							}
							if(SpareLength)
							{
								memcpy(Allocated->NameBuffer+Allocated->Name.Length/2,FileObject->FileName.Buffer,SpareLength);
								Allocated->Name.Length += SpareLength;
								ReturnLength=Allocated->Name.Length+8;
							}
							if(Allocated->Name.Length && TheMappedSystemVa && ReturnLength <= IrpSp->Parameters.Read.Length)
							{
								memcpy(TheMappedSystemVa,Allocated,ReturnLength);
								CopyLength=ReturnLength;
								statu=STATUS_SUCCESS;
							}
						}
					}
					ExFreePool(Allocated);
				}
			}
			ObfDereferenceObject((PVOID)FileObject);
		}
	}
	pIrp->IoStatus.Status=statu;
	pIrp->IoStatus.Information=CopyLength;
	IofCompleteRequest(pIrp,IO_NO_INCREMENT);
	return statu;
}

NTSTATUS UnloadRoutine(IN PDRIVER_OBJECT pDriverObject)
{
	UNICODE_STRING SymbolicLinkName;
	RtlInitUnicodeString(&SymbolicLinkName,L"\\DosDevices\\UnlockerDriver5");
	IoDeleteSymbolicLink(&SymbolicLinkName);
	IoDeleteDevice(pDriverObject->DeviceObject);
}

#pragma INITCODE
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject,IN PUNICODE_STRING pRegistryPath)
{
	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	UNICODE_STRING DeviceName,SymbolicLinkName;

	status=IoCreateDevice(pDriverObject,0,&DeviceName,FILE_DEVICE_UNKNOWN,FILE_DEVICE_SECURE_OPEN,FALSE,&pDevObj);
	if(status == STATUS_SUCCESS)
	{
		for(int i=0;i<IRP_MJ_MAXIMUM_FUNCTION;i++)
		{
			pDriverObject->MajorFunction[i]=DefaultDispatchRoutine;
		}
		pDriverObject->MajorFunction[IRP_MJ_CREATE]=DeviceCreateOrClose;
		pDriverObject->MajorFunction[IRP_MJ_CLOSE]=DeviceCreateOrClose;
		pDriverObject->MajorFunction[IRP_MJ_READ]=DeviceRead;
		pDriverObject->MajorFunction[IRP_MJ_WRITE]=DeviceWrite;
		pDriverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION]=DeviceQueryInfomation;
		pDriverObject->Flags |= DO_DIRECT_IO;
		pDriverObject->Flags &= ~DO_DEVICE_INITIALIZING;
		status=IoCreateSymbolicLink(&SymbolicLinkName,&DeviceName);
	}
	return status;
}
