#include <windows.h>
#include <richedit.h>
#include "shlobj.h"

HANDLE g_hInstance;
GUID CLSID_SysTrayInvoker={0x730F6CDC,0x2C86,0x11D2,0x87,0x73,0x92,0xE2,0x20,0x52,0x41,0x53};

int WINAPI intval(LPSTR cmd)
{
	LPSTR ptr=cmd;
	int result=0;

	while(TRUE)
	{//将整数字符串解析出整数："12345" -> 12345
		if(*ptr < '0' || *ptr > '9') break;
		result=*ptr+10*result-'0';
		ptr++;
	}
	return result;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	HWND hwnd=FindWindowW(L"SystemTray_Main",NULL);//属于explorer.exe
	int number=intval(lpCmdLine);
	g_hInstance=hInstance;
	if(hwnd == NULL)
	{
		if(SUCCEEDED(SHLoadInProc(CLSID_SysTrayInvoker)))
		{
			int i=0;
			while(TRUE)
			{
				Sleep(1000);
				if(FindWindowW(L"SystemTray_Main",0))
					break;
				i++;
				if(i>=30) return 0;
			}
		}
	}

	PostMessageW(hwnd,0x4DC,number,1);//?????EM_OUTLINE
	return 0;
}