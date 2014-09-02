#include <windows.h>
#include "resource.h"
#pragma comment(lib,"version.lib")

struct VersionStruct
{
	LPVOID lpVersionBuffer;
	LPVOID	lpXlate;/////////有误。。。。。。。。。。。。。。。。
	DWORD	dwVersionSize;
	DWORD	dwHandle;
	WCHAR	szVersionKey[60];
	DWORD	cchXlateString;
	DWORD	cXlate;
	HGLOBAL	hmemVersion;
	HLOCAL	pszXlate;
};

VersionStruct VS={NULL,NULL,0,0,L"",0,0,NULL,NULL};

LPCWSTR WINAPI lstrcati2(LPWSTR lpString,LPCWSTR lpString2,int maxpath/*maxlength*/)
{
	if(lpString && lpString2 && maxpath>0)
	{
		int len=lstrlenW(lpString);
		lstrcpynW(lpString+len,lpString2,maxpath-len);
	}
	return lpString;
}

LPCWSTR WINAPI GetVersionDatum(LPCWSTR lpBuffer)
{
	unsigned int puLen;

	if(VS.hmemVersion == NULL) return NULL;
	lstrcpyW(VS.szVersionKey+25,lpBuffer);
	VerQueryValueW(VS.lpVersionBuffer,VS.szVersionKey,(void**)&lpBuffer,&puLen);
	if(puLen) return lpBuffer;
	else return NULL;
}

void FreeVersionInfo()
{
	VS.lpVersionBuffer=NULL;
	VS.dwHandle=NULL;
	if(VS.hmemVersion)
	{
		GlobalUnlock(VS.hmemVersion);
		GlobalFree(VS.hmemVersion);
		VS.hmemVersion=0;
	}
	if(VS.pszXlate)
	{
		LocalFree(VS.pszXlate);
		VS.pszXlate=0;
	}
}

LPCWSTR WINAPI GetVersionInfo(WCHAR* dest,const WCHAR* src)
{
	unsigned int puLen=0;
	if(VS.hmemVersion) FreeVersionInfo();
	lstrcati2(dest,L"\\",MAX_PATH);
	lstrcati2(dest,src,MAX_PATH);
	VS.dwVersionSize=GetFileVersionInfoSizeW(dest,&VS.dwHandle);
	if(VS.dwVersionSize==0 || (VS.hmemVersion=GlobalAlloc(GMEM_ZEROINIT,VS.dwVersionSize))==NULL) return NULL;
	VS.lpVersionBuffer=GlobalLock(VS.hmemVersion);
	if(FALSE == GetFileVersionInfoW(dest,VS.dwHandle,VS.dwVersionSize,VS.lpVersionBuffer)) return 0;
	
	VerQueryValueW(VS.lpVersionBuffer,L"\\VarFileInfo\\Translation",&VS.lpXlate,&puLen);
	if(puLen)
	{
		VS.cXlate=puLen>>2;
		VS.cchXlateString=45*(puLen>>2);
		VS.pszXlate=LocalAlloc(GMEM_ZEROINIT,90*(puLen>>2));
	}
	else VS.lpXlate=NULL;
	wsprintfW(VS.szVersionKey,L"\\StringFileInfo\\%04X04B0\\",GetThreadLocale());
	
	LPCWSTR result=GetVersionDatum(L"InternalName");
	if(result) return result;
	if(puLen) 
	{
		wsprintfW(VS.szVersionKey,L"\\StringFileInfo\\%04X%04X\\",LOWORD(VS.lpXlate),HIWORD(VS.lpXlate));
		if((result=GetVersionDatum(L"InternalName"))!=0) return result;
	}
	lstrcpyW(VS.szVersionKey,L"\\StringFileInfo\\040904B0\\");
	if((result=GetVersionDatum(L"InternalName"))!=0) return result;
	lstrcpyW(VS.szVersionKey,L"\\StringFileInfo\\040904E4\\");
	if((result=GetVersionDatum(L"InternalName"))!=0) return result;
	lstrcpyW(VS.szVersionKey,L"\\StringFileInfo\\04090000\\");
	return GetVersionDatum(L"InternalName");
}

