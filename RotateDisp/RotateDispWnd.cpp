// RotateDispWnd.cpp : implementation file
//

#include "stdafx.h"
#include "RotateDisp.h"
#include "RotateDispWnd.h"
#include "RotatedispDlg.h"

#ifdef WINVER
#undef WINVER
#define WINVER  0x0500
#include <imm.h>
#pragma comment(lib,"Imm32.lib")
#endif
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"RotateMouse\\Debug\\RotateMouse.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRotateDispWnd dialog
extern void WINAPI SetRotate(BOOL Begin);
extern void WINAPI SetRotateAngle(int angle);
extern void WINAPI UnSetHook();
extern void WINAPI SetHook();

UINT msg_tray=0;//异常还不知道代码怎么写
UINT msg_bar=0;

#define ANG_STAY	-1
#define ANG_0		0
#define ANG_90		1
#define ANG_180		2
#define ANG_270		3


CRotateDispWnd::CRotateDispWnd(CWnd* pParent /*=NULL*/)
{
	//{{AFX_DATA_INIT(CRotateDispWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	msg_tray=RegisterWindowMessage("WM_APP_WM_USER_TRAY_RotateDisp");
	msg_bar=RegisterWindowMessage("TaskbarCreated");

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	hDlgWnd=NULL;
	ImmDisableIME(-1);
	
	IsAboutDlgShow=false;
	IsMouseDllOk=false;
	if(PathFileExists("RotateMouse.dll"))
		IsMouseDllOk=true;
}


void CRotateDispWnd::DoDataExchange(CDataExchange* pDX)
{
	CWnd::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRotateDispWnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRotateDispWnd, CWnd)
	//{{AFX_MSG_MAP(CRotateDispWnd)
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_VERSIONINFO, OnVersioninfo)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRotateDispWnd message handlers

void CRotateDispWnd::OnExit() 
{
	// TODO: Add your command handler code here
	PostMessage(WM_CLOSE,0,0);
}

void CRotateDispWnd::OnVersioninfo() 
{
	// TODO: Add your command handler code here
	if(!IsAboutDlgShow)
	{
		IsAboutDlgShow=TRUE;
		SetForegroundWindow();
		CRotateDispDlg Dlg;
		Dlg.DoModal();
		IsAboutDlgShow=FALSE;
	}
}

LRESULT CRotateDispWnd::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	CMenu mpop;

	if(message == msg_tray)
	{
		if(lParam == WM_RBUTTONUP)
		{
			POINT Point;
			MENUITEMINFO mii;
			GetCursorPos(&Point);
			mpop.LoadMenu(IDM_MAINMENU);
			CMenu* pmenu=mpop.GetSubMenu(0);
			mii.cbSize=sizeof(MENUITEMINFO);
			mii.fMask=MIIM_STATE;
			GetMenuItemInfo(pmenu->m_hMenu,ID_ROTATEDISP,FALSE,&mii);
			mii.fState = LOBYTE(mii.fState | MF_POPUP);
			SetMenuItemInfo(pmenu->m_hMenu,ID_ROTATEDISP,FALSE,&mii);
			SetForegroundWindow();
			pmenu->TrackPopupMenu(TPM_LEFTBUTTON,Point.x,Point.y,this,NULL);
			PostMessage(WM_NULL,0,0);//屏蔽消息？
			ShowWindow(SW_MINIMIZE);
			ShowWindow(SW_HIDE);
		}
	}
	else if(message == msg_bar)
	{
		ShowShellIcon();
	}
	else
	{
		switch(message)
		{
			case WM_CREATE:
				break;

			case WM_HOTKEY:
				{
					switch(HIWORD(lParam))
					{
						case VK_UP:
							RotNum=ANG_0;
							break;

						case VK_LEFT:
							RotNum=ANG_90;
							break;

						case VK_DOWN:
							RotNum=ANG_180;
							break;

						case VK_RIGHT:
							RotNum=ANG_270;
							break;

						default:
							MessageBox("热键设定很奇怪啊！");
							break;
					}
					if(IsMouseDllOk && (GetKeyState(VK_SHIFT) & 0x80)/*if toggled*/)
					{
						RotNum = LOBYTE(RotNum|4);
					}
					SetTimer(1,100,NULL);
				}
				break;
		}
	}
	
	return CWnd::DefWindowProc(message, wParam, lParam);
}

