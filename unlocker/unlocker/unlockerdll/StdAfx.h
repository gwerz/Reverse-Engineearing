// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__2ADEAEDE_D507_4201_8E51_D93D95FD91A9__INCLUDED_)
#define AFX_STDAFX_H__2ADEAEDE_D507_4201_8E51_D93D95FD91A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

struct Thunk
{
	union//要动态执行的代码
	{//此处分xp和win7是因为SHFileOperationW函数实现区别
		struct
		{//32位xp
			BYTE	origincode[6];//xp的SHFileOperationW函数前6字节机器码
			BYTE	jmp;//跳转指令E9
			int		offset;//为jmp指令计算偏移
		}xp;
		
		struct
		{//32位win7
			BYTE	origincode[5];//win7的SHFileOperationW函数前5字节机器码
			BYTE	jmp;//跳转指令E9
			int		offset;//为jmp指令计算偏移
		}win7;
		
		BYTE codedata[12];
	}codeseg;
	
	DWORD	initfuncnum;//此处无用
	HMODULE hmod;
	HHOOK	hhook;
	FARPROC	FuncBaseAddress;
};

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2ADEAEDE_D507_4201_8E51_D93D95FD91A9__INCLUDED_)
