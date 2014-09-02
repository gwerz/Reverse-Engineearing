//注意：代码中注释的部分是逆向scrsavw.lib的结果，编程时已经提供好的，不需要我们自己写，包括winmain函数
//这样的话，实际代码量只有250行
#ifndef UNICODE
#define UNICODE


#include <windows.h>
#include <commctrl.h>
#include <scrnsave.h>

#pragma comment(lib,"scrnsavw.lib")//!=scrsave.lib

HWND hWndProgressBar=NULL,hWndOKButton=NULL,hWndText=NULL;
int dwRand,uRepeatTime=0;
short wXScreen=0,wX2Screen,wYScreen,wY2Screen,wWarpSpeed=0,wDensity=0,wMoveStep=0,wMoveIncrement=0;
int nX[200],nY[200],nZ[200];
int uElapse=50,nLower=10,nUpper=200;
UINT_PTR uIDEvent=0;
int aStarsDlgHelpIds[]={-1,-1,104,4216,105,4216,100,4216,106,4217,101,4217,102,4217,0,0};

//scrnsavw.lib内部变量：
//HMODULE hInstPwdDLL=NULL,hInstImm=NULL;
//BOOL fOnWin95=FALSE,fClosing=FALSE,fCheckingPassword=FALSE;
//BYTE bACLineStatus=0xFF;
//UINT AutoPlayMsg=0;
// typedef HIMC (WINAPI* IMMASSOCIATECONTEXT)(HWND hWnd,HIMC hIMC);
// typedef BOOL (WINAPI* VERYFYPWDPROC)(HWND hWnd);
// IMMASSOCIATECONTEXT ImmFnc;
// VERYFYPWDPROC VerifyPassword;
// HIMC hPrevImc=NULL;
// POINT ptMouse;
// DWORD dwPasswordDelay=0,dwBlankTime=0,lastcheck=0xFFFFFFFF;
// BOOL preview_like_fullscreen=FALSE;
// DWORD dwWakeThreshold=4;

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
	InitCommonControls();
	return TRUE;
}

unsigned short myrand()
{//最有趣的是这个程序自己又做了个rand
	return HIWORD(dwRand=214013*dwRand+2531011);//不同之处在于：微软msvcrt里的rand()函数用LOWORD
}

void WINAPI CreateStar(unsigned short curstar)
{
	if(wXScreen)
	{
		nX[curstar]=myrand()%wXScreen-wX2Screen;
		nY[curstar]=myrand()%wYScreen-wY2Screen;
	}
	else
	{
		nX[curstar]=0;
		nY[curstar]=0;
	}
	nZ[curstar]=2560;
}

void GetIniEntries()
{
	LoadString(hMainInstance,100,szName,40);
	LoadString(hMainInstance,1007,szAppName,40);
	LoadString(hMainInstance,1001,szIniFile,13);
	LoadString(hMainInstance,1002,szScreenSaver,22);
	LoadString(hMainInstance,1009,szHelpFile,13);
	LoadString(hMainInstance,1008,szNoHelpMemory,255);
	wWarpSpeed=(unsigned short)GetPrivateProfileInt(szAppName,L"WarpSpeed",5,szIniFile);
	if(wWarpSpeed > 10) wWarpSpeed=5;
	wDensity=(unsigned short)GetPrivateProfileInt(szAppName,L"Density",25,szIniFile);
	if(wDensity > 200) wDensity=200;
	else if(wDensity < 10) wDensity=10;
}

