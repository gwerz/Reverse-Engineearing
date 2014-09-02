#ifndef UNICODE
#define UNICODE

#include <windows.h>
#include <winternl.h>
#include <stdio.h>


bool __fastcall IsWow64Process()
{//判断是否64位进程
	ULONG_PTR info;
	return NtQueryInformationProcess(GetCurrentProcess(),ProcessWow64Information,&info,sizeof(ULONG_PTR),NULL) && info;
}

void main()
{
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInformation;
	WCHAR CommandLine[522],sysdir[261],command[261];
	
	ProcessInformation.hProcess=NULL;
	ProcessInformation.hThread=NULL;
	ProcessInformation.dwProcessId=0;
	ProcessInformation.dwThreadId=0;
	memset(&StartupInfo,0,sizeof(STARTUPINFO));
	memset(sysdir,0,sizeof(sysdir));
	memset(command,0,sizeof(command));
	memset(CommandLine,0,sizeof(CommandLine));
	StartupInfo.cb=sizeof(STARTUPINFO);
	StartupInfo.lpReserved=NULL;
	UINT num=GetSystemDirectory(sysdir,261);
	if(num)
	{
		WCHAR* lastchar=sysdir+num-1;
		if(*lastchar == L'\\')
			*lastchar=L'\0';
		swprintf(command,L"%s\\mmc.exe",sysdir);
		if(IsWow64Process())
		{//这里不理解，该结构体全网都查不到
			DWORD unknown=*(DWORD*)(__readfsdword(0x18)+3952);
			*(LPWSTR*)(unknown+5312)=command;
			*(LPWSTR*)(unknown+5316)=NULL;
		}
		swprintf(CommandLine,L"%s %s\\com\\comexp.msc",command,sysdir);
		if(CreateProcess(NULL,CommandLine,NULL,NULL,FALSE,0,NULL,NULL,&StartupInfo,&ProcessInformation))
		{
			CloseHandle(ProcessInformation.hProcess);
			CloseHandle(ProcessInformation.hThread);
			return;
		}
	}
	LPWSTR* buf=NULL;
	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL,GetLastError(),LOCALE_USER_DEFAULT,(LPWSTR)buf,0,NULL))
	{
		MessageBox(NULL,*buf, L"DCOMCnfg",MB_ICONHAND);
		LocalFree(buf);
	}
}

#endif