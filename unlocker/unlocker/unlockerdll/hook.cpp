// hook.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <shellapi.h>
#include <shlwapi.h>

#pragma comment(lib,"shlwapi.lib")

#define MYDLLEXPORT extern "C" __declspec(dllexport)

typedef int (WINAPI* SHFILEOPERATION)(LPSHFILEOPSTRUCTW lpFileOp);
Thunk GlobalThunk;


int WINAPI InjectCheck(LPSHFILEOPSTRUCTW lpFileOp)
{//SHFileOperation的外壳检测程序，用来替换SHFileOperatio
	int errorcode=((SHFILEOPERATION)&GlobalThunk.codeseg.codedata)(lpFileOp);
	if(errorcode == ERROR_ACCESS_DENIED || errorcode == ERROR_SHARING_VIOLATION)
	{
		DWORD nRet=GetLastError();
		if(lpFileOp->wFunc ==  FO_MOVE || lpFileOp->wFunc == FO_DELETE || lpFileOp->wFunc == FO_RENAME)
		{
			if(nRet == ERROR_INVALID_HANDLE || nRet == ERROR_SUCCESS)//MSDN上说不要检查getlasterror。。。
			{
				SHELLEXECUTEINFOW ExecInfo;
				WCHAR szPath[1024],file[1024],fparam[1024];
				memset(&ExecInfo,0,sizeof(SHELLEXECUTEINFOW));
				ExecInfo.cbSize=sizeof(SHELLEXECUTEINFOW);
				ExecInfo.lpVerb=L"open";
				GetModuleFileNameW(GlobalThunk.hmod,szPath,1024);
				PathRemoveFileSpecW(szPath);
				wsprintfW(file,L"\"%s\\Unlocker.exe\"",szPath);
				ExecInfo.lpFile=file;
				wsprintfW(fparam,L"\"%s\"",lpFileOp->pFrom);
				ExecInfo.lpParameters=fparam;
				ExecInfo.nShow=SW_SHOWNORMAL;
				ShellExecuteExW(&ExecInfo);
			}
		}
	}
	return errorcode;
}

BOOL CALLBACK EnumFunc(HWND hwnd,LPARAM lParam)
{
#define UNKNOWNMSG		0x8005//参见unlockerassitant代码
	TCHAR String[256];
	GetWindowText(hwnd,String,256);
	if(!lstrcmp(String,"UnlockerAssistant"))
		PostMessage(hwnd,UNKNOWNMSG,0,0);
	return TRUE;
}

LRESULT CALLBACK CBTProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	return CallNextHookEx(GlobalThunk.hhook,nCode,wParam,lParam);
}

MYDLLEXPORT void HookInstall()
{
	GlobalThunk.hhook=SetWindowsHookEx(WH_CBT,(HOOKPROC)CBTProc,GlobalThunk.hmod,0);
}

MYDLLEXPORT void HookUninstall()
{
	UnhookWindowsHookEx(GlobalThunk.hhook);
}

