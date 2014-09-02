#include "stdafx.h"

#ifdef __cplusplus
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING  RegistryPath);
#endif

NTSTATUS RtlULongAdd(ULONG InSize,ULONG ParameterLength,PULONG OutSize)
{
	if(InSize+ParameterLength < InSize)
	{
		*OutSize=-1;
		return STATUS_INTEGER_OVERFLOW;
	}
	else
	{
		*OutSize=InSize+ParameterLength;
		return STATUS_SUCCESS;
	}
}

USBD_STATUS USBD_ValidateConfigurationDescriptorInternal(PUSB_CONFIGURATION_DESCRIPTOR ConfigDesc, ULONG BufferLength, 
		USHORT Level, PUCHAR *Offset, ULONG Tag)
{
	USBD_STATUS status=USBD_STATUS_SUCCESS;
	if(!Tag) Tag='NbsU';
	if(!ConfigDesc || BufferLength < sizeof(USB_CONFIGURATION_DESCRIPTOR) || !Offset)
		return USBD_STATUS_BUFFER_TOO_SMALL;

	if(ConfigDesc->bLength < sizeof(USB_CONFIGURATION_DESCRIPTOR))
	{
		status=USBD_STATUS_BAD_DESCRIPTOR_BLEN;
		*Offset=(PUCHAR)ConfigDesc;
		return status;
	}
	if(ConfigDesc->bDescriptorType != USB_CONFIGURATION_DESCRIPTOR_TYPE)
	{
		status=USBD_STATUS_BAD_DESCRIPTOR_TYPE;
		*Offset=&ConfigDesc->bDescriptorType;
		return status;
	}
	if(ConfigDesc->wTotalLength <= BufferLength && ConfigDesc->wTotalLength <= sizeof(USB_CONFIGURATION_DESCRIPTOR)*(ConfigDesc->bNumInterfaces+1))
	{
		status=USBD_STATUS_BAD_CONFIG_DESC_LENGTH;
		*Offset=(PUCHAR)&ConfigDesc->wTotalLength;
		return status;
	}

	if(Level <= 1 || ConfigDesc->bNumInterfaces == 0)
	{
		*Offset=NULL;
		return status;
	}

	return status;
}

NTSTATUS DllUnload(void)
{
	return STATUS_SUCCESS;
}

NTSTATUS DllInitialize(PUNICODE_STRING RegistryPath)
{
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING  RegistryPath)
{
	return STATUS_SUCCESS;
}

void USBD_GetUSBDIVersion(PUSBD_VERSION_INFORMATION VersionInformation)
{
	if(VersionInformation)
	{
		VersionInformation->USBDI_Version=USBDI_VERSION;//Windows Vista,Windows 7,Windows8
		VersionInformation->Supported_USB_Version=0x200;
	}
}

PUSB_COMMON_DESCRIPTOR USBD_ParseDescriptors(PVOID DescriptorBuffer, ULONG TotalLength, PVOID StartPosition, LONG DescriptorType)
{
	PUSB_COMMON_DESCRIPTOR itor=(PUSB_COMMON_DESCRIPTOR)StartPosition;
	PUSB_COMMON_DESCRIPTOR result=NULL;
	while((UCHAR*)itor < (UCHAR*)DescriptorBuffer+TotalLength)
	{
		if(itor->bDescriptorType == DescriptorType)
		{
			result=itor;
			break;
		}
		if(itor->bLength == 0)
			return NULL;
		itor=PUSB_COMMON_DESCRIPTOR((UCHAR*)itor+itor->bLength);
	}
	return result;
}

PUSB_INTERFACE_DESCRIPTOR USBD_ParseConfigurationDescriptorEx(PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor, 
	PVOID StartPosition, LONG InterfaceNumber, LONG AlternateSetting, LONG InterfaceClass, LONG InterfaceSubClass, LONG InterfaceProtocol)
{
	PVOID itor=StartPosition;
	PUSB_INTERFACE_DESCRIPTOR pid=NULL;
	while(!pid)
	{
		PUSB_COMMON_DESCRIPTOR pcd=USBD_ParseDescriptors(ConfigurationDescriptor,ConfigurationDescriptor->wTotalLength,itor,USB_INTERFACE_DESCRIPTOR_TYPE);
		if(!pcd)
			break;
		pid=(PUSB_INTERFACE_DESCRIPTOR)pcd;
		if((InterfaceNumber != -1 && pid->bInterfaceNumber != InterfaceNumber) ||
			(AlternateSetting != -1 && pid->bAlternateSetting != AlternateSetting) ||
			(InterfaceClass != -1 && pid->bInterfaceClass != InterfaceClass) ||
			(InterfaceSubClass != -1 && pid->bInterfaceSubClass != InterfaceSubClass) ||
			(InterfaceProtocol != -1 && pid->bInterfaceProtocol != InterfaceProtocol))
		{
			itor=PVOID((UCHAR*)pcd+pid->bLength);
		}
	}
	return pid;
}

