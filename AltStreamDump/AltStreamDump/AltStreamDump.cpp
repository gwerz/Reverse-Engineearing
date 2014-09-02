#include <windows.h>
#include <stdio.h>
#include "NecessaryDef.h"
#include "resource.h"

HANDLE hConsoleOutput;

BOOL MyLoadLib(HMODULE& hmod)
{
	if(hmod == NULL)
	{
		hmod=LoadLibraryW(L"advapi32.dll");
	}
	return hmod != NULL;
}

DWORD GetPrivilege(HMODULE& hmod)
{
	HANDLE curp=GetCurrentProcess();
	if(!MyLoadLib(hmod))
		return GetLastError();

	OPENPROCESSTOEKN MyOpenProcessToken=(OPENPROCESSTOEKN)GetProcAddress(hmod,"OpenProcessToken");
	HANDLE TokenHandle;
	if(!MyOpenProcessToken || !MyOpenProcessToken(curp,TOKEN_ADJUST_GROUPS,&TokenHandle))
		return GetLastError();

	LOOKUPPRIVILEGEVALUE MyLookupPrivilege=(LOOKUPPRIVILEGEVALUE)GetProcAddress(hmod,"LookupPrivilegeValueW");
	TOKEN_PRIVILEGES NewState;
	if(MyLookupPrivilege)	
		MyLookupPrivilege(NULL,L"SeBackupPrivilege",&NewState.Privileges[0].Luid);
	NewState.PrivilegeCount=1;
	NewState.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;

	ADJUSTTOKENPRIVILEGES MyAdjustPrivilege=(ADJUSTTOKENPRIVILEGES)GetProcAddress(hmod,"AdjustTokenPrivileges");
	if(MyAdjustPrivilege)
		MyAdjustPrivilege(curp,FALSE,&NewState,0,NULL,NULL);
	DWORD err=GetLastError();
	CloseHandle(TokenHandle);
	return err;
}

int wmain(int argc,wchar_t** argv)
{
	hConsoleOutput=GetStdHandle(STD_OUTPUT_HANDLE);
	MainClass mymain;
	for(int i=1;i<argc-1;i++)
	{
		mymain.SetParam(argv[i],argv[i+1]);
	}

	if(argc > 1 && !wcscmp(argv[1],L"-h"))
	{
		HRSRC rs=FindResourceW(NULL,MAKEINTRESOURCEW(IDR_HELP),L"BIN");
		HGLOBAL rc;
		char* str=NULL;
		if(rs && (rc=LoadResource(NULL,rs)))
			str=(char*)LockResource(rc);
		printf(str);
	}
	else
	{
		HMODULE hmod=NULL;
		GetPrivilege(hmod);
		mymain.GoSearch();
		wprintf(L"\r\n\r\nType AltStreamDump -h to get more information about this tool.\r\n");
		if(hmod)
			FreeLibrary(hmod);
	}

	return 0;
}