LRESULT WINAPI ScreenSaverProcW(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
		case WM_CREATE:
			{
				GetIniEntries();
				dwRand=GetTickCount();
				wXScreen=LOWORD(((CREATESTRUCT*)lParam)->cx);
				wYScreen=LOWORD(((CREATESTRUCT*)lParam)->cy);
				wX2Screen=wXScreen/2;
				wY2Screen=wYScreen/2;
				int i=0;
				if(wDensity)
				{
					while(i<wDensity)
					{
						CreateStar(i++);
					}
					wMoveStep=10*(wWarpSpeed+1);
					uIDEvent=SetTimer(hWnd,1,uElapse,NULL);
				}
			}
			break;

		case WM_DESTROY:
			if(uIDEvent) KillTimer(hWnd,uIDEvent);
			break;

		case WM_SIZE:
			wYScreen=HIWORD(lParam);
			wXScreen=LOWORD(lParam);
			break;

		case WM_TIMER:
			{
				int curstar=0;
				HDC hDC=GetDC(hWnd);
				if(wDensity)
				{
					int scale,x,y;
					while(curstar<wDensity)
					{
						scale=nZ[curstar]?nZ[curstar]:1;
						x=wX2Screen+2560*nX[curstar]/scale;
						y=wY2Screen+2560*nY[curstar]/scale;
						PatBlt(hDC,x,y,(2560-nZ[curstar])/640+1,(2560-nZ[curstar])/640+1,BLACKNESS);
						if(wMoveIncrement<wMoveStep) wMoveIncrement++;
						else if(wMoveIncrement>wMoveStep) wMoveIncrement--;
						
						nZ[curstar]=(nZ[curstar]<=wMoveIncrement)?0:nZ[curstar]-wMoveIncrement;
						if(nZ[curstar] == 0) CreateStar(curstar);

						scale=nZ[curstar]?nZ[curstar]:1;
						x=wX2Screen+2560*nX[curstar]/scale;
						y=wY2Screen+2560*nY[curstar]/scale;
						if(x<0 || y<0 || x>wXScreen || y>wYScreen)//如果在屏幕外
						{
							CreateStar(curstar);
							scale=nZ[curstar]?nZ[curstar]:1;
							x=wX2Screen+2560*nX[curstar]/scale;
							y=wY2Screen+2560*nY[curstar]/scale;
						}
						PatBlt(hDC,x,y,(2560-nZ[curstar])/640+1,(2560-nZ[curstar])/640+1,WHITENESS);
						curstar++;
					}
				}
				ReleaseDC(hWnd,hDC);
				MSG Msg;
				if(PeekMessage(&Msg,hWnd,WM_TIMER,WM_TIMER,PM_REMOVE))
				{//控制WM_TIMER事件发送频率，发送过快则降低频率
					uElapse+=10;
					SetTimer(hWnd,1,uElapse,NULL);
					uRepeatTime=0;
				}
				else
				{
					uRepeatTime++;
					if(uRepeatTime >= 100)
					{//控制WM_TIMER事件发送频率，发送过慢则提高频率
						uRepeatTime=0;
						uElapse-=100;
						if(uElapse<50) uElapse=50;
						SetTimer(hWnd,1,uElapse,NULL);
					}
				}
			}
			break;

		default:
			break;
	}
	return DefScreenSaverProc(hWnd,msg,wParam,lParam);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message)
	{
		case WM_HELP:
			WinHelp((HWND)((LPHELPINFO)lParam)->hItemHandle,szHelpFile,HELP_WM_HELP,(ULONG_PTR)aStarsDlgHelpIds);
			break;

		case WM_CONTEXTMENU:
			WinHelp((HWND)wParam,szHelpFile,HELP_CONTEXTMENU,(ULONG_PTR)aStarsDlgHelpIds);
			break;

		case WM_INITDIALOG:
			{
				GetIniEntries();
				hWndProgressBar=GetDlgItem(hDlg,100);
				hWndOKButton=GetDlgItem(hDlg,1);
				hWndText=GetDlgItem(hDlg,101);
				SendMessage(hWndText,EM_LIMITTEXT,3,0);
				SendDlgItemMessage(hDlg,102,UDM_SETBUDDY,(WPARAM)hWndText,0);//Sets the buddy window for an up-down control. 
				SendDlgItemMessage(hDlg,102,UDM_SETRANGE,0,(LPARAM)MAKELONG(nUpper,nLower));
				SetScrollRange(hWndProgressBar,SB_CTL,0,10,FALSE);
				//SB_CTL:Sets the range of a scroll bar control. The hWnd parameter must be the handle to the scroll bar control.
				//nUpper,nLower:Maximum position and minimum position for the up-down control. Neither position can be greater than the UD_MAXVAL
				//value or less than the UD_MINVAL value. In addition, the difference between the two positions cannot exceed UD_MAXVAL. 
				SetScrollPos(hWndProgressBar,SB_CTL,wWarpSpeed,TRUE);
				SetDlgItemInt(hDlg,101,wDensity,FALSE);
			}
			break;

		case WM_COMMAND:
			{
				WCHAR String[20];
				BOOL Translated;
				switch(LOWORD(wParam))//control ID
				{
					case 1:
						wsprintf(String,L"%d",GetDlgItemInt(hDlg,101,&Translated,FALSE));
						WritePrivateProfileString(szAppName,L"Density",String,szIniFile);
						wsprintf(String,L"%d",wWarpSpeed);
						WritePrivateProfileString(szAppName,L"WarpSpeed",String,szIniFile);
						EndDialog(hDlg,TRUE);
						return 1;

					case 2:
						EndDialog(hDlg,FALSE);
						break;

					case 101:
						if(HIWORD(wParam) == EN_UPDATE)
						{
							int num=GetDlgItemInt(hDlg,101,&Translated,FALSE);
							if(num > 200 || num < 10) Translated=FALSE;
							EnableWindow(GetDlgItem(hDlg,102),Translated);
							EnableWindow(GetDlgItem(hDlg,1),Translated);
						}
						break;

					default:
						break;
				}
			}
			break;

		case WM_HSCROLL:
			{
				switch(wParam)
				{
					case SB_LINEUP:
					case SB_PAGEUP:
						wWarpSpeed--;
						if(wWarpSpeed <= 0) wWarpSpeed=0;
						if(wWarpSpeed >= 10) wWarpSpeed=10;
						SetScrollPos((HWND)lParam,SB_CTL,wWarpSpeed,TRUE);
						break;
						
					case SB_LINEDOWN:
					case SB_PAGEDOWN:
						wWarpSpeed++;
						if(wWarpSpeed <= 0) wWarpSpeed=0;
						if(wWarpSpeed >= 10) wWarpSpeed=10;
						SetScrollPos((HWND)lParam,SB_CTL,wWarpSpeed,TRUE);
						break;

					case SB_THUMBPOSITION:
						wWarpSpeed=HIWORD(wParam);
						if(wWarpSpeed <= 0) wWarpSpeed=0;
						if(wWarpSpeed >= 10) wWarpSpeed=10;
						SetScrollPos((HWND)lParam,SB_CTL,wWarpSpeed,TRUE);
						break;

					case SB_TOP:
						wWarpSpeed=0;
						SetScrollPos((HWND)lParam,SB_CTL,wWarpSpeed,TRUE);
						break;

					case SB_BOTTOM:
						wWarpSpeed=10;
						SetScrollPos((HWND)lParam,SB_CTL,wWarpSpeed,TRUE);
						break;

					case SB_THUMBTRACK:
					case SB_ENDSCROLL:
						return 1;

					default:
						if(wWarpSpeed <= 0) wWarpSpeed=0;
						if(wWarpSpeed >= 10) wWarpSpeed=10;
						SetScrollPos((HWND)lParam,SB_CTL,wWarpSpeed,TRUE);
						break;
				}
			}
			break;

		default:
			break;
	}
	return 0;
}

