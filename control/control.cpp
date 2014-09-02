#ifndef UNICODE
#define UNICODE

#include <windows.h>
#include <shlwapi.h>
#include <cpl.h>

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"IMM32.lib")

struct Cplstruct
{
	LPWSTR CplServiceName;
	DWORD dwOS;
	LPWSTR CplFileName;
	LPWSTR parameter;
};

typedef LONG (WINAPI* CPLAPPLET)(HWND hwndCPl,UINT uMsg,LPARAM lParam1,LPARAM lParam2);

Cplstruct CplsTable[21]=
{
	{	L"DESKTOP",						-1,		L"desk.cpl",		NULL						},
	{	L"COLOR",						-1,		L"desk.cpl",		L",2"						},
	{	L"DATE/TIME",					-1,		L"timedate.cpl",	NULL						},
	{	L"PORTS",						-1,		L"sysdm.cpl",		L",1"						},
	{	L"INTERNATIONAL",				-1,		L"intl.cpl",		NULL						},
	{	L"MOUSE",						-1,		L"main.cpl",		NULL						},
	{	L"KEYBOARD",					-1,		L"main.cpl",		L"@1"						},
	{	L"NETWARE",						-1,		L"nwc.cpl",			NULL						},
	{	L"TELEPHONY",					-1,		L"telephon.cpl",	NULL						},
	{	L"INFRARED",					-1,		L"irprops.cpl",		NULL						},
	{	L"USERPASSWORDS",	OS_ANYSERVER,		L"lusrmgr.msc",		NULL						},
	{	L"USERPASSWORDS",	OS_XPORGREATER,		L"nusrmgr.cpl",		NULL						},
	{	L"USERPASSWORDS2",				-1,		L"rundll32.exe",	L"netplwiz.dll,UsersRunDll"	},
	{	L"PRINTERS",					-1,		L"explorer.exe",	L"\"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\::{2227A280-3AEA-1069-A2DE-08002B30309D}\""	},
	{	L"FONTS",						-1,		L"explorer.exe",	L"\"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\::{D20EA4E1-3957-11d2-A40B-0C5020524152}\"" },
	{	L"ADMINTOOLS",					-1,		L"explorer.exe",	L"\"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\::{D20EA4E1-3957-11d2-A40B-0C5020524153}\"" },
	{	L"SCHEDTASKS",					-1,		L"explorer.exe",	L"\"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\::{D6277990-4C6A-11CF-8D87-00AA0060F5BF}\"" },
	{	L"NETCONNECTIONS",				-1,		L"explorer.exe",	L"\"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\::{7007ACC7-3202-11D1-AAD2-00805FC1270E}\"" },
	{	L"FOLDERS",						-1,		L"rundll32.exe",	L"shell32.dll,Options_RunDLL 0"},
	{	L"SCANNERCAMERA",				-1,		L"explorer.exe",	L"\"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\::{E211B736-43FD-11D1-9EFB-0000F8757FCD}\"" },
	{	L"STICPL.CPL",					-1,		L"explorer.exe",	L"\"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\::{E211B736-43FD-11D1-9EFB-0000F8757FCD}\"" }
};

LSTATUS WINAPI GetRegisteredCplPath(LPCWSTR compstring,LPWSTR buffer,DWORD remainbuffersize)
{
	LSTATUS statu;
	WCHAR Src[520],Dst[520],ValueName[260];
	WCHAR SubKey[62]=L"Software\\Microsoft\\Windows\\CurrentVersion\\Control Panel\\Cpls";
	HKEY hKey[2]={HKEY_LOCAL_MACHINE,HKEY_CURRENT_USER};
	HKEY hkResult;


	statu=ERROR_INSUFFICIENT_BUFFER;
	if(remainbuffersize)
	{
		buffer[0]=L'\0';
		for(int i=0;i<2;i++)
		{
			if(buffer[0]) break;
			statu=RegOpenKeyEx(hKey[i],SubKey,0,KEY_QUERY_VALUE,&hkResult);
			if(statu == ERROR_SUCCESS)
			{
				int index=0;
				while(buffer[0] == L'\0')
				{
					DWORD cchValueName=260,cbData=sizeof(Src),Type;
					statu=RegEnumValue(hkResult,index,ValueName,&cchValueName,NULL,&Type,(LPBYTE)Src,&cbData);
					if(statu != ERROR_SUCCESS) break;
					if(cbData>2 && (Type == REG_SZ || Type == REG_EXPAND_SZ) && (lstrcmpi(compstring,ValueName) == 0))
					{
						DWORD returnnum=ExpandEnvironmentStrings(Src,Dst,520);
						if(returnnum != 0 && returnnum+2 < remainbuffersize)
							wsprintf(buffer,L"\"%s\"");
					}
				}
				RegCloseKey(hkResult);
			}
		}
	}

	return statu;
}

