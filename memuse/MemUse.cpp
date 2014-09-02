#define UNICODE
#include <windows.h>
#include "resource.h"
#define IDT_timer 1000

BOOL IsShowInKb;
BOOL IsAlwayOnTop;
HFONT hFont;
HCURSOR hCursor1,hCursor2;

void showstring(HWND hDlg,int nIDDlgItem,DWORDLONG data)
{
	NUMBERFMTW Format;//数字分隔符样式111,111,111.00
	WCHAR String[25];
	WCHAR Value[38];

	memset(&Format,0,sizeof(NUMBERFMTW));
	if(IsShowInKb)
		data /= 1024;
	Format.Grouping=3;
	Format.lpDecimalSep=L".";
	Format.lpThousandSep=L",";
	wsprintfW(Value,L"%lu",data,Format);
	GetNumberFormatW(LOCALE_USER_DEFAULT,0,Value,&Format,String,25);
	if(IsShowInKb) lstrcatW(String,L"K");
	SetDlgItemTextW(hDlg,nIDDlgItem,String);
}

void timerfunc(HWND hDlg)
{
	WCHAR string[80];
	MEMORYSTATUSEX buffer;

	buffer.dwLength=sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&buffer);
	showstring(hDlg,TotalPhysical,buffer.ullTotalPhys);
	showstring(hDlg,TotalVirtual,buffer.ullTotalPageFile);
	showstring(hDlg,UsedPhysical,buffer.ullTotalPhys-buffer.ullAvailPhys);
	showstring(hDlg,UsedVirtual,buffer.ullTotalPageFile-buffer.ullAvailPageFile);
	showstring(hDlg,AvailablePhysical,buffer.ullAvailPhys);
	showstring(hDlg,AvailableVirtual,buffer.ullAvailPageFile);
	wsprintfW(string,L"[Ph: %d%%] MemUse v0.4",buffer.dwMemoryLoad);
	SetWindowTextW(hDlg,string);
}

void paintsth(HWND hwndDlg,WPARAM wParam,LPARAM lParam)
{
	HBRUSH brush=NULL;
	if(GetDlgCtrlID(hwndDlg) <= 1803)
	{
		SetBkColor((HDC)wParam,GetSysColor(COLOR_BTNFACE));
		SetTextColor((HDC)wParam,RGB(255,0,0));//红
		SelectObject((HDC)wParam,hFont);
		GetSysColorBrush(COLOR_BTNFACE);
	}
}

BOOL CALLBACK AboutDlgFunc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_COMMAND:

		case WM_CLOSE:
			EndDialog(hwndDlg,0);
			DeleteObject(hFont);
			return 1;

		case WM_INITDIALOG:
			{
				LOGFONTW logfont;
				GetObjectW(GetStockObject(DEFAULT_GUI_FONT),sizeof(LOGFONTW),&logfont);
				logfont.lfUnderline=TRUE;
				hFont=CreateFontIndirectW(&logfont);

				hCursor1=LoadCursorW(NULL,IDC_ARROW);
				hCursor2=LoadCursorW(NULL,IDC_HAND);
				SendMessageW(GetDlgItem(hwndDlg,IDI_MainIcon),STM_SETICON,
					GetClassLongW(hwndDlg,GCL_HICON),0);
				return 1;
			}

		case WM_MOUSEMOVE:
			SetCursor(hCursor1);
			//这里并不是忘记了break
		case WM_LBUTTONUP:
			{
				POINT pt;
				pt.y=HIWORD(lParam);
				pt.x=LOWORD(lParam);
				HWND child=ChildWindowFromPointEx(hwndDlg,pt,CWP_SKIPTRANSPARENT);
				if(GetDlgCtrlID(child) <= 1803)//自己看看是哪些控件吧
				{
					if(uMsg == WM_MOUSEMOVE)//记者上面是没有break的
						SetCursor(hCursor2);
					else//WM_LBUTTONUP
					{
						WCHAR Buffer[250];
						SendMessageW(child,WM_GETTEXT,250,(LPARAM)Buffer);
						ShellExecuteW(hwndDlg,L"open",Buffer,NULL,NULL,SW_SHOWNORMAL);
					}
				}
			}

		case WM_CTLCOLORSTATIC:
			paintsth(hwndDlg,wParam,lParam);
			break;

		default:
			break;
	}

	return 0;
}

BOOL CALLBACK DlgFunc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				HMENU sysmenu=GetSystemMenu(hwndDlg,FALSE);
				AppendMenuW(sysmenu,MF_SEPARATOR,0,NULL);
				AppendMenuW(sysmenu,MF_INSERT,112,L"Always On &Top");
				AppendMenuW(sysmenu,MF_INSERT,114,L"S");
				AppendMenuW(sysmenu,MF_SEPARATOR,0,NULL);
				AppendMenuW(sysmenu,MF_INSERT,113,L"&About...");
				SetTimer(hwndDlg,IDT_timer,1000,NULL);
				timerfunc(hwndDlg);
				HICON icon=LoadIconW((HINSTANCE)GetWindowLongW(hwndDlg,GWL_HINSTANCE),MAKEINTRESOURCEW(IDI_MainIcon));
				SetClassLongW(hwndDlg,GCL_HICON,(LONG)icon);
			}
			break;

		case WM_CLOSE:
			EndDialog(hwndDlg,0);
			break;

		case WM_TIMER:
			timerfunc(hwndDlg);
			break;

		case WM_SYSCOMMAND:
			{
				HMENU sysmenu;
				DWORD checkstate;
				DWORD uID;
				RECT rect;

				switch(LOWORD(wParam))
				{
					case 114://show in kb
						{
							IsShowInKb= IsShowInKb == FALSE;//取反
							sysmenu=GetSystemMenu(hwndDlg,FALSE);
							checkstate=MF_CHECKED*(IsShowInKb != FALSE);
							uID=114;
						}
						break;

					case 113:
						{
							DialogBoxParamW((HINSTANCE)GetWindowLongW(hwndDlg,GWL_HINSTANCE),
								MAKEINTRESOURCEW(AboutDlg),hwndDlg,AboutDlgFunc,0);
							return 0;
						}

					case 112:
						{
							GetWindowRect(hwndDlg,&rect);
							sysmenu=GetSystemMenu(hwndDlg,FALSE);
							IsAlwayOnTop= IsAlwayOnTop == FALSE;//取反
							SetWindowPos(hwndDlg,IsAlwayOnTop != FALSE? HWND_TOPMOST:HWND_NOTOPMOST,
								rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,0);
							checkstate=MF_CHECKED*(IsAlwayOnTop != FALSE);
							uID=112;
						}
						break;

					default:
						return 0;
				}

				CheckMenuItem(sysmenu,uID,checkstate);
				return 0;
			}

		default:
			return 0;

	}
	return 1;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	DialogBoxParamW(hInstance,MAKEINTRESOURCEW(MainDlg),NULL,DlgFunc,0);
	return  0;
}