// int atoui(LPWSTR string)
// {
// 	int result=0;
// 	while(*string >= '0' && *string <= '9')
// 	{
// 		result=10*result+*string-'0';
// 		string++;
// 	}
// 	return result;
// }

// BOOL WINAPI HogMachine(UINT pvParam)
// {
// 	BOOL result;
// 	if(fOnWin95) 
// 		result=SystemParametersInfo(SPI_SETSCREENSAVERRUNNING,pvParam,(PVOID)&pvParam,0);
// 	return result;
// }

// int WINAPI RealScreenSaverProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
// {
// 	switch(msg)
// 	{
// 		case WM_CREATE:
// 			hInstImm=GetModuleHandle(L"IMM32.DLL");
// 			ImmFnc=(IMMASSOCIATECONTEXT)GetProcAddress(hInstImm,"ImmAssociateContext");
// 			if(ImmFnc) hPrevImc=ImmFnc(hWnd,NULL);
// 			GetCursorPos(&ptMouse);
// 			if(fChildPreview == FALSE) SetCursor(NULL);
// 			break;
// 
// 		case WM_DESTROY:
// 			if(hInstImm && ImmFnc && hPrevImc)
// 				ImmFnc(hWnd,hPrevImc);
// 			PostQuitMessage(0)
// 			break;
// 
// 		case WM_SYSKEYDOWN:
// 			if(fClosing)
// 				return DefWindowProc(hWnd,msg,wParam,lParam);
// 			break;
// 
// 		case WM_SETTEXT:
// 			return 0;
// 
// 		case WM_PAINT:
// 			if(fClosing) return DefWindowProc(hWnd,msg,wParam,lParam);
// 			if(fChildPreview == FALSE) SetCursor(NULL);
// 			break;
// 
// 		case WM_HELP:
// 		case WM_CONTEXTMENU:
// 			if(fChildPreview == NULL)
// 			{
// 				HWND father=GetParent(hWnd);
// 				if(father && IsWindow(father))
// 					PostMessage(father,msg,(WPARAM)father,lParam);
// 				return 1;
// 			}
// 			break;
// 
// 		case WM_SYSCOMMAND:
// 			if(fChildPreview == NULL)
// 			{
// 				if(wParam == SC_NEXTWINDOW || wParam == SC_PREVWINDOW || wParam == SC_SCREENSAVE)
// 					return 0;
// 				if(wParam == SC_MONITORPOWER) PostMessageW(hWnd,WM_CLOSE,0,0);
// 			}
// 			break;
// 
// 		case WM_TIMER:
// 			if(fClosing) return 0;
// 			Sleep(0);
// 			break;
// 
// 		case WM_MOUSEMOVE:
// 		case WM_LBUTTONDOWN:
// 		case WM_RBUTTONDOWN:
// 		case WM_MBUTTONDOWN:
// 		case WM_KEYDOWN:
// 			if(fClosing) return DefWindowProc(hWnd,msg,wParam,lParam);
// 			break;
// 
// 		default:
// 			break;
// 	}
// 	return ScreenSaverProc(hWnd,msg,wParam,lParam);
// }