PUSB_INTERFACE_DESCRIPTOR USBD_ParseConfigurationDescriptor(PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor, UCHAR InterfaceNumber, UCHAR AlternateSetting)
{
	return USBD_ParseConfigurationDescriptorEx(ConfigurationDescriptor,ConfigurationDescriptor,InterfaceNumber,AlternateSetting,-1,-1,-1);
}

PURB USBD_CreateConfigurationRequestEx(PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor, USBD_INTERFACE_LIST_ENTRY *InterfaceList)
{
	PUSBD_INTERFACE_LIST_ENTRY itorl=InterfaceList;
	USHORT size=sizeof(_URB_SELECT_CONFIGURATION)-sizeof(USBD_INTERFACE_INFORMATION);//_URB_SELECT_CONFIGURATION头部大小
	while(itorl->InterfaceDescriptor)
	{//一个URB_SELECT_CONFIGURATION头部接多个USBD_INTERFACE_INFORMATION，一个USBD_INTERFACE_INFORMATION头部接多个USBD_PIPE_INFORMATION
		size += sizeof(USBD_PIPE_INFORMATION)*itorl->InterfaceDescriptor->bNumEndpoints+sizeof(USBD_INTERFACE_INFORMATION)-sizeof(USBD_PIPE_INFORMATION);
		itorl++;
	}
	PURB pUrb=(PURB)ExAllocatePoolWithTag(NonPagedPool,size,'DBSU');
	if(!pUrb)
		return NULL;
	RtlZeroMemory(pUrb,size);
	PUSBD_INTERFACE_INFORMATION itorc=&pUrb->UrbSelectConfiguration.Interface;
	itorl=InterfaceList;
	while(itorl->InterfaceDescriptor)
	{
		UCHAR num=0;
		itorc->InterfaceNumber=itorl->InterfaceDescriptor->bInterfaceNumber;
		itorc->AlternateSetting=itorl->InterfaceDescriptor->bAlternateSetting;
		itorc->NumberOfPipes=itorl->InterfaceDescriptor->bNumEndpoints;
		for(UCHAR num=0;num < itorl->InterfaceDescriptor->bNumEndpoints;num++)
		{
			itorc->Pipes[num].MaximumTransferSize=0xFFFFFFFF;
			itorc->Pipes[num].PipeFlags=UsbdPipeTypeControl;
		}
		itorc->Length= sizeof(USBD_PIPE_INFORMATION)*itorl->InterfaceDescriptor->bNumEndpoints+sizeof(USBD_INTERFACE_INFORMATION)-sizeof(USBD_PIPE_INFORMATION);
		itorl->Interface=itorc;
		itorl++;
		itorc=PUSBD_INTERFACE_INFORMATION((UCHAR*)itorc+itorc->Length);
	}
	pUrb->UrbSelectConfiguration.Hdr.Length=size;
	pUrb->UrbSelectConfiguration.Hdr.Function=URB_FUNCTION_SELECT_CONFIGURATION;
	pUrb->UrbSelectConfiguration.ConfigurationDescriptor=ConfigurationDescriptor;
	return pUrb;
}

PURB USBD_CreateConfigurationRequest(PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor, PUSHORT Size)
{
	PUSBD_INTERFACE_LIST_ENTRY list=(PUSBD_INTERFACE_LIST_ENTRY)ExAllocatePoolWithTag(PagedPool,
			sizeof(USBD_INTERFACE_LIST_ENTRY)*(ConfigurationDescriptor->bNumInterfaces+1),'DBSU');
	if(!list)
		return NULL;
	LONG InterfaceNumber=0;
	PURB pUrb=NULL;
	while(ConfigurationDescriptor->bNumInterfaces > InterfaceNumber)
	{
		PUSB_INTERFACE_DESCRIPTOR pid=USBD_ParseConfigurationDescriptorEx(ConfigurationDescriptor,(PVOID)ConfigurationDescriptor,
				InterfaceNumber,0,-1,-1,-1);
		if(!pid)
			break;
		list[InterfaceNumber].InterfaceDescriptor=pid;
		InterfaceNumber++;
	}
	if(ConfigurationDescriptor->bNumInterfaces <= InterfaceNumber)
	{
		list[InterfaceNumber].InterfaceDescriptor=NULL;
		pUrb=USBD_CreateConfigurationRequestEx(ConfigurationDescriptor,list);
	}
	ExFreePool(list);

	if(pUrb)
		*Size=pUrb->UrbSelectConfiguration.Hdr.Length;
	else
		*Size=0;
	return pUrb;
}

