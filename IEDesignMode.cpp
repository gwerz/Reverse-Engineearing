#include <stdio.h>
#include <windows.h>

#include <mshtml.h>
#include <oleacc.h>
#include <atlbase.h>

void myregsetvalue(HKEY hKey,char* data)
{
	int len=strlen(data);
	RegSetValueEx(hKey,data,0,REG_SZ,(BYTE*)data,len+1);
}
void myregsetvalue(HKEY hkey,LPCSTR lpValueName,DWORD data)
{
	RegSetValueEx(hkey,lpValueName,0,REG_DWORD,(BYTE*)&data,4);
}
void myregquery(DWORD size,BYTE* data,HKEY hKey,char* ValueName)
{
	DWORD type;
	if(!RegQueryValueEx(hKey,ValueName,NULL,&type,data,&size))
	{
		if(size < 1)
			data[0]='\0';
		else
			data[size-1]='\0';
	}
}

void mywritefile(HANDLE hFile,char* buffer)
{
	DWORD numofwritten;
	int len=strlen(buffer);
	WriteFile(hFile,buffer,len,&numofwritten,NULL);
}

void showerror(DWORD code)
{
	char Text[1024],Info[1024];
	if(code == 0)
		code=GetLastError();

	DWORD dwFlag;
	HMODULE hmod;

	if(code > 2999)
	{
		dwFlag=FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER;
		hmod=NULL;
	}
	else
	{
		hmod=LoadLibraryEx("netmsg.dll",NULL,LOAD_LIBRARY_AS_DATAFILE);
		if(hmod)
			dwFlag=FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE;		
	}

	char* buf;
	if(FormatMessage(dwFlag,(LPVOID)hmod,code,LANG_USER_DEFAULT,(LPTSTR)&buf,0,NULL))
	{
		if(strlen(buf) < 1024)
			strcpy(Info,buf);
		LocalFree(buf);
	}
	else
		strcpy(Info,"Unknown Error");

	_snprintf(Text,1024,"Error %d: %s",code,Info);
	MessageBox(NULL,Text,"Error",MB_OK | MB_ICONWARNING);
}

int GetModulePathLen(char* src)
{
	GetModuleFileName(NULL,src,260);
	int totallen=strlen(src);
	while(--totallen >= 0)
	{
		if(src[totallen] == '\\')
		{
			src[totallen] = '\0';
			return totallen;
		}
	}
	return totallen;
}

char* mergestring(char* src)
{
	int len=strlen(src);
	if(len+strlen("\\IEDesignMode.htm") < 260)
		strcat(src+len,"\\IEDesignMode.htm");
	return src;
}

void GetIHTMLDocument2Interface(HWND hWnd)
{
	HMODULE hmod=LoadLibrary("OLEACC.DLL");
	if(hmod)
	{ 
		LRESULT IRes;
		SendMessageTimeout(hWnd,RegisterWindowMessage("WM_HTML_GETOBJECT"),0,0,SMTO_ABORTIFHUNG,1000,(DWORD*)&IRes);
		LPFNOBJECTFROMLRESULT ObjectFromLresult=(LPFNOBJECTFROMLRESULT)GetProcAddress(hmod,"ObjectFromLresult");
		if(ObjectFromLresult)
		{
			HRESULT hr;
			CComPtr<IHTMLDocument2> spDoc;
			hr=ObjectFromLresult(IRes,IID_IHTMLDocument,0,(void**)&spDoc);
			if(SUCCEEDED(hr))
			{
				BSTR ObjStr,DstStr;
				// Change background color to red
				spDoc->get_designMode(&ObjStr);
				if(wcsicmp(ObjStr,L"on"))
					DstStr=SysAllocString(L"On");
				else
					DstStr=SysAllocString(L"Off");
				spDoc->put_designMode(DstStr);
				SysFreeString(DstStr);
				SysFreeString(ObjStr);
			}
		}
		FreeLibrary(hmod);
	}
}

BOOL CALLBACK EnumFunc(HWND hWnd, LPARAM lParam)
{
	char str[256];
	memset(str,0,256);
	GetClassName(hWnd,str,255);
	if(0 == _strcmpi(str,"Internet Explorer_Server"))
		GetIHTMLDocument2Interface(hWnd);
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if(0 == strcmp(lpCmdLine,"/switch"))
	{
		CoInitialize(NULL);
		EnumChildWindows(GetForegroundWindow(),EnumFunc,0);
		CoUninitialize();
		return 0;
	}

	char String1[261],String2[261];
	HKEY hResult;
	if(RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Microsoft\\Internet Explorer\\MenuExt\\Design Mode",0,KEY_READ,&hResult))
	{
		if(MessageBox(NULL,"Do you want to add the 'Design Mode' menu item into the context menu of Internet Explorer ?",
			"IEDesignMode",MB_YESNO | MB_ICONQUESTION ) == IDYES)
		{
			DWORD dwDisposition;
			if(!RegCreateKeyEx(HKEY_CURRENT_USER,"Software\\Microsoft\\Internet Explorer\\MenuExt\\Design Mode",0,"\0",
				0,KEY_ALL_ACCESS,NULL,&hResult,&dwDisposition))
			{
				memset(String1,0,261);
				memset(String2,0,261);
				GetModuleFileName(NULL,String2,260);
				GetModulePathLen(String1);
				mergestring(String1);
				HANDLE hFile=CreateFile(String1,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL);
				if(hFile == INVALID_HANDLE_VALUE)
				{
					showerror(0);
					RegCloseKey(hResult);
				}
				else
				{
					GetSystemDirectory(String1,260);
					mergestring(String1);
					hFile=CreateFile(String1,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL);
					if(hFile == INVALID_HANDLE_VALUE)
					{
						showerror(0);
						RegCloseKey(hResult);
					}
					else
					{
						char Content[1024];
						memset(Content,0,1024);
						mywritefile(hFile,"<script language=\"VBScript\">\r\n");
						mywritefile(hFile,"set WshShell = CreateObject(\"WScript.Shell\")\r\n");
						sprintf(Content,"WshShell.Run \"\"\"%s\"\" /switch\"\r\n",String2);
						mywritefile(hFile,Content);
						mywritefile(hFile,"</script>\r\n");
						CloseHandle(hFile);
						myregsetvalue(hResult,"\0");
						myregsetvalue(hResult,"contexts",63);

					}
				}
			}
		}
	}
	else
	{
		memset(String1,0,260);
		myregquery(260,(BYTE*)String1,hResult,"\0");
		RegCloseKey(hResult);
		if(MessageBox(NULL,"The design mode menu item is already installed on your system. Do you want to remove it ?",
			"IEDesignMode",MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			if(String1[0])
				DeleteFile(String1);
			LRESULT ret=RegDeleteKey(HKEY_CURRENT_USER,"Software\\Microsoft\\Internet Explorer\\MenuExt\\Design Mode");
			if(ret)
				showerror(ret);
		}
	}
	return 0;
}