// int WINAPI DoConfigBox(HWND hWndParent)
// {
// 	int result=RegisterDialogClasses(hMainInstance);
// 	if(result)
// 		result=DialogBoxParam(hMainInstance,MAKEINTRESOURCE(DLG_SCRNSAVECONFIGURE),hWndParent,(DLGPROC)ScreenSaverConfigureDialog,0);
// 	return result;
// }

// void WINAPI ScreenSaverChangePassword(HWND hParent)
// {
// 	typedef void (WINAPI* PWDCHANGEPASSWORD)(LPWSTR type,HWND hwnd,WPARAM wParam,LPARAM lParam);
// 	HMODULE hmod=LoadLibrary(L"MPR.DLL");
// 	if(hmod)
// 	{
// 		PWDCHANGEPASSWORD PwdChangePassword=(PWDCHANGEPASSWORD)GetProcAddress(hmod,"PwdChangePasswordW");//我的xp里还没这个函数。。。不然我就知道他密码怎么放得了
// 		if(PwdChangePassword) PwdChangePassword(L"SCRSAVE",hParent,0,0);
// 		FreeLibrary(hmod);
// 	}
// }

//int WINAPI DoPasswordCheck(HWND hWnd)
// {
// 	if(fCheckingPassword || fClosing) return 0;
// 	DWORD count;
// 	if(VerifyPassword == NULL || dwPasswordDelay)
// 	{
// 		count=GetTickCount();
// 		if(count-dwBlankTime < dwPasswordDelay)
// 			return fClosing=1;
// 	}
// 	if(lastcheck == 0xFFFFFFFF || count-lastcheck >= 200)
// 	{
// 		MSG Msg;
// 		fCheckingPassword=TRUE;
// 		PeekMessage(&Msg,hWnd,WM_TIMER,WM_TIMER,PM_REMOVE | PM_NOYIELD);
// 		PeekMessage(&Msg,hWnd,WM_TIMER,WM_TIMER,PM_REMOVE | PM_NOYIELD);
// 		fClosing=SendMessage(hWnd,SCRM_VERIFYPW,0,0);
// 		fCheckingPassword=FALSE;
// 		if(fClosing == FALSE) SetCursor(NULL);
// 		lastcheck=GetTickCount();
// 	}
// 	return fClosing;
// }

