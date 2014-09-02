// unlocker.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <shellapi.h>
#include <commctrl.h>
#include <shlwapi.h>

#define MYWM_NOTIFYICON 0x8000
#define CLOSE			0x8001
#define AUTOSTART		0x8002
#define MYWM_CLEARICON	0x8003
#define HIDEICON		0x8004
#define UNKNOWNMSG		0x8005

typedef  void (*FUNC)();

NOTIFYICONDATA	Data;
DWORD			dwState;
UINT			globalmsg;
char			szTip[2560];

#define NIS_HIDDEN			0x00000001
#define NIS_SHAREDICON		0x00000002

void copysomestring(LPSTR,LPCSTR,LPCSTR,LPCSTR,LPCSTR,LPCSTR,LPCSTR,LPCSTR,LPCSTR,LPCSTR);
void showbylanguage();
BOOL IsRegInfoOk();
BOOL RegSetvalue(HKEY);
void setsomevalue(BOOL);
void setmenu(HWND hWnd);
BOOL CALLBACK DialogFunc(HWND,UINT,WPARAM,LPARAM);
int CALLBACK EnumFunc(HWND hWnd,LPARAM lparam);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	int NumArgs;
	HMODULE hMod;

	showbylanguage();
	EnumWindows(EnumFunc,0);
	if(dwState || (hMod=LoadLibrary("UnlockerHook.dll"))==0)
		return 0;
	InitCommonControls();
	globalmsg=RegisterWindowMessage("TaskbarCreated");
	HWND newwindow=CreateDialogParam(hInstance,MAKEINTRESOURCE(MainDlg),NULL,DialogFunc,0);

	FUNC Install=(FUNC)GetProcAddress(hMod,"HookInstall");
	Install();
	Data.cbSize=sizeof(NOTIFYICONDATA);
	Data.uFlags=NIF_MESSAGE | NIF_ICON  | NIF_TIP;
	Data.hIcon=LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_MAINICON));
	Data.uID=1;
	Data.uCallbackMessage=MYWM_NOTIFYICON;
	Data.hWnd=newwindow;
	lstrcpy(Data.szTip,szTip);
	LPWSTR* command=CommandLineToArgvW(GetCommandLineW(),&NumArgs);
	if(NumArgs<2 || lstrcmpW(command[1],L"-H"))
	{
		Shell_NotifyIcon(NIM_ADD,&Data);
		if(IsRegInfoOk())
			setsomevalue(0);
	}
	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//程序发送退出指令
	if(Data.hIcon)
		DestroyIcon(Data.hIcon);
	FUNC Uninstall=(FUNC)GetProcAddress(hMod,"HookUninstall");
	Uninstall();
	return msg.wParam;
}

int CALLBACK EnumFunc(HWND hWnd,LPARAM lparam)
{
	char str[256]; 
	GetWindowText(hWnd,str,sizeof(str));
	if(!lstrcmp(str,"UnlockerAssistant"))
	{
		PostMessage(hWnd,MYWM_CLEARICON,0,0);
		dwState=NIS_HIDDEN;
	}
	return 1;
}

void copysomestring(LPSTR lpString1,LPSTR lpString2,LPSTR lpString3,LPSTR lpString4,
					LPSTR lpString5,LPSTR lpString6,LPSTR lpString7,LPSTR lpString8,
					LPSTR lpString9,LPSTR lpString10)
{
	LPSTR str=lpString1;
	lstrcpy(lpString1,lpString2);
	lstrcpy(str+256,lpString3);
	lstrcpy(str+512,lpString4);
	lstrcpy(str+768,lpString5);
	lstrcpy(str+1024,lpString6);
	lstrcpy(str+1280,lpString7);
	lstrcpy(str+1536,lpString8);
	lstrcpy(str+1792,lpString9);
	lstrcpy(str+2048,lpString10);
}

void showbylanguage()
{
	HKEY hKey;
	DWORD language;
	char Data[1024];
	DWORD cbData;

	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Unlocker",
					0,KEY_QUERY_VALUE,&hKey);
	language=0x409;//0x0409 英语(美国) English (United States)
	cbData=1024;
	if(!RegQueryValueEx(hKey,"Language",NULL,NULL,(LPBYTE)Data,&cbData))
		language=StrToInt(Data);
	RegCloseKey(hKey);

//	if(language == 0x409)
	{
		copysomestring(szTip,"Unlocker Assistant","Hide Icon","Choose Language","Check for updates","Autostart","Assist only explorer.exe",
			"Close","Some software running on your machine is conflicting with Unlocker Assistant.",
			"Unlocker Assistant will now be closed and disabled.");
	}
}

void messagebox(char* dest,va_list list)
{
	char Text[1024];
	wvsprintf(Text,dest,list);
	MessageBox(NULL,Text,szTip,MB_OK);
}