void  main()
{
	WCHAR WinDir[MAX_PATH]=L"";
	WCHAR String[MAX_PATH]=L"";
	WCHAR OtherString[MAX_PATH]=L"";
	BOOL flag=0;
	STARTUPINFOW StartupInfo;
	PROCESS_INFORMATION ProcessInformation;
	LPWSTR  next;
	BOOL hasquote=FALSE;
	wchar_t symbol[]=L"\\";
	wchar_t stringTT[]=L"winhstb",string1[]=L"winhlp32",string2[]=L".exe";

	LPWSTR command=GetCommandLineW();
	while(*command == '"')//原始:*command && *command=='"'     感觉没必要
	{
		command++;
		hasquote=TRUE;
	}
	GetSystemWindowsDirectoryW(WinDir,MAX_PATH);
	lstrcpyW(String,WinDir);
	lstrcati2(WinDir,symbol,MAX_PATH);
	lstrcati2(WinDir,string1,MAX_PATH);
	lstrcati2(WinDir,string2,MAX_PATH);
	lstrcpyW(OtherString,string1);
	lstrcati2(OtherString,string2,MAX_PATH);

	LPCWSTR versioninfo=GetVersionInfo(String,OtherString);

	if(versioninfo && CompareStringW(LOCALE_USER_DEFAULT,NORM_IGNORECASE/*ignore case*/,
		versioninfo,lstrlenW(versioninfo),stringTT,lstrlenW(stringTT))==CSTR_EQUAL)
	{
		FreeVersionInfo();
		LoadStringW(NULL,ERROR1,WinDir,MAX_PATH);
		MessageBoxW(NULL,string2,NULL,MB_OK | MB_ICONHAND );
		ExitProcess(1);
	}

	FreeVersionInfo();
	if(hasquote)
	{
		LPWSTR ptr=command;
		while(*command)
		{
			if(*command == '"') break;
			command++;
		}
		if(*command)
		{
			next=command+1;
			if(next[0] && next[1])
			{
				if(CompareStringW(LOCALE_USER_DEFAULT,NORM_IGNORECASE,next,lstrlenW(string2),
					string2,lstrlenW(string2)) == CSTR_EQUAL)
					next+=lstrlenW(string2);
				if(flag == 0)
				{
					while(*next == '"') next++;
					lstrcati2(WinDir,L" ",MAX_PATH);
					lstrcati2(WinDir,next,MAX_PATH);
				}
				memset(&StartupInfo,0,sizeof(STARTUPINFOW));
				StartupInfo.cb=sizeof(StartupInfo);
				StartupInfo.lpDesktop=NULL;
				StartupInfo.lpTitle=NULL;
				StartupInfo.cbReserved2=0;
				StartupInfo.lpReserved2=0;
				StartupInfo.wShowWindow=SW_SHOW;
				StartupInfo.dwFlags=STARTF_FORCEONFEEDBACK | STARTF_RUNFULLSCREEN | 
					STARTF_USEPOSITION | STARTF_USESHOWWINDOW;
				
				if(CreateProcessW(NULL,string2,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,
					NULL,NULL,&StartupInfo,&ProcessInformation))
				{
					WaitForSingleObject(ProcessInformation.hProcess,INFINITE);
					CloseHandle(ProcessInformation.hProcess);
					CloseHandle(ProcessInformation.hThread);
					ExitProcess(0);
				}
				else ExitProcess(1);
			}
		}
		flag=1;
		command=ptr;
		while(*command)
		{
			if(CompareStringW(LOCALE_USER_DEFAULT,NORM_IGNORECASE,command,lstrlenW(string1),
					string1,lstrlenW(string1)) == CSTR_EQUAL)
				break;
			command++;
		}
		if(*command == L'\0')
		{
			LoadStringW(NULL,ERROR2,WinDir,MAX_PATH);
			MessageBoxW(NULL,WinDir,NULL,MB_OK | MB_ICONHAND );
			ExitProcess(1);
		}
		next=command+lstrlenW(string1);
		if(CompareStringW(LOCALE_USER_DEFAULT,NORM_IGNORECASE,next,lstrlenW(string2),
					string2,lstrlenW(string2)) == CSTR_EQUAL)
			next+=lstrlenW(string2);

		if(flag == 0)
		{
			while(*next == '"') next++;
			lstrcati2(WinDir,L" ",MAX_PATH);
			lstrcati2(WinDir,next,MAX_PATH);
		}
		memset(&StartupInfo,0,sizeof(STARTUPINFOW));
		StartupInfo.cb=sizeof(StartupInfo);
		StartupInfo.lpDesktop=NULL;
		StartupInfo.lpTitle=NULL;
		StartupInfo.cbReserved2=0;
		StartupInfo.lpReserved2=0;
		StartupInfo.wShowWindow=SW_SHOW;
		StartupInfo.dwFlags=STARTF_FORCEONFEEDBACK | STARTF_RUNFULLSCREEN | 
			STARTF_USEPOSITION | STARTF_USESHOWWINDOW;
		
		if(CreateProcessW(NULL,WinDir,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,
			NULL,NULL,&StartupInfo,&ProcessInformation))
		{
			WaitForSingleObject(ProcessInformation.hProcess,INFINITE);
			CloseHandle(ProcessInformation.hProcess);
			CloseHandle(ProcessInformation.hThread);
			ExitProcess(0);
		}
		else ExitProcess(1);
	}

	while(*command)
	{
		if(CompareStringW(LOCALE_USER_DEFAULT,NORM_IGNORECASE,command,lstrlenW(string1),
					string1,lstrlenW(string1)) == CSTR_EQUAL)
			break;
		command++;
	}
	if(*command)
	{
		next=command+lstrlenW(string1);
		if(CompareStringW(LOCALE_USER_DEFAULT,NORM_IGNORECASE,next,lstrlenW(string2),
			string2,lstrlenW(string2)) == CSTR_EQUAL)
			next+=lstrlenW(string2);
		
		if(flag == 0)
		{
			while(*next == '"') next++;
			lstrcati2(WinDir,L" ",MAX_PATH);
			lstrcati2(WinDir,next,MAX_PATH);
		}
		memset(&StartupInfo,0,sizeof(STARTUPINFOW));
		StartupInfo.cb=sizeof(StartupInfo);
		StartupInfo.lpDesktop=NULL;
		StartupInfo.lpTitle=NULL;
		StartupInfo.cbReserved2=0;
		StartupInfo.lpReserved2=0;
		StartupInfo.wShowWindow=SW_SHOW;
		StartupInfo.dwFlags=STARTF_FORCEONFEEDBACK | STARTF_RUNFULLSCREEN | 
			STARTF_USEPOSITION | STARTF_USESHOWWINDOW;
		
		if(CreateProcessW(NULL,string2,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,
			NULL,NULL,&StartupInfo,&ProcessInformation))
		{
			WaitForSingleObject(ProcessInformation.hProcess,INFINITE);
			CloseHandle(ProcessInformation.hProcess);
			CloseHandle(ProcessInformation.hThread);
			ExitProcess(0);
		}
		else ExitProcess(1);
	}
	LoadStringW(NULL,ERROR2,WinDir,MAX_PATH);
	MessageBoxW(NULL,WinDir,NULL,MB_OK | MB_ICONHAND );
	ExitProcess(1);
}