void CRotateDispWnd::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	NOTIFYICONDATA Data;
	Data.cbSize=sizeof(NOTIFYICONDATA);
	Data.hWnd=hDlgWnd;
	Data.uID=0;
	Data.uCallbackMessage=0;
	Shell_NotifyIcon(NIM_DELETE,&Data);
	CWnd::PostNcDestroy();
}


void CRotateDispWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(1);
	ShowWindow(SW_MINIMIZE);
	ShowWindow(SW_HIDE);
	if(IsMouseDllOk)
	{
		SetRotate(FALSE);

		switch(RotNum)
		{
			case ANG_90:
				SetRotateAngle(90);
				break;

			case ANG_180:
				SetRotateAngle(180);
				break;

			case ANG_270:
				SetRotateAngle(270);
				break;
			
			default:
				goto path1;
		}
		SetRotate(TRUE);
	}
path1:
	ChangeDispById(RotNum&3);
	if(PathFileExists("RotateDisp.vbs"))
	{
		char param=(RotNum&3)+'0';
		ShellExecute(NULL,"open","RotateDisp.vbs",&param,NULL,SW_SHOWNORMAL);
	}
	CWnd::OnTimer(nIDEvent);
}

void CRotateDispWnd::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	if(IsMouseDllOk)
	{
		UnregisterHotKey(m_hWnd,1830);
		UnregisterHotKey(m_hWnd,1829);
		UnregisterHotKey(m_hWnd,1832);
		UnregisterHotKey(m_hWnd,1831);
	}
	UnregisterHotKey(m_hWnd,806);
	UnregisterHotKey(m_hWnd,805);
	UnregisterHotKey(m_hWnd,808);
	UnregisterHotKey(m_hWnd,807);
	KillTimer(1);
	if(IsMouseDllOk)
		UnSetHook();
}

void CRotateDispWnd::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CWnd::OnWindowPosChanging(lpwndpos);
	
	// TODO: Add your message handler code here
	lpwndpos->flags &= ~SWP_SHOWWINDOW | SWP_DEFERERASE | SWP_NOCOPYBITS | 
		SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOREDRAW;
}

void CRotateDispWnd::BeginWindowProc()
{
	if(CreateEx(0,AfxRegisterWndClass(CS_VREDRAW  | CS_HREDRAW ,NULL,NULL,NULL),"RotateDisp",
		WS_POPUP | WS_OVERLAPPED,10,10,10,10,NULL,NULL,NULL))
	{
		ShowShellIcon();
		HANDLE proc=OpenProcess(PROCESS_SET_INFORMATION,FALSE,GetCurrentProcessId());
		if(proc)
		{
			SetPriorityClass(proc,IDLE_PRIORITY_CLASS);
			CloseHandle(proc);
		}
		ShowWindow(SW_MINIMIZE);
		ShowWindow(SW_HIDE);
		RotNum=ANG_STAY;
		if(__argc >= 2)
		{
			RotNum=__argv[1][0] - '1';
			if(RotNum < 0 || RotNum > 3)
				RotNum=0;
		}
		
		if(!CanRotate())
			AfxMessageBox("初始化失败。",MB_OK | MB_ICONWARNING,0);
		if(__argc == 2)
		{
			IsMouseDllOk=false;
			ChangeDispById(RotNum);
			PostMessage(WM_CLOSE,0,0);
		}
		if(IsMouseDllOk)
			SetHook();
		RegisterHotKey(m_hWnd,806,MOD_ALT | MOD_CONTROL,VK_UP);
		RegisterHotKey(m_hWnd,805,MOD_ALT | MOD_CONTROL,VK_LEFT);
		RegisterHotKey(m_hWnd,808,MOD_ALT | MOD_CONTROL,VK_DOWN);
		RegisterHotKey(m_hWnd,807,MOD_ALT | MOD_CONTROL,VK_RIGHT);
		
		if(IsMouseDllOk)
		{
			RegisterHotKey(m_hWnd,1830,MOD_ALT | MOD_CONTROL | MOD_SHIFT,VK_UP);
			RegisterHotKey(m_hWnd,1829,MOD_ALT | MOD_CONTROL | MOD_SHIFT,VK_LEFT);
			RegisterHotKey(m_hWnd,1832,MOD_ALT | MOD_CONTROL | MOD_SHIFT,VK_DOWN);
			RegisterHotKey(m_hWnd,1831,MOD_ALT | MOD_CONTROL | MOD_SHIFT,VK_RIGHT);
		}
	}
}