// void UnloadPwdDLL()
// {
// 	if(fOnWin95 && hInstPwdDLL)
// 	{
// 		FreeLibrary(hInstPwdDLL);
// 		hInstPwdDLL=NULL;
// 		if(VerifyPassword)
// 		{
// 			VerifyPassword=FALSE;
// 			HogMachine(0);
// 		}
// 	}
// }

// LRESULT WINAPI DefScreenSaverProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
// {
// 
// 	if(fChildPreview == FALSE && fClosing ==FALSE)
// 	{
// 		switch (msg)
// 		{
// 			case WM_CLOSE:
// 				if(fOnWin95 && DoPasswordCheck(hWnd) == FALSE)
// 				{
// 					GetCursorPos(&ptMouse);
// 					return 0;
// 				}
// 				break;
// 
// 			case SCRM_VERIFYPW:
// 				if(fOnWin95)
// 				{
// 					if(VerifyPassword) return VerifyPassword(hWnd);
// 					else return 1;
// 				}
// 				break;
// 
// 			case WM_MOUSEMOVE:
// 				if(fCheckingPassword == FALSE)
// 				{
// 					POINT Point;
// 					int deltax,deltay;
// 					GetCursorPos(&Point);
// 					if(Point.x < ptMouse.x)
// 						deltax=ptMouse.x-Point.x;
// 					else
// 						deltax=Point.x-ptMouse.x;
// 
// 					if(Point.y < ptMouse.y)
// 						deltay=ptMouse.y-Point.y;
// 					else
// 						deltay=Point.y-ptMouse.y;
// 
// 					if(deltax+deltay > dwWakeThreshold)
// 					{
// 						PostMessage(hWnd,WM_CLOSE,0,0);
// 						ptMouse.x=Point.x;
// 						ptMouse.y=Point.y;
// 					}
// 				}
// 				break;
// 
// 			case WM_LBUTTONDOWN:
// 			case WM_RBUTTONDOWN:
// 			case WM_MBUTTONDOWN:
// 			case WM_SYSKEYDOWN:
// 			case WM_KEYDOWN:
// 				PostMessage(hWnd,WM_CLOSE,0,0);
// 				break;
// 				
// 			case WM_POWERBROADCAST:
// 				{
// 					switch(wParam)
// 					{
// 						case PBT_APMRESUMESUSPEND:
// 						case PBT_APMRESUMESTANDBY:
// 						case PBT_APMRESUMEAUTOMATIC:
// 							if((lParam&1) == 0) 
// 								PostMessage(hWnd,WM_CLOSE,0,0);
// 							break;
// 
// 						case PBT_APMPOWERSTATUSCHANGE:
// 							{
// 								SYSTEM_POWER_STATUS SystemPowerStatus;
// 								BYTE laststate=bACLineStatus;
// 								if(GetSystemPowerStatus(&SystemPowerStatus))
// 									bACLineStatus=SystemPowerStatus.ACLineStatus;
// 								else
// 									bACLineStatus=0xFF;
// 								if(laststate != bACLineStatus)
// 									PostMessage(hWnd,WM_CLOSE,0,0);
// 							}
// 							break;
// 
// 						default:
// 							PostMessage(hWnd,WM_CLOSE,0,0);
// 							break;
// 					}
// 				}
// 				break;
// 
// 			case WM_SHOWWINDOW:
// 				if(wParam) SetCursor(NULL);
// 				break;
// 
// 			case WM_ACTIVATEAPP:
// 				if(wParam == FALSE) PostMessage(hWnd,WM_CLOSE,0,0);
// 				break;
// 
// 			case WM_SETCURSOR:
// 				SetCursor(NULL);
// 				return 1;
// 
// 			case WM_POWER:
// 				if(wParam == PWR_CRITICALRESUME) PostMessage(hWnd,WM_CLOSE,0,0);
// 				break;
// 
// 			default:
// 				break;
// 		}
// 	}
// 
// 	if(msg == AutoPlayMsg && msg)
// 	{
// 		PostMessage(hWnd,WM_CLOSE,0,0);
// 		return VerifyPassword;
// 	}
// 	else
// 		return DefWindowProc(hWnd,msg,wParam,lParam);
// }


