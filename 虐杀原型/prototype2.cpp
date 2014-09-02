#include <windows.h>
#include "resource.h"
#include <stdio.h>
#include <tchar.h>
#include <winuser.h>

struct PrototypeStruct
{
	LPSTR* ptrlpCmdLine;
	HWND hInstance;
	HWND hPrevInstance;
	int nShowCmd;
};

typedef bool (*ENGINEINITIALIZE)(PrototypeStruct* data);
typedef void (*ENGINETERMINATE)();
typedef bool (*ENGINESERVICE)(bool IsStop,bool FocusWindow);
typedef void (*ENGINEKEYBOARDSERVICE)(UINT uMsg,WPARAM wParam,LPARAM lParam,bool IsCapitalOn,bool IsNumlockOn);
typedef void (*ENGINEMOUSESERIVCE)(UINT uMsg,WPARAM wParam,LPARAM lParam);
typedef void (*ENGINEWINDOWFOCUS)(bool FocusWindow);

ENGINEWINDOWFOCUS EngineWindowFocus=NULL;
bool Enable=TRUE;
STICKYKEYS stickykeys={sizeof(STICKYKEYS),0};
TOGGLEKEYS togglekeys={sizeof(TOGGLEKEYS),0};
FILTERKEYS filterkeys={sizeof(FILTERKEYS),0,0,0,0,0,};
TCHAR WindowName[]={0x5B,0x50,0x52,0x4F,0x54,0x4F,0x54,0x59,0x50,0x45,0xAE,0x32,0x5D,0x00};

LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_SYSCOMMAND:
			if(wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER) 
				return 1;
			break;

		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			if(wParam == VK_MENU /*alt*/|| wParam == VK_F10)
				return 1;

		case WM_ACTIVATEAPP:
			if(wParam == 1) Enable=TRUE;
			else if(wParam == 0) Enable=FALSE;
			EngineWindowFocus(Enable);
			return 1;	

		case WM_CLOSE:
			PostQuitMessage(0);
			EngineWindowFocus(Enable);
			return 1;

		default:
			break;
	}
	return DefWindowProc(hwnd,uMsg,wParam,lParam);
}

void SomeSysConfig(bool clear)
{
	STICKYKEYS stickykey;
	TOGGLEKEYS togglekey;
	FILTERKEYS filterkey;

	if(clear)
	{
		SystemParametersInfo(SPI_SETSTICKYKEYS,sizeof(STICKYKEYS),&stickykeys,0);
		SystemParametersInfo(SPI_SETTOGGLEKEYS,sizeof(TOGGLEKEYS),&togglekeys,0);
		SystemParametersInfo(SPI_SETFILTERKEYS,sizeof(FILTERKEYS),&filterkeys,0);
	}
	else
	{
		memcpy(&stickykey,&stickykeys,sizeof(STICKYKEYS));
		if((stickykeys.dwFlags & SKF_STICKYKEYSON) == 0)//if StickyKeys feature is off
		{
			stickykey.dwFlags=stickykeys.dwFlags & ~(SKF_CONFIRMHOTKEY | SKF_HOTKEYACTIVE);
			SystemParametersInfo(SPI_SETSTICKYKEYS,sizeof(STICKYKEYS),&stickykey,0);
		}

		memcpy(&togglekey,&togglekeys,sizeof(TOGGLEKEYS));
		if((togglekeys.dwFlags & TKF_TOGGLEKEYSON) == 0)//if ToggleKeys feature is off.
		{
			togglekey.dwFlags=togglekeys.dwFlags & ~(DWORD)(TKF_CONFIRMHOTKEY | TKF_HOTKEYACTIVE);
			SystemParametersInfo(SPI_SETTOGGLEKEYS,sizeof(TOGGLEKEYS),&togglekey,0);
		}

		memcpy(&filterkey,&filterkeys,sizeof(FILTERKEYS));
		if((filterkeys.dwFlags & FKF_FILTERKEYSON) == 0)//if FilterKeys feature is off.
		{
			filterkey.dwFlags=filterkeys.dwFlags & ~(DWORD)(FKF_CONFIRMHOTKEY | FKF_HOTKEYACTIVE);
			SystemParametersInfo(SPI_SETFILTERKEYS,sizeof(FILTERKEYS),&filterkey,0);
		}
	}
}