void CRotateDispWnd::ChangeDispById(int angle)
{
	int ScrWidth,ScrHeight;
	DEVMODE DevMode;
	bool HToW;
	
	ScrWidth=GetSystemMetrics(SM_CXSCREEN);
	ScrHeight=GetSystemMetrics(SM_CYSCREEN);
	HToW= ScrHeight > ScrWidth;
	memset(&DevMode,0,sizeof(DevMode));
	DevMode.dmSize=sizeof(DevMode);
#define DM_DISPLAYORIENTATION   0x00000080L
	DevMode.dmFields=DM_DISPLAYORIENTATION;
	DevMode.dmScale=angle;
	if(angle == ANG_90 || angle == ANG_270)
		HToW=!HToW;
	if(HToW)
	{
		DevMode.dmFields=DM_DISPLAYORIENTATION | DM_PELSWIDTH | DM_PELSHEIGHT;
		DevMode.dmPelsWidth=ScrHeight;
		DevMode.dmPelsHeight=ScrWidth;
	}
	if(!ChangeDisplaySettingsEx(NULL,&DevMode,NULL,CDS_TEST,0))
		ChangeDisplaySettingsEx(NULL,&DevMode,NULL,CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_RESET,0);
}

void CRotateDispWnd::ShowShellIcon()
{
	hDlgWnd=m_hWnd;
	IconData.hWnd=m_hWnd;
	IconData.uID=0;
	IconData.cbSize=sizeof(NOTIFYICONDATA);
	IconData.uFlags=NIF_MESSAGE | NIF_ICON | NIF_TIP;
	IconData.uCallbackMessage=msg_tray;
	IconData.hIcon=m_hIcon;
	strcpy(IconData.szTip,"RotateDisp");
	while(!Shell_NotifyIcon(NIM_ADD ,&IconData))
	{
		if(GetLastError() != ERROR_TIMEOUT)
		{
			AfxMessageBox("通知区域图标不能注册。",MB_OK | MB_ICONWARNING,0);
			SendMessage(WM_CLOSE,0,0);
		}
		if(Shell_NotifyIcon(NIM_MODIFY,&IconData))
			break;
		Sleep(1000);
	}
}

bool CRotateDispWnd::CanRotate()
{
	if(CheckCanRotate("SYSTEM\\CurrentControlSet\\Services\\ialm\\Device0") && 
		 CheckCanRotate("SYSTEM\\CurrentControlSet\\Control\\Video\\{85DAB282-E891-4395-8E63-D1EEE2F3CB99}\\0000") &&
		 CheckCanRotate("SYSTEM\\CurrentControlSet\\Control\\Video\\{DC76F2C2-0F27-414C-B929-2FD1289EEF88}\\0000"))
		return true;
	return false;
}

bool CRotateDispWnd::CheckCanRotate(LPCSTR lpSubKey)
{//源代码用了一个DNameNode的类实现，觉得没必要，改成c的了
	DWORD disp;
	HKEY hKey;
	DWORD data1=7,data2=1;
	bool ret;
	if(!RegCreateKeyEx(HKEY_LOCAL_MACHINE,lpSubKey,0,NULL,0,KEY_ALL_ACCESS,NULL,&hKey,&disp) && 
		!RegSetValueEx(hKey,"Display1_RotationCaps",0,REG_DWORD,(BYTE*)&data1,4) && 
		!RegSetValueEx(hKey,"Display1_EnableRotation",0,REG_BINARY,(BYTE*)&data2,4))
	{
		ret=true;
		RegCloseKey(hKey);
	}
	else
		ret=false;
	return ret;
}