// void LoadPwdDLL()
// {
// 	if(fOnWin95)
// 	{
// 		HKEY hKey;
// 		if(hInstPwdDLL) UnloadPwdDLL();
// 		if(RegOpenKey(HKEY_CURRENT_USER,L"Control Panel\\Desktop",&hKey) == ERROR_SUCCESS)
// 		{
// 			DWORD cbData=sizeof(DWORD),*Data;
// 			if(RegQueryValueEx(hKey,L"ScreenSaveUsePassword",NULL,NULL,(LPBYTE)&Data,&cbData) == ERROR_SUCCESS && Data)
// 			{
// 				hInstPwdDLL=LoadLibrary(L"PASSWORD.CPL");
// 				if(hInstPwdDLL)
// 				{
// 					VerifyPassword=(VERYFYPWDPROC)GetProcAddress(hInstPwdDLL,"VerifyScreenSavePwd");
// 					if(VerifyPassword) HogMachine(TRUE);
// 					else UnloadPwdDLL();
// 				}
// 			}
// 			RegCloseKey(hKey);
// 		}
// 	}
// }

// int WINAPI DoScreenSave(HWND hWndParent)
// {
// 	WNDCLASSW WndClass;
// 
// 	WndClass.hCursor=NULL;
// 	WndClass.hIcon=LoadIcon(hMainInstance,MAKEINTRESOURCEW(100));
// 	WndClass.lpszMenuName=NULL;
// 	WndClass.lpszClassName=L"WindowsScreenSaverClass";
// 	WndClass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
// 	WndClass.hInstance=hMainInstance;
// 	WndClass.style=CS_CLASSDC | CS_HREDRAW | CS_VREDRAW;
// 	WndClass.lpfnWndProc=(WNDPROC)RealScreenSaverProc;
// 	WndClass.cbWndExtra=0;
// 	WndClass.cbClsExtra=0;
// 
// 	RECT Rect;
// 	int left,right,top,bottom;
// 	DWORD dwstyle,dwExstyle;
// 	WCHAR* lpWindowName;
// 	if(hWndParent)
// 	{
// 		GetClientRect(hWndParent,&Rect);
// 		right=Rect.right;
// 		bottom=Rect.bottom;
// 		left=0;
// 		top=0;
// 		dwstyle=WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
// 		dwExstyle=0;
// 		fChildPreview=TRUE;
// 		lpWindowName=L"Preview";
// 	}
// 	else
// 	{
// 		left=GetSystemMetrics(SM_XVIRTUALSCREEN);
// 		top=GetSystemMetrics(SM_YVIRTUALSCREEN);
// 		right=GetSystemMetrics(SM_CXVIRTUALSCREEN);
// 		bottom=GetSystemMetrics(SM_CYVIRTUALSCREEN);
// 		if(right == 0 || bottom == 0)
// 		{
// 			RECT Rect;
// 			HDC hdc=GetDC(NULL);
// 			GetClipBox(hdc,&Rect);
// 			ReleaseDC(hdc);
// 			right=Rect.right-Rect.left;
// 			left=Rect.left;
// 			top=Rect.top;
// 			bottom=Rect.bottom-Rect.top;
// 		}//其实不用这么复杂
// 		dwstyle=WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
// 		dwExstyle=WS_EX_TOPMOST;
// 		lpWindowName=L"Screen Saver";
// 		HWND hwnd=FindWindow(L"WindowsScreenSaverClass", L"Screen Saver");
// 		if(hwnd && IsWindow(hwnd))
// 		{
// 			SetForegroundWindow(hwnd);
// 			return 0;
// 		}
// 		SYSTEM_POWER_STATUS SystemPowerStatus;
// 		if(GetSystemPowerStatus(&SystemPowerStatus))
// 			bACLineStatus=SystemPowerStatus.ACLineStatus;
// 		else
// 			bACLineStatus=0xFF;
// 		LoadPwdDLL();
// 	}
// 	if(fOnWin95)
// 		AutoPlayMsg=RegisterWindowMessage(L"QueryCancelAutoPlay");
// 	else
// 		AutoPlayMsg=0;
// 
// 	if(RegisterClass(&WndClass))
// 	{
// 		hMainWindow=CreateWindowEx(dwExstyle,L"WindowsScreenSaverClass",lpWindowName,dwstyle,
// 			left,top,right,bottom,hWndParent,NULL,hMainInstance,NULL);
// 	}
// 	MSG Msg;
// 	Msg.wParam=0;
// 	if(hMainWindow)
// 	{
// 		if(fChildPreview == FALSE)
// 			SetForegroundWindow(hMainWindow);
// 		while(GetMessage(&Msg,NULL,0,0))
// 		{
// 			TranslateMessage(&Msg);
// 			DispatchMessage(&Msg);
// 		}
// 	}
// 	UnloadPwdDLL();
// 	return Msg.wParam;
// }