BOOL IsRegInfoOk()
{
	BOOL isok;
	char Data[1024];
	DWORD cbData;
	HKEY hKey;

	isok=FALSE;
	if(!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,KEY_QUERY_VALUE,&hKey))
	{
		cbData=1024;
		if(!RegQueryValueEx(hKey,"UnlockerAssistant",NULL,NULL,(LPBYTE)Data,&cbData))
			isok=TRUE;
		RegCloseKey(hKey);
	}
	return isok;
}

BOOL RegSetvalue(HKEY value)
{
	char Data[1024];
	DWORD cbData;
	HKEY hKey=value;

	DWORD nRet=RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",0,KEY_WRITE,&hKey);
	if(!nRet)
	{
		RegDeleteValue(hKey,"UnlockerAssistant");
		cbData=1024;
		return RegCloseKey(hKey);
	}
	return nRet;
}

void setsomevalue(BOOL state)
{
	char* format;
	char filename[1024];
	char String[1024];
	HKEY hKey;

	DWORD nRet=RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",0, KEY_WRITE,&hKey);
	if(!nRet)
	{
		GetModuleFileName(NULL,filename,sizeof(filename));
		if(state) format="\"%s\" -H";
		else format="\"%s\"";
		wsprintf(String,format,filename);
		RegSetValueEx(hKey,"UnlockerAssistant",0,REG_SZ,(unsigned char*)String,lstrlen(String)+1);
		RegCloseKey(hKey);
	}
}

void setmenu(HWND hWnd)
{
	HMENU Mainmenu;
	POINT pt;
	DWORD	  STATE;

	GetCursorPos(&pt);
	Mainmenu=CreatePopupMenu();
	if(Mainmenu)
	{
		STATE=MF_UNCHECKED;
		if(IsRegInfoOk()) STATE=MF_CHECKED;
		InsertMenu(Mainmenu,0,MF_BYPOSITION | MF_ENABLED |MF_STRING,HIDEICON,szTip+256);//"Hide Icon"
		EnableMenuItem(Mainmenu,0,MF_BYPOSITION | MF_ENABLED);
		InsertMenu(Mainmenu,1,MF_BYPOSITION | MF_SEPARATOR | MF_ENABLED,0,NULL);
		InsertMenu(Mainmenu,2,STATE | MF_BYPOSITION  |MF_ENABLED |MF_STRING,AUTOSTART,szTip+1024);//"Autostart"
		EnableMenuItem(Mainmenu,2,MF_BYPOSITION | MF_ENABLED);
		InsertMenu(Mainmenu,3,MF_BYPOSITION | MF_SEPARATOR | MF_ENABLED,0,NULL);
		InsertMenu(Mainmenu,4,MF_BYPOSITION | MF_ENABLED |MF_STRING,CLOSE,szTip+1536);//"Close"
		EnableMenuItem(Mainmenu,4,MF_BYPOSITION | MF_ENABLED |MF_STRING);
		SetForegroundWindow(hWnd);
		TrackPopupMenu(Mainmenu,TPM_BOTTOMALIGN | TPM_LEFTBUTTON,pt.x,pt.y,0,hWnd,NULL);
		DestroyMenu(Mainmenu);
	}
}

BOOL CALLBACK DialogFunc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HKEY temp;
	BOOL state;

	if(uMsg == globalmsg)
	{
		Shell_NotifyIcon(NIM_ADD,&Data);
		return 0;
	}
	switch (uMsg)
	{
		case WM_DESTROY:
			Data.uFlags=0;
			Shell_NotifyIcon(NIM_DELETE,&Data);
			PostQuitMessage(0);
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case CLOSE:
					DestroyWindow(hWnd);
					break;

				case HIDEICON:
					Shell_NotifyIcon(NIM_DELETE,&Data);
					if(!IsRegInfoOk()) return 0;
					state=TRUE;
					setsomevalue(state);
					break;

				case AUTOSTART:
					if(IsRegInfoOk())
					{
						RegSetvalue(temp);
						break;
					}
					else
					{
						state=FALSE;
						setsomevalue(state);
						break;
					}

				default:
					return 1;
			}

		case MYWM_NOTIFYICON:
			if(lParam == WM_CONTEXTMENU || lParam == WM_RBUTTONDOWN )//鼠标右键事件
				setmenu(hWnd);
			break;

		case MYWM_CLEARICON:
			Shell_NotifyIcon(NIM_ADD,&Data);
			if(!IsRegInfoOk()) return 0;
			state=FALSE;
			setsomevalue(state);
			break;

		case UNKNOWNMSG:
			messagebox("%s\n\n%s",szTip+1792);
			RegSetvalue(temp);
			DestroyWindow(hWnd);
			break;

		default:
			break;
	}
	return 0;
}