BOOL APIENTRY DllMain(HANDLE hModule,DWORD ul_reason_for_call,LPVOID lpReserved)
{
	switch(ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			{
				WCHAR String[1024];
				HMODULE hmod;
				GlobalThunk.hmod=(HMODULE)hModule;
				DisableThreadLibraryCalls((HMODULE)hModule);
				lstrcpyW(String,L"");
				GetModuleFileNameW(NULL,String,1024);
				if(lstrlenW(String))
				{
					PathStripPathW(String);
					if(!lstrcmpiW(String,L"explorer.exe") && (hmod=GetModuleHandle("Shell32.dll")))
					{
						DWORD flOldProtect;
						GlobalThunk.FuncBaseAddress=GetProcAddress(hmod,"SHFileOperationW");
						if(!GlobalThunk.FuncBaseAddress || !VirtualProtect(&GlobalThunk,11,PAGE_EXECUTE_READWRITE,&flOldProtect))
						{//修改数据段为代码段且有可读写权限
							CloseHandle(hmod);
							return TRUE;
						}
						HANDLE hProcess=GetCurrentProcess();
						BYTE Buffer[5];//修改SHFileOperation的前几字节，使其能跳转到InjectCheck
						ReadProcessMemory(hProcess,(LPVOID)GlobalThunk.FuncBaseAddress,&GlobalThunk,6,NULL);
						if(GlobalThunk.codeseg.codedata[3] == 0x83 && GlobalThunk.codeseg.codedata[4] == 0xEC)
						{
/*
可知是32位xp，SHFileOperationW函数的机器码前10字节:558BEC83EC2C5356578B7D0833C0 对应汇编代码：
	seg000:00000000                 push    ebp
	seg000:00000001                 mov     ebp, esp
	seg000:00000003                 sub     esp, 2Ch
	seg000:00000006                 push    ebx		
	seg000:00000007                 push    esi
	seg000:00000008                 push    edi
	seg000:00000009                 mov     edi, [ebp+8]
	seg000:0000000C                 xor     eax, eax
	............................................
*/
							int offset=((int)GlobalThunk.FuncBaseAddress+6)-int(GlobalThunk.codeseg.codedata+11);
/*GlobalThunk.codeseg.codedata+11：代码执行GlobalThunk结束位置，相当于已经执行了SHFileOperation的一部分
jmp指令占5字节，从GlobalThunk跳回到SHFileOperation继续执行下面的部分，则jmp的偏移从jmp下句开始算，
即下面的codedata+0000000B，而SHFileOperation要执行的位置在FuncBaseAddress+00000006的push    ebx
thunk中的指令：
	seg000:00000000                 push    ebp
	seg000:00000001                 mov     ebp, esp
	seg000:00000003                 sub     esp, 2Ch
	seg000:00000006					jmp		offset
	seg000:0000000B					00000000000
	............................................
*/
//以下操作得目的主要是将SHFileOperation拆开，则每次程序调用SHFileOperation就可以执行InjectCheck一次
							GlobalThunk.codeseg.xp.jmp=0xE9;//写入jmp指令机器码
							memcpy(&GlobalThunk.codeseg.xp.offset,&offset,4);//写入jmp的offset
							FlushInstructionCache(hProcess,&GlobalThunk,11);
							offset=(int)InjectCheck-((int)GlobalThunk.FuncBaseAddress+5);//从GlobalThunk的代码跳到InjectCheck函数
							Buffer[0]=0xE9;//写入跳转指令
							memcpy(Buffer+1,&offset,4);
							WriteProcessMemory(hProcess,GlobalThunk.FuncBaseAddress,Buffer,5,NULL);
							FlushInstructionCache(hProcess,GlobalThunk.FuncBaseAddress,5);
						}
						else if((GlobalThunk.codeseg.codedata[3] == 0x8B && GlobalThunk.codeseg.codedata[4] == 0xEC) || 
									GlobalThunk.codeseg.codedata[3] == 0x53 || GlobalThunk.codeseg.codedata[4] == 0x55)
						{//32位win7 与上面原理相似，不做分析了
							int offset=((int)GlobalThunk.FuncBaseAddress+5)-int(GlobalThunk.codeseg.codedata+10);
							GlobalThunk.codeseg.win7.jmp=0xE9;
							memcpy(&GlobalThunk.codeseg.win7.offset,&offset,4);
							FlushInstructionCache(hProcess,&GlobalThunk,11);
							offset=(int)InjectCheck-((int)GlobalThunk.FuncBaseAddress+5);//从GlobalThunk的代码跳到InjectCheck函数
							Buffer[0]=0xE9;//写入跳转指令
							memcpy(Buffer+1,&offset,4);
							WriteProcessMemory(hProcess,GlobalThunk.FuncBaseAddress,Buffer,5,NULL);
							FlushInstructionCache(hProcess,GlobalThunk.FuncBaseAddress,5);
						}
						else
						{
							EnumWindows((WNDENUMPROC)EnumFunc,0);
						}
						CloseHandle(hmod);
					}
				}
/*
执行顺序：①②③④⑤   整个过程相当于替换SHFileOperation为InjectCheck
经过处理的函数应该类似于如下：
GlobalThunk.CodeSeg函数:
push	ebp.//③跳到这里
mov		ebp,esp
sub		esp,2Ch
jmp		SHFileOperation//④继续执行剩下的SHFileOperation
..........以下无代码

SHFileOperation函数:
jmp		InjectCheck//①一旦有程序调用该函数，则直接跳
push	ebx//⑤跳到这里继续执行
push	esi
...省略

InjectCheck函数:
GlobalThunk.CodeSeg(LPSHFILEOPSTRUCTW lpFileOp);//②跳到这里执行GlobalThunk.CodeSeg其实也就是执行SHFileOperation的前几句
...检查返回值//⑥返回InjectCheck
...
...省略

*/
			}
			break;

		case DLL_PROCESS_DETACH:
			if(GlobalThunk.FuncBaseAddress)
			{
				HANDLE hproc=GetCurrentProcess();
				WriteProcessMemory(hproc,(LPVOID)GlobalThunk.FuncBaseAddress,&GlobalThunk,5,NULL);//结束时恢复SHFileOperation原始机器码
				FlushInstructionCache(hproc,(LPVOID)GlobalThunk.FuncBaseAddress,5);
			}
			break;

		default:
			break;
	}

    return TRUE;
}