// int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
// {
// 	INITCOMMONCONTROLSEX picce;
// 	int result;
// 	OSVERSIONINFO VersionInformation;
// 	CPPEH_RECORD ms_exc;
// 
// 	picce.dwSize=sizeof(INITCOMMONCONTROLSEX);
// 	picce.dwICC=ICC_TAB_CLASSES;
// 	InitCommonControlsEx(&picce);
// 	hMainInstance=hInstance;
// 	VersionInformation.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
// 	if(GetVersionEx(&VersionInformation) && VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
// 		fOnWin95=TRUE;
// 	else 
// 		fOnWin95=FALSE;
// 	ms_exc.disabled=FALSE;
// 
// 	while(TRUE)
// 	{
// 		switch(*lpCmdLine)
// 		{
// 			case L'S':
// 				return DoScreenSave(NULL);
// 
// 			case L'a':
// 			case L'A':
// 				if(fOnWin95 == FALSE) return -1;
// 			case L'l':
// 			case L'L':
// 				if(*lpCmdLine == L'l' || *lpCmdLine == 'L') 
// 					preview_like_fullscreen=TRUE;
// 			case L'p':
// 			case L'P':
// 				{
// 					do
// 					{
// 						lpCmdLine++;
// 					}
// 					while(*lpCmdLine == L' ');
// 					HWND hParent=(HWND)atoui(lpCmdLine);
// 					if(hParent == NULL || IsWindow(hParent) == FALSE)
// 						hParent=GetForegroundWindow();
// 					ScreenSaverChangePassword(hParent);
// 					return 0;
// 				}
// 
// 			case L'c':
// 			case L'C':
// 				{
// 					HWND hWndParent=NULL;
// 					if(*(lpCmdLine+1) == L':') 
// 						hWndParent=(HWND)atoui(lpCmdLine+1);
// 					if(hWndParent == NULL || IsWindow(hWndParent) == FALSE)
// 						hWndParent=GetForegroundWindow();
// 					return DoConfigBox(hWndParent);
// 				}
// 
// 			case L'\0':
// 				return DoConfigBox(NULL);
// 
// 			case L' ':
// 			case L'-':
// 			case L'/':
// 				lpCmdLine++;
// 				break;
// 
// 			case L'l':
// 			case L'L':
// 
// 			default:
// 				return -1;
// 		}
// 	}
// 
// 	ms_exc.disabled=-1;
// 	return result;
// }

#endif