void ProcessPolicy()
{

	HMODULE hmod=LoadLibrary(L"desk.cpl");
	if(hmod)
	{
		CPLAPPLET CPlApplet=(CPLAPPLET)GetProcAddress(hmod,"CPlApplet");
		if(CPlApplet)
			CPlApplet(NULL,102/*unknownmessage*/,0,0);
		FreeLibrary(hmod);
	}
}

LRESULT CALLBACK DummyControlPanelProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	SHELLEXECUTEINFO pExecInfo;

	switch(Msg)
	{
		case WM_CREATE:
			SetTimer(hWnd,1,10000,NULL);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_COMMAND:
			if(wParam == -1)//??
			{
				memset(&pExecInfo,0,sizeof(SHELLEXECUTEINFO));
				pExecInfo.cbSize=sizeof(SHELLEXECUTEINFO);
				pExecInfo.fMask=SEE_MASK_WAITFORINPUTIDLE | SEE_MASK_NOASYNC;
				pExecInfo.lpFile=L"explorer.exe";
				pExecInfo.lpParameters=L"\"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\::{2227A280-3AEA-1069-A2DE-08002B30309D}\"";
				pExecInfo.nShow=SW_SHOWNORMAL;
				ShellExecuteEx(&pExecInfo);
			}
			break;

		case WM_TIMER:
			if(wParam == 1)
				//The wParam parameter of the WM_TIMER message contains the value of the nIDEvent parameter. 
				//SetTimer(hWnd,1,10000,NULL);ÖÐµÄnIDEvent=1
				DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd,Msg,wParam,lParam);
	}
	return 0;
}

HWND WINAPI CreateDummyControlPanel(HINSTANCE hInstance)
{
	WNDCLASS WndClass;
	WndClass.style=0;
	WndClass.lpfnWndProc=DummyControlPanelProc;
	WndClass.cbClsExtra=0;
	WndClass.hInstance=hInstance;
	WndClass.hIcon=NULL;
	WndClass.hCursor=NULL;
	WndClass.hbrBackground=COLOR_SCROLLBAR;
	WndClass.lpszMenuName=NULL;
	WndClass.lpszClassName=L"CtlPanelClass";
	RegisterClass(&WndClass);
	return CreateWindowEx(0,L"CtlPanelClass",NULL,0,0,0,0,0,NULL,NULL,hInstance,NULL);
}

int WINAPI wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPWSTR lpCmdLine,int nCmdShow)
{
	WCHAR String[520];
	SHELLEXECUTEINFO pExecInfo;
	HWND hwnd;

	memset(&pExecInfo,0,sizeof(pExecInfo));
	ImmDisableIME(0);
	hwnd=CreateDummyControlPanel(hInstance);
	if(lpCmdLine[0] == L'\0' || lstrcmpiW(lpCmdLine,L"PANEL") == 0)
	{
		pExecInfo.lpFile=L"explorer.exe";
		pExecInfo.lpParameters=L"\"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\"";
	}
	else
	{
		if(lstrcmpiW(L"/policy",lpCmdLine))
		{
			ProcessPolicy();
			return 1;
		}
		if(pExecInfo.lpFile == NULL)
		{
			for(int i=0;i<21;i++)
			{
				if((lstrcmpi(CplsTable[i].CplServiceName,lpCmdLine) == 0) && (CplsTable[i].dwOS == -1 || IsOS(CplsTable[i].dwOS)))
				{
					pExecInfo.lpFile=CplsTable[i].CplFileName;
					pExecInfo.lpParameters=CplsTable[i].parameter;
					break;
				}
			}
		}
		if(pExecInfo.lpFile == NULL)
		{
			lstrcpy(String,L"Shell32.dll,Control_RunDLL ");
			int len=lstrlen(String);
			pExecInfo.lpFile=L"rundll32.exe";
			pExecInfo.lpParameters=String;
			if(GetRegisteredCplPath(lpCmdLine,String+len,520-len))
				lstrcpyn(String+len,lpCmdLine,520-len);
		}
	}

	if(nCmdShow == SW_HIDE) nCmdShow=SW_SHOWNORMAL;
	pExecInfo.nShow=nCmdShow;
	pExecInfo.cbSize=sizeof(SHELLEXECUTEINFO);
	pExecInfo.fMask=SEE_MASK_WAITFORINPUTIDLE | SEE_MASK_NOASYNC;
	ShellExecuteEx(&pExecInfo);
	if(IsWindow(hwnd))
	{
		MSG Msg;
		while(GetMessage(&Msg,NULL,0,0))
		{
			DispatchMessage(&Msg);
		}
	}

	return 1;
}

#endif
