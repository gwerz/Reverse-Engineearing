#include <windows.h>
#include <mbstring.h>
#include <process.h>

HANDLE LetWndProcQuitEvent=NULL,WaitUntilWndProcQuitEvent=NULL;
HWND hGlobalWnd=NULL,hMainWnd=NULL;
BOOL IsWindowProcRun=FALSE;

BOOL PreparePalette(LPCSTR bmpname,HBITMAP* ptrbitmap,HPALETTE* hPalette,int* width,int* height);
LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
void LaunchingWarcraftThread(void* dummy);
void WarcraftPopupWndProc();
HWND BeginEvent();
void EndEvent(DWORD Totalcount);
void QuitWindow();


int WINAPI WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	int nRet=0;
	int curcount=GetTickCount();
	DWORD ExitCode=0;
	char Text[260],Data[260];

	HANDLE WarcraftLauncherEvent=CreateEvent(NULL,FALSE,FALSE,"WARCRAFT_III_LAUNCHER");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(WarcraftLauncherEvent);
		return 0;
	}

	HANDLE WarcraftGameAppEvent=CreateEvent(NULL,FALSE,FALSE,"Warcraft III Game Application");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		LoadString(hInstance,12,Data,260);
		LoadString(hInstance,13,Text,260);
		MessageBox(NULL,Data,Text,MB_OK);
		CloseHandle(WarcraftGameAppEvent);
		return 0;
	}

	CloseHandle(WarcraftGameAppEvent);
	hMainWnd=BeginEvent();
	HANDLE WarcraftOKEvent=CreateEvent(NULL,FALSE,FALSE,"WARCRAFT_III_OK");
	if(GetLastError() != ERROR_ALREADY_EXISTS)
	{
		STARTUPINFO StartupInfo;
		PROCESS_INFORMATION ProcessInformation;
		char War3dotexe[260],FrozenThronedotexe[260];

		do 
		{
			while(1)
			{
				while(1)
				{
					memset(&StartupInfo,0,sizeof(STARTUPINFO));
					memset(&ProcessInformation,0,sizeof(PROCESS_INFORMATION));
					LoadString(hInstance,1,War3dotexe,260);
					LoadString(hInstance,7,FrozenThronedotexe,260);
					_mbslwr((unsigned char*)War3dotexe);//Convert a string to lowercase.
					_mbslwr((unsigned char*)FrozenThronedotexe);
					LPSTR lpCommandLine=GetCommandLine();
					LPSTR findpos=(LPSTR)_mbsstr((unsigned char*)lpCommandLine,(unsigned char*)"\"");
					
					if(findpos)
					{
						_mbscpy((unsigned char*)Data,(unsigned char*)lpCommandLine);
					}
					else
					{
						_mbscpy((unsigned char*)Data,(unsigned char*)"\"");//???
						_mbscpy((unsigned char*)Data,(unsigned char*)lpCommandLine);
						_mbscpy((unsigned char*)Data,(unsigned char*)"\"");
					}

					_mbslwr((unsigned char*)Data);
					_mbscpy((unsigned char*)Text,(unsigned char*)"");
					findpos=(LPSTR)_mbsstr((unsigned char*)Data,(unsigned char*)FrozenThronedotexe);
					_mbsnbcat((unsigned char*)Text,(unsigned char*)Data,findpos-Data);//获得本层目录名
					_mbscat((unsigned char*)Text,(unsigned char*)War3dotexe);
					_mbscat((unsigned char*)Text,(unsigned char*)"\" ");
					_mbscat((unsigned char*)Text,(unsigned char*)lpCmdLine);

					if(CreateProcess(NULL,Text,NULL,NULL,TRUE,0,NULL,NULL,&StartupInfo,&ProcessInformation))
						break;
					nRet=GetLastError();
					LoadString(hInstance,2,Data,260);
					LoadString(hInstance,9,Text,260);
					EndEvent(0);
					if(MessageBox(hMainWnd,Text,Data,MB_TOPMOST | MB_RETRYCANCEL) == IDCANCEL)
						goto End;
				}
				HANDLE handles[2]={ProcessInformation.hProcess,WarcraftOKEvent};
				nRet=WaitForMultipleObjects(2,handles,FALSE,INFINITE);
				LoadString(hInstance,2,Data,260);
				if(nRet) break;
				GetExitCodeProcess(ProcessInformation.hProcess,&ExitCode);

				int nID;
				switch(ExitCode)
				{
					case 1:
						nID=4;
						//冰封王座无法定位您的光盘驱动器。$0A请确认您的冰封王座光盘已在光盘驱动器中，然后点击‘重试’。
						break;

					case 2:
						nID=5;
						//冰封王座无法检测到您的光盘驱动器。$0A请确认您的冰封王座光盘已在光盘驱动器中，然后点击‘重试’。
						break;

					case 5:
						nID=6;
						//请确认您的冰封王座光盘已在光盘驱动器中，然后点击‘重试’。
						break;

					case 13:
						nID=10;
						//你需要这台PC上的管理员权限才能运行冰封王座。请检查您的设置，然后点击‘重试’。
						break;

					case 0:
					case 3:
					case 4:
					case 6:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
					case 12:
						nID=3;
						//请核实您的冰封王座光盘已在光盘驱动器中，然后点击‘重试’。
						break;

					default:
						nID=8;
						//冰封王座无法运行。$0A请确认您的冰封王座光盘已在光盘驱动器中，然后点击‘重试’。
						break;
				}
				LoadString(hInstance,nID,Text,260);
				EndEvent(0);
				if(MessageBox(hMainWnd,Text,Data,MB_TOPMOST | MB_RETRYCANCEL) == IDCANCEL)
					goto End;
			}
		} 
		while (nRet != WAIT_OBJECT_0+1);
		EndEvent(0);
	}