bool SetConfig(LPCSTR SetProperty,LPCSTR SetString,int* SetValue)//该函数设置显示属性
{
	LPSTR findptr=strstr(SetString,SetProperty);
	if(findptr == NULL) return FALSE;
	return sscanf(findptr+_tcslen(SetProperty),"=%d",SetValue) == 1;//从命令行读取设置参数
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	LPSTR FilePart;
	TCHAR LibFileName[256];
	TCHAR ClassName[256];
	TCHAR PathName[256];
	TCHAR Text[256];
	TCHAR FileName[256];
	TCHAR PrototypeStr[2048];

	strncpy(PrototypeStr,lpCmdLine,sizeof(PrototypeStr));
	strlwr(PrototypeStr);
	GetModuleFileName(hInstance,FileName,sizeof(FileName));//prototype2.exe
	GetFullPathName(FileName,sizeof(PathName),PathName,&FilePart);
	FilePart[-1]='\0';//断开目录和文件名
	strcpy(LibFileName,FilePart);
	LPSTR dotpos=strchr(LibFileName,'.');
	*dotpos='\0';//prototype2 exe
	strcat(dotpos,"engine");//prototype2engine
	if(strstr(PrototypeStr,"tool"))
		strcat(dotpos,"Tool");
	strcat(dotpos,".dll");//prototype2engine.dll
	strcpy(ClassName,FilePart);
	dotpos=strchr(ClassName,'.');
	strcpy(dotpos-1,"WindowClass");//prototypeWindowClass

	HWND wind=FindWindowEx(NULL,NULL,ClassName,WindowName);//???
	if(wind)
	{
		ShowWindow(wind,SW_SHOW);
		SetForegroundWindow(wind);
		return 0;
	}
	SetCurrentDirectory(PathName);
	LPSTR strpos=strstr(PrototypeStr,"batchmode");
	BOOL flag1= strpos!=NULL,flag2= strpos!=NULL;
	if(strpos == NULL && ((strstr(PrototypeStr,"exporthelpfile")) || strstr(PrototypeStr,"exportschema")))
	{
		fprintf(stderr,"%s(%d) : warning : ExportHelpFile or ExportSchema command-line options also need the BatchMode option\n  "
				"for unattended error output handling.","D:\\work\\worker1\\atg\\engine\\code\\main\\mainwin32.cpp",209);
		flag1=flag2=TRUE;
	}

	HMODULE hmod=LoadLibrary(LibFileName);
	if(hmod == NULL)
	{
		if(flag1)
		{
			fprintf(stderr,"%s(%d) : error : could not load DLL '%s'.\n[\n    You probably have a static initializer that is crashing."
					"\n    The most common cause is forgetting core::NameStringAllocationStatic in a static core::Name:\n          "
					"static const engine::Name Foo( \"Bar\", core::NameStringAllocationStatic );\n\n    Or verify that the DLL exists."
					" You can debug the issue by running your 'main' project with the command line: >>%s<<\n]\n",
					"D:\\work\\worker1\\atg\\engine\\code\\main\\mainwin32.cpp",230,LibFileName,PrototypeStr);
		}
		else
		{
			wsprintf(Text, "ERROR: Could not load DLL '%s'.\n    You probably have a static initializer that is crashing.\n    The most"
					" common cause is forgetting core::NameStringAllocationStatic in a static core::Name:\n          "
					"static const engine::Name Foo( \"Bar\", core::NameStringAllocationStatic );\n\n    Or verify that the DLL exists. "
					"You can debug the issue by running your 'main' project with the command line: >>%s<<",LibFileName,PrototypeStr);
			MessageBox(NULL,Text,"MainWin32",MB_OK | MB_ICONHAND);
		}
		return -1;
	}

	ENGINEINITIALIZE EngineInitialize=(ENGINEINITIALIZE)GetProcAddress(hmod,"EngineInitialize");
	ENGINETERMINATE EngineTerminate=(ENGINETERMINATE)GetProcAddress(hmod,"EngineTerminate");
	ENGINESERVICE EngineService=(ENGINESERVICE)GetProcAddress(hmod,"EngineService");
	ENGINEKEYBOARDSERVICE EngineKeyboardService=(ENGINEKEYBOARDSERVICE)GetProcAddress(hmod,"EngineKeyboardService");
	ENGINEMOUSESERIVCE EngineMouseService=(ENGINEMOUSESERIVCE)GetProcAddress(hmod,"EngineMouseService");
	EngineWindowFocus=(ENGINEWINDOWFOCUS)GetProcAddress(hmod,"EngineWindowFocus");
	if(EngineInitialize == NULL || EngineTerminate == NULL || EngineService == NULL || EngineKeyboardService == NULL ||
			EngineMouseService == NULL || EngineWindowFocus == NULL)
	{
		if(flag2)
		{
			fprintf(stderr,"%s(%d) : error : DLL '%s' does not contain required exports.\n",
				"D:\\work\\worker1\\atg\\engine\\code\\main\\mainwin32.cpp",258,LibFileName);
		}
		else
		{
			wsprintf(Text,"ERROR: DLL \"%s\" does not contain required exports.",LibFileName);
			MessageBox(NULL,Text,"MainWin32",MB_OK | MB_ICONHAND);
			
		}
		FreeLibrary(hmod);
		return -1;
	}
	
	WNDCLASS WndClass={CS_OWNDC | CS_HREDRAW | CS_VREDRAW,WndProc,0,0,hInstance,LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON)),
						LoadCursor(NULL,IDC_ARROW),(HBRUSH)GetStockObject(DKGRAY_BRUSH),NULL,ClassName};
	RegisterClass(&WndClass);

	int windowx=CW_USEDEFAULT,windowy=CW_USEDEFAULT,windowwidth=1280,windowheight=720;
	SetConfig("windowx",PrototypeStr,&windowx);
	SetConfig("windowy",PrototypeStr,&windowy);
	bool flag3=SetConfig("windowwidth",PrototypeStr,&windowwidth);
	bool flag4=SetConfig("windowheight",PrototypeStr,&windowheight);
	RECT rect;
	memset(&rect,0,sizeof(RECT));
	if(flag3 == FALSE)
	{
		if(flag4 == TRUE)
		{
			rect.bottom=windowheight;
			// v22 = (signed int)(0x38E38E390ui64 * windowheight >> 32) >> 1;
			// v20 = v22 + (v22 >> 31);其实进行的是除法运算
			rect.right=windowwidth=windowheight*16/9;
		}
		else//flag4 == FALSE
		{
			rect.bottom=windowheight;
			rect.right=windowwidth;	
		}
	}
	else//flag3 == TRUE
	{
		if(flag4 == TRUE)
		{
			rect.bottom=windowheight;
			rect.right=windowwidth;			
		}
		else//flag4 == FALSE
		{
			rect.bottom=windowwidth*9/16;
			rect.right=windowwidth;
		}
	}
	
	AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE);
	HWND hWnd=CreateWindowEx(0,ClassName,WindowName,8/*???*/,windowx,windowy,rect.right-rect.left,rect.bottom-rect.top,
								NULL,NULL,hInstance,NULL);
	SystemParametersInfo(SPI_GETSTICKYKEYS,sizeof(STICKYKEYS),&stickykeys,0);
	SystemParametersInfo(SPI_GETTOGGLEKEYS,sizeof(TOGGLEKEYS),&togglekeys,0);
	SystemParametersInfo(SPI_GETFILTERKEYS,sizeof(FILTERKEYS),&filterkeys,0);
	SomeSysConfig(FALSE);

	PrototypeStruct proto={&lpCmdLine,hWnd,hWnd,nShowCmd};
	
	if(EngineInitialize(&proto))
	{
		bool HasGotMsg=FALSE;
		do 
		{
			if(HasGotMsg) break;
			bool IsCaptitalOn= GetKeyState(VK_CAPITAL)!=0;
			bool IsNumlockOn= GetKeyState(VK_NUMLOCK)!=0;
			MSG Msg;
			if(PeekMessage(&Msg,NULL,0,0,PM_REMOVE))
			{
				while(Msg.message != WM_QUIT)
				{
					switch(Msg.message)
					{
						case WM_KEYUP:
						case WM_KEYFIRST://???
							EngineKeyboardService(Msg.message,Msg.wParam,Msg.lParam,IsCaptitalOn,IsNumlockOn);
							break;

						case WM_MOUSEFIRST:
						case WM_LBUTTONDOWN:
						case WM_LBUTTONUP:
						case WM_RBUTTONDOWN:
						case WM_RBUTTONUP:
						case WM_MBUTTONDOWN:
						case WM_MBUTTONUP:
							EngineMouseService(Msg.message,Msg.wParam,Msg.lParam);
							break;
#define WM_MOUSEWHEEL 0x20A
						case WM_MOUSEWHEEL:
							{
								POINT Point={LOWORD(Msg.lParam),HIWORD(Msg.lParam)};
								LPARAM lParam=Msg.lParam;
								if(ScreenToClient(hWnd,&Point))
									lParam=MAKELONG(Point.x,Point.y);
								EngineMouseService(Msg.message,Msg.wParam,lParam);
							}
							break;
						
						default:
							break;
					}
					TranslateMessage(&Msg);
					DispatchMessage(&Msg);
					if(PeekMessage(&Msg,NULL,0,0,PM_REMOVE) == FALSE)
						goto LABEL_49;

				}
				HasGotMsg=TRUE;
			}
LABEL_49:;

		} 
		while (EngineService(HasGotMsg,!Enable));
		exit(0);
	}



	if(!flag2)
		OutputDebugString("Early exit while running the game, errors have probably happened.\n");
	SomeSysConfig(FALSE);
	DestroyWindow(hWnd);
	UnregisterClass(ClassName,hInstance);
	FreeLibrary(hmod);
	OutputDebugString("Game has successfully terminated. All is well.\n");
	return 0;
}

