#include <windows.h>
#include "unlocker.h"

OperateFileClass* OperateFileClass::Initial()
{
	IsFiletoUnlockUnicode=FALSE;
	IsListOfFiletoUnlock=FALSE;
	IsOutputLog=FALSE;
	IsNotShowGUItoUnlock=FALSE;
	Operation=FILENONE;
	Command=NULL;
	ObjectName=NULL;
	return this;
}

int OperateFileClass::GetOperateFileParamFromCmdLine()
{
	int ArgNum;
	LPWSTR* ParamArray=CommandLineToArgvW(GetCommandLineW(),&ArgNum);
	
	if(ArgNum < 2) return 1;
	int i=1;
	while(i<ArgNum)
	{
		LPWSTR cur=ParamArray[i];
		if(!lstrcmpiW(cur,L"-H") || !lstrcmpiW(cur,L"/H") || !lstrcmpiW(cur,L"-?") || !lstrcmpiW(cur,L"/?"))
		{
			ShowMessageBox(
				"Command line usage:\n\n   Unlocker.exe Object [Option]\n\nObject:\n\n   "
				"Complete path including drive to a file or folder\n\nOptions:\n\n   "
				"/H or -H or /? or -?: Display command line usage\n    "
				"/S or -S: Unlock object without showing the GUI\n   "
				"/L or -L: Object is a text file containing the list of files to unlock\n   "
				"/LU or -LU: Similar to /L with a unicode list of files to unlock\n   "
				"/O or -O: Outputs Unlocker-Log.txt log file in Unlocker directory\n   "
				"/D or -D: Delete file\n   "
				"/R Object2 or -R Object2: Rename file, if /L or /LU is set object2 points to a text file containing the new name of files\n   "
				"/M Object2 or -M Object2: Move file, if /L or /LU is set object2 points a text file containing the new location of files\n");
			return 0;
		}
		
		if(i == 1)	Command=ParamArray[1];
		else
		{
			if(!lstrcmpiW(cur,L"-S") || !lstrcmpiW(cur,L"/S"))
				IsNotShowGUItoUnlock=TRUE;
			if(!lstrcmpiW(cur,L"-L") || !lstrcmpiW(cur,L"/L"))
				IsListOfFiletoUnlock=TRUE;
			if(!lstrcmpiW(cur,L"-LU") || !lstrcmpiW(cur,L"/LU"))
			{
				IsListOfFiletoUnlock=TRUE;
				IsFiletoUnlockUnicode=TRUE;
			}
			if(!lstrcmpiW(cur,L"-O") || !lstrcmpiW(cur,L"/O"))
				IsOutputLog=TRUE;
			if(!lstrcmpiW(cur,L"-D") || !lstrcmpiW(cur,L"/D"))
				Operation=FILEDELETE;
			if(!lstrcmpiW(cur,L"-R") || !lstrcmpiW(cur,L"/R"))
			{
				if(i+1<ArgNum)
				{
					Operation=FILERENAME;
					ObjectName=ParamArray[++i];
				}
			}
			if(!lstrcmpiW(cur,L"-M") || !lstrcmpiW(cur,L"/M"))
			{
				if(i+1<ArgNum)
				{
					Operation=FILEMOVE;
					ObjectName=ParamArray[++i];
				}
			}
		}
		i++;
	}
	return 1;
}

OperateFileClass* GetMyOperateFile()
{
	if(!GlobalOperateFile)
	{
		GlobalOperateFile=(OperateFileClass*)GlobalAlloc(GMEM_ZEROINIT,sizeof(OperateFileClass));
		if(GlobalOperateFile) 
			GlobalOperateFile=GlobalOperateFile->Initial();
	}
	return GlobalOperateFile;
}

template<class T>
GeneralClass<T>::GerernalClass()
{
	Buf=NULL;
	MaxSize=0;
	CurPos=0;
	Handle=NULL;
}

template<class T>
void GeneralClass<T>::Construct(T* data)
{
	HGLOBAL newHandle;
	if(CurPos == MaxSize)
	{
		if(MaxSize)
		{
			MaxSize=2*MaxSize;
			GlobalUnlock(Handle);
			newHandle=GlobalReAlloc(Handle,MaxSize*sizeof(T),GMEM_FIXED);
		}
		else
		{
			MaxSize=1;
			newHandle=GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,sizeof(T));
		}
		Handle=newHandle;
		Buf=(T*)GlobalLock(newHandle);
	}
	memcpy(Buf+CurPos++,data,sizeof(T));
}

template<class T>
GeneralClass<T>::Destruct()
{
	if(Handle)
	{
		GlobalUnlock(Handle);
		GlobalFree(Handle);
	}
}

CoreAPIArray* CoreAPIArray::CoreAPIArrayInitial()
{
	ZwQuerySystemInformation=NULL;
	ZwQueryInformationProcess=NULL;
	ZwQueryObject=NULL;
	ZwDeleteFile=NULL;
	RtlInitUnicodeString=NULL;
	RtlAdjustPrivilege=NULL;
	NtLoadDriver=NULL;
	NtUnloadDriver=NULL;
}

BOOL CoreAPIArray::GetCoreAPIArray()
{
	HMODULE hmod=LoadLibrary("ntdll.dll");
	if(hmod)
	{
		ZwQuerySystemInformation=(ZWQUERYSYSTEMINFORMATION)GetProcAddress(hmod,"ZwQuerySystemInformation");
		ZwQueryInformationProcess=(ZWQUERYINFORMATIONPROCESS)GetProcAddress(hmod,"ZwQueryInformationProcess");
		ZwQueryObject=(ZWQUERYOBJECT)GetProcAddress(hmod,"ZwQueryObject");
		ZwDeleteFile=(ZWDELETEFILE)GetProcAddress(hmod,"ZwDeleteFile");
		RtlInitUnicodeString=(RTLINITUNICODESTRING)GetProcAddress(hmod,"RtlInitUnicodeString");
		RtlAdjustPrivilege=(RTLADJUSTPRIVILEGE)GetProcAddress(hmod,"RtlAdjustPrivilege");
		NtLoadDriver=(NTLOADDRIVER)GetProcAddress(hmod,"NtLoadDriver");
		NtUnloadDriver=(NTUNLOADDRIVER)GetProcAddress(hmod,"NtUnloadDriver");
		return TRUE;
	}
	return¡¡FALSE;
}

CoreAPIArray* GetMyCoreAPIArray()
{
	if(!GlobalCoreAPI)
	{
		GlobalCoreAPI=(CoreAPIArray*)GlobalAlloc(GMEM_ZEROINIT,sizeof(CoreAPIArray));
		if(GlobalCoreAPI)
			GlobalCoreAPI=GlobalCoreAPI->CoreAPIArrayInitial();
	}
	return GlobalCoreAPI;
}
