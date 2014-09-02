// RotateMouse.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

HINSTANCE	hMod=NULL;
HHOOK		hhook=NULL;
int			Angle=0;
BOOL		BeginRotate=FALSE;
LRESULT WINAPI MouseLLHookProc(int nCode,WPARAM wParam,LPARAM lParam);

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	hMod=(HINSTANCE)hModule;
    return TRUE;
}

__declspec(dllexport) void WINAPI SetHook()
{
	BeginRotate=FALSE;
	Angle=0;
	hhook=SetWindowsHookEx(WH_MOUSE_LL,MouseLLHookProc,hMod,0);
}

__declspec(dllexport) void WINAPI SetReverseX(int)
{

}

__declspec(dllexport) void WINAPI SetReverseY(int)
{
	
}

__declspec(dllexport) void WINAPI SetRotate(BOOL begin)
{
	BeginRotate=begin;
}

__declspec(dllexport) void WINAPI SetRotateAngle(int angle)
{
	Angle=angle;
}

__declspec(dllexport) void WINAPI UnSetHook()
{
	if(hhook)
	{
		UnhookWindowsHookEx(hhook);
		hhook=NULL;
	}
}

LRESULT WINAPI MouseLLHookProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode == HC_ACTION && BeginRotate && Angle != 0 && wParam == WM_MOUSEMOVE)
	{
		POINT Point;
		MSLLHOOKSTRUCT* mhs=(MSLLHOOKSTRUCT*)lParam;
		GetCursorPos(&Point);
		int len=mhs->pt.x-Point.x;
		int hei=mhs->pt.y-Point.y;
		int newx,newy;
		switch(Angle)
		{
			case 90:
				newx=Point.x-hei;
				newy=len+Point.y;
				Point.x -= hei;
				Point.y=newy;
				break;

			case 180:
				newx=Point.x-len;
				newy=Point.y-hei;
				Point.x -= len;
				Point.y=newy;			
				break;

			case 270:
				newx=hei+Point.x;
				newy=Point.y-len;
				Point.x += hei;
				Point.y=newy;
				break;

			default:
				newx=mhs->pt.x;
				newy=mhs->pt.y;
				Point.x=newx;
				Point.y=newy;
				break;
		}
		SetCursorPos(newx,newy);
		return -1;
	}
	return CallNextHookEx(hhook,nCode,wParam,lParam);
}