ULONG USBD_InternalGetInterfaceLength(PUSB_INTERFACE_DESCRIPTOR InterfaceDescriptor, PUCHAR BufferEnd)
{
	ULONG Len=InterfaceDescriptor->bLength;
	UCHAR ptNum=InterfaceDescriptor->bNumEndpoints;
	PUSB_INTERFACE_DESCRIPTOR itor=PUSB_INTERFACE_DESCRIPTOR((UCHAR*)InterfaceDescriptor+Len);

	if(ptNum)
	{
		do 
		{
			while(TRUE)
			{
				Len += itor->bLength;
				if(itor->bDescriptorType != USB_ENDPOINT_DESCRIPTOR_TYPE )
					break;
				itor=PUSB_INTERFACE_DESCRIPTOR((UCHAR*)itor+itor->bLength);
				if(--ptNum == 0)
					goto GETLEN;
			}
		} 
		while((PUCHAR)itor < BufferEnd && itor->bLength);
	}
GETLEN:
	while((PUCHAR)itor < BufferEnd && itor->bDescriptorType != USB_INTERFACE_DESCRIPTOR_TYPE)
	{
		Len += itor->bLength;
		itor=PUSB_INTERFACE_DESCRIPTOR((UCHAR*)itor+itor->bLength);
	}
	return Len;
}

ULONG USBD_GetInterfaceLength(PUSB_INTERFACE_DESCRIPTOR InterfaceDescriptor, PUCHAR BufferEnd)
{
	return USBD_InternalGetInterfaceLength(InterfaceDescriptor,BufferEnd);
}

NTSTATUS USBD_GetRegistryKeyValue(HANDLE KeyHandle, PCWSTR KeyName, ULONG KeyNameLength, PVOID Parameter, ULONG ParameterLength)
{
	NTSTATUS status;
	UNICODE_STRING UKeyName;
	RtlInitUnicodeString(&UKeyName,KeyName);
	ULONG Result;
	status=RtlULongAdd(sizeof(KEY_VALUE_FULL_INFORMATION),KeyNameLength,&Result);
	if(!(NT_SUCCESS(status)))
		return status;
	status=RtlULongAdd(Result,ParameterLength,&Result);
	if(!(NT_SUCCESS(status)))
		return status;
	PKEY_VALUE_FULL_INFORMATION info=(PKEY_VALUE_FULL_INFORMATION)ExAllocatePoolWithTag(PagedPool,Result,'DBSU');
	if(info)
	{
		ULONG RetLen;
		status=ZwQueryValueKey(KeyHandle,&UKeyName,KeyValueFullInformation,(PVOID)info,Result,&RetLen);
		if(NT_SUCCESS(status))
			RtlCopyMemory(Parameter,(UCHAR*)info+info->DataOffset,ParameterLength);
		ExFreePool(info);
	}
	return status;
}

NTSTATUS USBD_GetPdoRegistryParameter(PDEVICE_OBJECT DevInstRegKey, PVOID Parameter, ULONG ParameterLength, PCWSTR KeyName, ULONG KeyNameLength)
{
	HANDLE KeyHandle;
	NTSTATUS status=IoOpenDeviceRegistryKey(DevInstRegKey,PLUGPLAY_REGKEY_DRIVER,KEY_ALL_ACCESS,&KeyHandle);
	if(NT_SUCCESS(status))
	{
		status=USBD_GetRegistryKeyValue(KeyHandle,KeyName,KeyNameLength,Parameter,ParameterLength);
		ZwClose(DevInstRegKey);
	}
	return status;
}

USBD_STATUS USBD_QueryBusTime(PDEVICE_OBJECT RootHubPdo, PULONG CurrentFrame)
{
	return USBD_STATUS_NOT_SUPPORTED;
}

VOID USBD_RegisterHcFilter(PDEVICE_OBJECT DeviceObject, PDEVICE_OBJECT FilterDeviceObject)
{

}

ULONG USBD_CalculateUsbBandwidth(ULONG MaxPacketSize, UCHAR EndpointType, BOOLEAN LowSpeed)
{
    ULONG OverheadTable[] = 
	{
		0x00, /* UsbdPipeTypeControl */
		0x09, /* UsbdPipeTypeIsochronous */
		0x00, /* UsbdPipeTypeBulk */
		0x0d  /* UsbdPipeTypeInterrupt */
	};
	if(EndpointType >= 4)//未定义管道类型
		return 0;
	ULONG Result=0;
	if(OverheadTable[EndpointType])
		Result=((MaxPacketSize + OverheadTable[EndpointType]) * 8 * 7) / 6;
	if(LowSpeed)
		Result <<= 3;
	return Result;
}

USBD_STATUS USBD_ValidateConfigurationDescriptor(PUSB_CONFIGURATION_DESCRIPTOR ConfigDesc, ULONG BufferLength, USHORT Level, PUCHAR *Offset, ULONG Tag)
{
	if(ConfigDesc && Offset)
		return USBD_ValidateConfigurationDescriptorInternal(ConfigDesc,BufferLength,Level,Offset,Tag);
	else
		return STATUS_INVALID_PARAMETER;
}