End:
	EndEvent(curcount);
	QuitWindow();
	CloseHandle(WarcraftOKEvent);
	CloseHandle(WarcraftLauncherEvent);
	return nRet;
}

BOOL PreparePalette(LPCSTR bmpname,HBITMAP* ptrbitmap,HPALETTE* hPalette,int* width,int* height)
{
	HBITMAP hBitmap;

	*ptrbitmap=NULL;
	*hPalette=NULL;
	hBitmap=(HBITMAP)LoadImage(GetModuleHandle(NULL),bmpname,IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION);
	*ptrbitmap=hBitmap;
	if(hBitmap == NULL) return FALSE;

	BITMAP pv;
	GetObject(hBitmap,sizeof(BITMAP),&pv);
	*width=pv.bmWidth;
	*height=pv.bmHeight;
	if(pv.bmBitsPixel*pv.bmPlanes > 8)
	{
		HDC hdc=GetDC(NULL);
		*hPalette=CreateHalftonePalette(hdc);
		ReleaseDC(NULL,hdc);
	}
	else
	{
		HDC hcomdc=CreateCompatibleDC(NULL);
		HBITMAP oldbitmap=(HBITMAP)SelectObject(hcomdc,*ptrbitmap);
#define TABLENUM 256
		int size=TABLENUM;
		RGBQUAD prgbq[256];
		GetDIBColorTable(hcomdc,0,TABLENUM,prgbq);
		LOGPALETTE* logpalet=(LOGPALETTE*)malloc(2*sizeof(WORD)+TABLENUM*sizeof(PALETTEENTRY));
		logpalet->palVersion=0x300;
		logpalet->palNumEntries=TABLENUM;
		for(int i=0;i<TABLENUM;i++)
		{
			logpalet->palPalEntry[i].peRed=prgbq[i].rgbRed;
			logpalet->palPalEntry[i].peGreen=prgbq[i].rgbGreen;
			logpalet->palPalEntry[i].peBlue=prgbq[i].rgbBlue;
			logpalet->palPalEntry[i].peFlags=0;
		}
		*hPalette=CreatePalette(logpalet);
		free(logpalet);
		SelectObject(hcomdc,oldbitmap);
		DeleteDC(hcomdc);
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_PAINT:
			{
				PAINTSTRUCT Paint;
				HBITMAP hbitmap,holdbitmap;
				HPALETTE hpal,holdpal;
				int width,height;
				BITMAP pv;
				char chText[2048];

				HDC paintdc=BeginPaint(hWnd,&Paint);
				if(PreparePalette(MAKEINTRESOURCE(106),&hbitmap,&hpal,&width,&height))
				{
					GetObject(hbitmap,sizeof(BITMAP),&pv);
					HDC hcomdc=CreateCompatibleDC(paintdc);
					holdbitmap=(HBITMAP)SelectObject(hcomdc,hbitmap);
					holdpal=SelectPalette(paintdc,hpal,FALSE);
					RealizePalette(paintdc);
					BitBlt(paintdc,0,0,pv.bmWidth,pv.bmHeight,hcomdc,0,0,SRCCOPY);
					SelectObject(hcomdc,holdbitmap);
					DeleteObject(hbitmap);
					SelectPalette(paintdc,holdpal,FALSE);
					DeleteObject(hpal);
				}
				LoadString(GetModuleHandle(NULL),11,chText,2048);
				RECT rt={0,0,width-16,0};
				DrawText(paintdc,chText,strlen(chText),&rt,DT_CALCRECT | DT_WORDBREAK | DT_CENTER);
				SetTextColor(paintdc,RGB(240,200,0));
				SetBkColor(paintdc,RGB(0,0,0));
				SetBkMode(paintdc,TRANSPARENT);
				int delta1,delta2;
				delta1=height-rt.bottom-8;
				delta2=(width-rt.right-16)/2+8;
				rt.bottom+=delta1;
				rt.top+=delta1;
				rt.right+=delta2;
				rt.left+=delta2;
				DrawText(paintdc,chText,strlen(chText),&rt,DT_WORDBREAK | DT_CENTER);
				EndPaint(hWnd,&Paint);
			}
			break;

		case WM_NCCALCSIZE:
			break;

		default:
			return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	return 0;
}

void LaunchingWarcraftThread(void* dummy)
{
	HBITMAP hBitmap;
	HPALETTE hPalet;
	int nWidth,nHeight;

	if(PreparePalette(MAKEINTRESOURCE(106),&hBitmap,&hPalet,&nWidth,&nHeight))
	{
		WNDCLASS WndClass;
		DeleteObject(hBitmap);
		DeleteObject(hPalet);
		memset(&WndClass,0,sizeof(WNDCLASS));
		WndClass.style=0;
		WndClass.lpfnWndProc=WndProc;
		WndClass.hInstance=GetModuleHandle(NULL);
		WndClass.lpszClassName="Warcraft III";
		RegisterClass(&WndClass);
		RECT Rect;
		GetClientRect(GetDesktopWindow(),&Rect);
		HWND hWarcraftWnd=CreateWindowEx(WS_EX_TOOLWINDOW,"Warcraft III","Launching Warcraft III",
							0,(Rect.right+Rect.left)/2-nWidth/2,(Rect.bottom+Rect.top)/2-nHeight/2,
							nWidth,nHeight,GetDesktopWindow(),NULL,GetModuleHandle(NULL),NULL);
		if(hWarcraftWnd)
		{
			ShowWindow(hWarcraftWnd,SW_SHOW);
			UpdateWindow(hWarcraftWnd);
			MSG Msg;
			while(WaitForSingleObject(LetWndProcQuitEvent,100) == WAIT_TIMEOUT)//如果没收到命令线程退出信号
			{//LetWndProcQuitEvent无信号
				while(PeekMessage(&Msg,hWarcraftWnd,0,0,PM_NOREMOVE))
				{
					GetMessage(&Msg,hWarcraftWnd,0,0);
					TranslateMessage(&Msg);
					DispatchMessage(&Msg);
				}
			}
			DestroyWindow(hWarcraftWnd);//收到了退出信号则退出线程
		}
	}
	SetEvent(WaitUntilWndProcQuitEvent);//发出线程已成功退出的信号
}

void WarcraftPopupWndProc()
{
	WNDCLASS WndClass;
	memset(&WndClass,0,sizeof(WNDCLASS));
	WndClass.style=0;
	WndClass.lpfnWndProc=DefWindowProc;
	WndClass.hInstance=GetModuleHandle(NULL);
	WndClass.lpszClassName="Warcraft III Root";
	RegisterClass(&WndClass);

	RECT Rect;
	MSG Msg;
	GetClientRect(GetDesktopWindow(),&Rect);
	hGlobalWnd=CreateWindowEx(0,"Warcraft III Root","Launching Warcraft III",WS_POPUP,
					(Rect.right+Rect.left)/2-16,(Rect.bottom+Rect.top)/2-16,16,16,
					GetDesktopWindow(),NULL,GetModuleHandle(NULL),NULL);
	ShowWindow(hGlobalWnd,SW_SHOWNORMAL);
	UpdateWindow(hGlobalWnd);
	while(PeekMessage(&Msg,hGlobalWnd,0,0,PM_NOREMOVE))
	{
		GetMessage(&Msg,hGlobalWnd,0,0);
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

HWND BeginEvent()
{
	WarcraftPopupWndProc();
	IsWindowProcRun=TRUE;
	LetWndProcQuitEvent=CreateEvent(NULL,TRUE,FALSE,NULL);//设置为无信号，使线程能执行消息响应
	WaitUntilWndProcQuitEvent=CreateEvent(NULL,TRUE,FALSE,NULL);//设置为无信号，表明消息响应线程未结束
	_beginthread(LaunchingWarcraftThread,0x2000,NULL);
	SetFocus(hGlobalWnd);
	return hGlobalWnd;
}

void EndEvent(DWORD Totalcount)
{
	if(IsWindowProcRun)
	{
		if(Totalcount)
		{
			DWORD curcount=GetTickCount();
			if(curcount < Totalcount)
				Sleep(Totalcount-curcount);
		}
		SetEvent(LetWndProcQuitEvent);//命令消息响应函数退出消息循环
		WaitForSingleObject(WaitUntilWndProcQuitEvent,INFINITE);//等待消息响应函数发出确认退出信号
		IsWindowProcRun=FALSE;
	}
}

void QuitWindow()
{
	if(hGlobalWnd) 
		DestroyWindow(hGlobalWnd);
}

