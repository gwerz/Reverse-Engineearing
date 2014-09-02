#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifdef __cplusplus
extern "C" 
{
#endif

#include <ntddk.h>
#include <ntddstor.h>
#include <mountdev.h>
#include <ntddvol.h>
#include <ntddcdrm.h>
#include <ntdddisk.h>
#include <ntddscsi.h>
//#include <ntifs.h>

#ifdef __cplusplus
}
#endif

#pragma pack(push,1)

typedef struct _DEVICE_EXTENSION 
{//sizeof=48
	PDEVICE_OBJECT	DeviceObj;//设备对象
	PFILE_OBJECT	FileObj;//文件对象
	ULONGLONG		FileLength;//文件长度
	ULONG			ChangedTime;//虚拟光驱状态改变次数
	UNICODE_STRING	ustrSymLinkName;//符号链接名
	UNICODE_STRING	FileName;//光盘镜像文件名
	ULONG			FileType;//0 1:UDF 2:ISO
	ULONG			BytePerSec;//扇区大小
	ULONG			SectorBits;//扇区大小比特数BytePerSec=2^SectorBits
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

typedef struct _VCDROM_OPEN_DRIVE //sizeof=54
{
	USHORT 		Index;//盘符序号
	WCHAR 		DriveBuffer[26];
} VCDROM_OPEN_DRIVE, *PVCDROM_OPEN_DRIVE;

typedef struct _VCDROM_OPEN_FILE //sizeof=516
{
	WCHAR  		FullPathFileName[255];
	USHORT 		NameLength;//in byte
	ULONG 		FileType;//0 1:UDF 2:ISO
} VCDROM_OPEN_FILE, *PVCDROM_OPEN_FILE;

typedef struct _VCDROM_GET_FILENAME //sizeof=514
{
	WCHAR  		FullPathFileName[255];
	USHORT 		NameLength;//in byte
	USHORT 		Loaded;//是否加载光盘镜像文件
} VCDROM_GET_FILENAME, *PVCDROM_GET_FILENAME;

#pragma pack(pop)

#define FILE_TYPE_DEFAULT	0x0//默认格式
#define FILE_TYPE_UDF		0x1//UDF格式
#define FILE_TYPE_ISO		0x2//ISO格式

#define SECTOR_SIZE			0x200//默认扇区大小
#define TOC_DATA_TRACK		0x04

#define IOCTL_CDROM_CREATE_VIRTUALDEVICE	CTL_CODE(FILE_DEVICE_CD_ROM,0xCC0,METHOD_BUFFERED,FILE_ANY_ACCESS)//创建虚拟光驱
#define IOCTL_CDROM_DELETE_VIRTUALDEVICE	CTL_CODE(FILE_DEVICE_CD_ROM,0xCC1,METHOD_BUFFERED,FILE_ANY_ACCESS)//删除虚拟光驱
#define IOCTL_CDROM_CDROM_LOADIMAGE			CTL_CODE(FILE_DEVICE_CD_ROM,0xCC2,METHOD_BUFFERED,FILE_ANY_ACCESS)//加载光盘镜像
#define IOCTL_CDROM_GET_EXISTINGDEVICE		CTL_CODE(FILE_DEVICE_CD_ROM,0xCC3,METHOD_BUFFERED,FILE_ANY_ACCESS)//获取创建过的虚拟光驱盘符
#define IOCTL_CDROM_GET_FILENAME			CTL_CODE(FILE_DEVICE_CD_ROM,0xCC4,METHOD_BUFFERED,FILE_ANY_ACCESS)//获取虚拟光驱中已经加载的全路径文件名