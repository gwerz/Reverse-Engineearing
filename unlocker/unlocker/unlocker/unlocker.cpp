#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <aclapi.h>
#include <tlhelp32.h>
#include "language.h"
#include "unlocker.h"

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"shell32.lib")

typedef void (*GlobalFunc)();
GlobalFunc Global_Func[]={NULL,GlobalStart1,GlobalStart2,GlobalStart3,GlobalStart4,GlobalStart5};
GlobalFunc InitialFuncArray[256];

GeneralClass<int> ListItemGeneral;	
GeneralClass<ProcessInfo> ListProcessGeneral;
GeneralClass<WCHAR [4][1024]> StringGeneral1,StringGeneral2;
GeneralClass<ProcessSInfo> ProcessSInfoGeneral;

DWORD ProcessGuage=0x20000;
TCHAR GlobalStrings[42][256];
TCHAR WindowTitle[256];
HANDLE UpdateThread;
WCHAR GlobalFileName[1024];
bool DriverRegInfoModified=FALSE;
UNICODE_STRING UnlockerDriverRegInfo={0,0,NULL};
WCHAR ADsPath[1024];
HCURSOR GlobalCursor;
LONG UpdateWindowOldProcedure;
BOOL IsCaptured;
OperateFileClass* GlobalOperateFile=NULL;
CoreAPIArray* GlobalCoreAPI;//!!!隆兜兵晒
bool CangetOwnership=TRUE;
HWND UnlockAllButtonhWnd,QuitButtonhWnd,ListCtrlhWnd,UnlockButtonhWnd,KillProcessButtonhWnd,ComboBoxhWnd;
WCHAR EditString[1024],szPath[1024],ssPath[1024];
int ComboBoxSelectIndex;
unsigned char ListSortDirection[5]={1,1,1,1,1};//電會圭��
RECT ListCtrlRect,QuitButtonRect,UnlockAllButtonRect,UnlockButtonRect,KillProcessButtonRect,ComboBoxRect;
int InitialFuncNum=0;//隆兜兵晒
int tmpnum=0;
bool IsToKillProcess;

BOOL DriverInit(LPWSTR path)
{
	WCHAR str[1024];
	HKEY hKey;
	wsprintfW(str,L"%s\\UnlockerDriver5.sys",path);
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"System\\CurrentControlSet\\Services\\UnlockerDriver5\\",0,REG_SZ,&hKey))
	{
		DriverRegInfoModified=TRUE;
		HKEY hkResult;
		DWORD dwDisposition;
		if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,"System\\CurrentControlSet\\Services\\UnlockerDriver5\\",
			0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&hkResult,&dwDisposition))
		{
			RegCloseKey(hKey);
		}
		else
		{
			WCHAR strpath[1024];
			wsprintfW(strpath,L"\\??\\%s",str);
			if(!RegSetValueExW(hkResult,L"ImagePath",0,REG_SZ,(BYTE*)strpath,2*lstrlenW()+2))
			{
				DWORD Data=REG_SZ;
				if(!RegSetValueEx(hkResult,"Type",0,REG_DWORD,(BYTE*)&Data,sizeof(DWORD)))
				{
					RegCloseKey(hkResult);
					RegCloseKey(hKey);
					goto SETREG;
				}
			}
			RegCloseKey(hkResult);
			RegCloseKey(hKey);
		}
		return FALSE;
	}
	
SETREG:
	GetMyCoreAPIArray()->RtlInitUnicodeString(&UnlockerDriverRegInfo,L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\UnlockerDriver5");
	if(GetMyCoreAPIArray()->NtLoadDriver(&UnlockerDriverRegInfo))
	{
		GetMyCoreAPIArray()->NtUnloadDriver(&UnlockerDriverRegInfo);
		if(GetMyCoreAPIArray()->NtLoadDriver(&UnlockerDriverRegInfo))
			return FALSE;
	}
	return TRUE;
}

int DriverUnload()
{
	HKEY hKey;
	GetMyCoreAPIArray()->NtUnloadDriver(&UnlockerDriverRegInfo);
	if(DriverRegInfoModified && !RegOpenKeyEx(HKEY_LOCAL_MACHINE,"System\\CurrentControlSet\\Services\\",
		0,DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,&hKey))
	{
		SHDeleteKey(hKey,"UnlockerDriver5");
		RegCloseKey(hKey);
	}
	return 1;
}

void CopyString(TCHAR (*objstring)[256],LPSTR string1,LPSTR string2,LPSTR string3,LPSTR string4,LPSTR string5,LPSTR string6,
							LPSTR string7,LPSTR string8,LPSTR string9,LPSTR string10,LPSTR string11,LPSTR string12,LPSTR string13,
							LPSTR string14,LPSTR string15,LPSTR string16,LPSTR string17,LPSTR string18,LPSTR string19,LPSTR string20,
							LPSTR string21,LPSTR string22,LPSTR string23,LPSTR string24,LPSTR string25,LPSTR string26,LPSTR string27,
							LPSTR string28,LPSTR string29,LPSTR string30,LPSTR string31,LPSTR string32,LPSTR string33,LPSTR string34,
							LPSTR string35,LPSTR string36,LPSTR string37,LPSTR string38,LPSTR string39,LPSTR string40,LPSTR string41,
							LPSTR string42)
{
	lstrcpy(objstring[0],string1);
	lstrcpy(objstring[1],string2);
	lstrcpy(objstring[2],string3);
	lstrcpy(objstring[3],string4);
	lstrcpy(objstring[4],string5);
	lstrcpy(objstring[5],string6);
	lstrcpy(objstring[6],string7);
	lstrcpy(objstring[7],string8);
	lstrcpy(objstring[8],string9);
	lstrcpy(objstring[9],string10);
	lstrcpy(objstring[10],string11);
	lstrcpy(objstring[11],string12);
	lstrcpy(objstring[12],string13);
	lstrcpy(objstring[13],string14);
	lstrcpy(objstring[14],string15);
	lstrcpy(objstring[15],string16);
	lstrcpy(objstring[16],string17);
	lstrcpy(objstring[17],string18);
	lstrcpy(objstring[18],string19);
	lstrcpy(objstring[19],string20);
	lstrcpy(objstring[20],string21);
	lstrcpy(objstring[21],string22);
	lstrcpy(objstring[22],string23);
	lstrcpy(objstring[23],string24);
	lstrcpy(objstring[24],string25);
	lstrcpy(objstring[25],string26);
	lstrcpy(objstring[26],string27);
	lstrcpy(objstring[27],string28);
	lstrcpy(objstring[28],string29);
	lstrcpy(objstring[29],string30);
	lstrcpy(objstring[30],string31);
	lstrcpy(objstring[31],string32);
	lstrcpy(objstring[32],string33);
	lstrcpy(objstring[33],string34);
	lstrcpy(objstring[34],string35);
	lstrcpy(objstring[35],string36);
	lstrcpy(objstring[36],string37);
	lstrcpy(objstring[37],string38);
	lstrcpy(objstring[38],string39);
	lstrcpy(objstring[39],string40);
	lstrcpy(objstring[40],string41);
	lstrcpy(objstring[41],string42);
}

void InitString()
{
	HKEY hKey;
	BYTE Data[1024];
	DWORD cbData=1024;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Unlocker",0,KEY_QUERY_VALUE,&hKey);
	DWORD codepage=1033;
	if(!RegQueryValueEx(hKey,"Language",NULL,NULL,Data,&cbData))
		codepage=StrToInt((LPCSTR)Data);//2052
	RegCloseKey(hKey);
	if(codepage == 2052)
	{
		CopyString(GlobalStrings,CODEPAGE2052);
	}	
	else
	{
		CopyString(GlobalStrings,CODEPAGEDEFAULT);
	}
}

bool SetACL(LPWSTR OjbectName)
{
	PSID pEveryoneSID=NULL,pAdminSID=NULL;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld=SECURITY_WORLD_SID_AUTHORITY;
	if(!AllocateAndInitializeSid(&SIDAuthWorld,1,SECURITY_WORLD_RID,0,0,0,0,0,0,0,&pEveryoneSID))
	{//ERROR
		if(pEveryoneSID) FreeSid(pEveryoneSID);
		return FALSE;
	}

	SID_IDENTIFIER_AUTHORITY SIDAuthNT=SECURITY_NT_AUTHORITY;
	if(!AllocateAndInitializeSid(&SIDAuthNT,2,SECURITY_BUILTIN_DOMAIN_RID,DOMAIN_ALIAS_RID_ADMINS,
		0,0,0,0,0,0,&pAdminSID))
	{
		if(pAdminSID) FreeSid(pAdminSID);
		if(pEveryoneSID) FreeSid(pEveryoneSID);
		return FALSE;
	}

	EXPLICIT_ACCESS ea[2];
	ZeroMemory(ea,2*sizeof(EXPLICIT_ACCESS));
	ea[0].grfAccessPermissions=GENERIC_ALL;
	ea[0].grfAccessMode=SET_ACCESS;
	ea[0].grfInheritance=NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm=TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType=TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName=(LPSTR)pEveryoneSID;
	ea[1].grfAccessPermissions=GENERIC_ALL;
	ea[1].grfAccessMode=SET_ACCESS;
	ea[1].grfInheritance=NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm=TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType=TRUSTEE_IS_GROUP;
	ea[1].Trustee.ptstrName=(LPSTR)pAdminSID;

	PACL pACL=NULL;
	if(!SetEntriesInAcl(2,ea,NULL,&pACL))
	{
		DWORD nRet=SetNamedSecurityInfoW(OjbectName,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL,NULL,pACL,NULL);
		if(nRet == ERROR_SUCCESS)
		{
ACLSUCCESS:
			if(pACL) LocalFree(pACL);
			if(pAdminSID) FreeSid(pAdminSID);
			if(pEveryoneSID) FreeSid(pEveryoneSID);
			return 1;
		}

		if(nRet == ERROR_ACCESS_DENIED)
		{
			if(CangetOwnership)
			{
				if(!SetNamedSecurityInfoW(OjbectName,SE_FILE_OBJECT,OWNER_SECURITY_INFORMATION,pAdminSID,NULL,NULL,NULL) &&
					!SetNamedSecurityInfoW(OjbectName,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL,NULL,pACL,NULL))
				{
					goto ACLSUCEESS;
				}
			}
			else
			{
				ShowMessageBox("%s\n\n%s",GlobalStrings[41],GlobalStrings[7]);//"Error Take Ownership Privileges"
			}
		}
	}

	if(pACL) LocalFree(pACL);
	if(pAdminSID) FreeSid(pAdminSID);
	if(pEveryoneSID) FreeSid(pEveryoneSID);
	return 0;
}

void NULLString(WCHAR (*objstring)[1024])
{
	lstrcpyW(objstring[0],L"");
	lstrcpyW(objstring[1],L"");
	lstrcpyW(objstring[2],L"");
	lstrcpyW(objstring[3],L"");
}

void ShowMessageBox(LPTSTR pszFmt,...)
{
	TCHAR Out[1024];
	va_list va;
	va_start(va,pszFmt);
	if(!GetMyOperateFile()->IsNotShowGUItoUnlock)
	{
		wvnsprintf(Out,1024,pszFmt,va);
		MessageBox(NULL,Out,"Unlocker 1.9.1",MB_OK);
	}
}

LPWSTR MergeString(LPWSTR Dst,LPCWSTR Src,LPCWSTR OptionalSrc)
{
	if(OptionalSrc && lstrlenW(OptionalSrc))
		wsprintfW(Dst,L"%s.%s",Src,OptionalSrc);
	else
		lstrcpyW(Dst,Src);
}

int DrawMessage(HWND hWnd,LPCSTR lpchText)
{
	HDC hdc=GetDC(hWnd);
	HFONT newfont=(HFONT)SendMessage(hWnd,WM_GETFONT,0,0);
	HFONT oldfont=(HFONT)SelectObject(hdc,(HGDIOBJ)newfont);
	RECT rc;
	DrawText(hdc,lpchText,-1,&rc,DT_CALCRECT);
	SelectObject(hdc,oldfont);
	return rc.right-rc.left+2*(rc.bottom-rc.top);
}
void GetPath(LPWSTR Dst,LPCWSTR Src)
{
	WCHAR szShortPath[1024],szLongPath[1024];
	wsprintfW(szShortPath,L"\\\\?\\%s",Src);
	memset(szLongPath,0,2048);
	GetLongPathNameW(szShortPath,szLongPath,1024);
	if(StrStrW(szLongPath,L"\\\\?\\") != szLongPath || lstrlenW(szLongPath) <= 4)
	{
		if(lstrlenW(szLongPath) <= 0)
		{
			lstrcpyW(Dst,Src);
		}
		else
		{
			lstrcpyW(Dst,szLongPath);
		}
	}
	else
	{
		lstrcpyW(Dst,szLongPath+4);
	}
}

void ListViewInsertColumn(HWND hWnd,LPSTR szText,int cx,int iCol)
{//iCol:Index of the new column
	LVCOLUMN col;//LVCOLUMNA
	memset(&col,0,sizeof(LVCOLUMN));
	col.cx=;
	col.pszText=szText;
	col.mask=LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	SendMessage(hWnd,LVM_INSERTCOLUMN,(WPARAM)iCol,(LPARAM)&col);
}

void SetWindowApparenceFromFile(HWND hWnd)
{
	HANDLE hfile=CreateFileW(GlobalFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hfile == INVALID_HANDLE_VALUE) return;

	WINDOWPLACEMENT place;
	DWORD NumofBytesRead;
	ReadFile(hfile,&place,sizeof(WINDOWPLACEMENT),&NumofBytesRead,NULL);

	int cx;
	for(int iCol=0;iCol<5;iCol++)
	{
		ReadFile(hfile,&cx,sizeof(int),&NumofBytesRead,NULL);
		SendMessage(ListCtrlhWnd,LVM_SETCOLUMNWIDTH,(LPARAM)iCol,MAKELPARAM(cx,0));
	}
	CloseHandle(hfile);
	SetWindowPlacement(hWnd,&place);
}

void WriteWindowApparenceToFile(HWND hDlg,INT_PTR nResult)
{
	WINDOWPLACEMENT wndpl;
	wndpl.length=sizeof(WINDOWPLACEMENT);
	if(GetWindowPlacement(hDlg,&wndpl))
	{
		HANDLE hfile=CreateFileW(GlobalFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hfile != INVALID_HANDLE_VALUE)
		{
			DWORD NumberOfBytesWritten;
			WriteFile(hfile,&wndpl,sizeof(WINDOWPLACEMENT),&NumberOfBytesWritten,NULL);
			int cx;
			for(int iCol=0;iCol<5;iCol++)
			{
				cx=SendMessage(ListCtrlhWnd,LVM_GETCOLUMNWIDTH,(WPARAM)iCol,0);
				WriteFile(hfile,&cx,sizeof(int),&NumberOfBytesWritten,NULL);
			}
			CloseHandle(hfile);
		}
	}
	EndDialog(hDlg,nResult);
}

RECT* MakeRect1(RECT* DstRect,HWND hWnd,POINT Point,RECT Rect)
{
	RECT rt;
	GetWindowRect(hWnd,&rt);
	DstRect->left=rt.left-Point.x;
	DstRect->top=rt.top-Point.y;
	DstRect->right=rt.left+Rect.right-rt.right-Rect.left;
	DstRect->bottom=rt.top+Rect.bottom-rt.bottom-Rect.top;
	return DstRect;
}

RECT* MakeRect2(RECT* DstRect,HWND hWnd,POINT Point,RECT Rect)
{
	RECT rt;
	GetWindowRect(hWnd,&rt);
	DstRect->left=rt.left-Point.x;
	DstRect->top=rt.top+Rect.top-Rect.bottom-Point.y;
	DstRect->right=rt.right-rt.left;
	DstRect->bottom=rt.bottom-rt.top;
	return DstRect;
}

RECT* MakeRect3(RECT* DstRect,HWND hWnd,POINT Point,RECT Rect)
{
	RECT rt;
	GetWindowRect(hWnd,&rt);
	DstRect->left=rt.left+Rect.left-Rect.right-Point.x;
	DstRect->top=rt.top+Rect.top-Rect.bottom-Point.y;
	DstRect->right=rt.right-rt.left;
	DstRect->bottom=rt.bottom-rt.top;
	return DstRect;
}

int CALLBACK RenameDialogFunc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				HICON icon=LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(106));//ICON彿坿。。。。。。。。。。。。。。。
				SendMessage(hWnd,WM_SETICON,ICON_SMALL,(LPARAM)icon);
				if(icon) DestroyIcon(icon);
				SetDlgItemText(hWnd,1,GlobalStrings[30]);//彿坿。。。。。。。。。。。。。。。。。。。。。。
				SetDlgItemText(hWnd,2,GlobalStrings[31]);//彿坿。。。。。。。。。。。。。。。。。。。。。。
				SetDlgItemTextW(hWnd,1016,EditString);//彿坿。。。。。。。。。。。。。。。。。。。。。。
				SetWindowText(hWnd,GlobalStrings[25]);
				PostMessage(hWnd,WM_UPDATEUISTATE,MAKEWPARAM(UIS_CLEAR,UISF_HIDEFOCUS),NULL);			
			}
			break;

		case WM_COMMAND:
			{
				switch(LOWORD(wParam))
				{
					case 1://彿坿。。。。。。。。。。。。。。。。。。。。。。
						GetDlgItemText(hWnd,1016,EditString,1024);//彿坿。。。。。。。。。。。。。。。。。。。。。。
						EndDialog(hWnd,1);
						break;

					case 2://彿坿。。。。。。。。。。。。。。。。。。。。。。
						EndDialog(hWnd,2);
						break;

					default:
						return 0;
				}
			}
			break;

		default:
			return 0;

	}
	return 1;
}

int CALLBACK BrowseCallbackProc(HWND hWnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	if(uMsg == BFFM_INITIALIZED)
		SendMessage(hWnd,BFFM_SETSELECTION,TRUE,(LPARAM)ADsPath);
	return 0;
}

void ChooseFolderForMovingObject()
{
	IMalloc* pMalloc;
	BROWSEINFOW bi;
	if(!SHGetMalloc(&pMalloc))
	{
		WCHAR ChooseDirectory[1024];
		CoInitialize(NULL);
		memset(&bi,0,sizeof(BROWSEINFOW));
		bi.pszDisplayName=szPath;
		MultiByteToWideChar(CP_ACP,0,GlobalStrings[24],-1,ChooseDirectory,1024);//"Choose destination folder for moved object"
		bi.lpszTitle=ChooseDirectory;
		bi.ulFlags=BIF_NEWDIALOGSTYLE;
		bi.lpfn=BrowseCallbackProc;
		LPITEMIDLIST item=SHBrowseForFolderW(&bi);
		if(item)
		{
			SHGetPathFromIDListW(item,szPath);
			pMalloc->lpVtbl->Free(pMalloc,item);
		}
		CoUninitialize();
		pMalloc->lpVtbl->Release(pMalloc);
	}
}

LPWSTR MyReadFileW(LPWSTR Buffer,int NumberOfBytesRead,HANDLE hFile)
{
	int i=0;
	LPWSTR nRet;
	while(NumberOfBytesRead-1 > i)
	{
		DWORD curRead;
		ReadFile(hFile,Buffer,sizeof(WCHAR),&curRead,NULL);
		if(curRead < 2 || Buffer[i] == L'\0' || Buffer[i] == L'\n') 
			break;
		if(Buffer[i] != L'\r')
		{
			i++;
		}
	}
	
	if(i>0)
	{
		Buffer[i]=L'\0';
		nRet=Buffer;
	}
	else
	{
		nRet=NULL;
	}
	return nRet;
}

LPSTR MyReadFileA(LPSTR Buffer,int NumberOfBytesRead,HANDLE hFile)
{
	int i=0;
	LPSTR nRet;
	while(NumberOfBytesRead-1 > i)
	{
		DWORD curRead;
		ReadFile(hFile,Buffer,sizeof(TCHAR),&curRead,NULL);
		if(curRead < 2 || Buffer[i] == '\0' || Buffer[i] == '\n') 
			break;
		if(Buffer[i] != '\r')
		{
			i++;
		}
	}
	
	if(i>0)
	{
		Buffer[i]='\0';
		nRet=Buffer;
	}
	else
	{
		nRet=NULL;
	}
	return nRet;
}

//Start Initial Begin
#define Global_Begin	Global_Func;
#define Global_End		Global_Func+sizeof(Global_Func)
#define Initial_Begin	InitialFuncArray
#define Initial_End		InitialFuncArray+InitialFuncNum
void FuncFunctionArray(GlobalFunc* FuncAddrBegin,GlobalFunc* FuncAddrEnd)
{
	for(GlobalFunc* curfunc=FuncAddrBegin;curfunc<FuncAddrEnd;curfunc++)
	{
		if(*curfunc) (*curfunc)();
	}
}
void GlobalInitial(GlobalFunc* Cur)
{
	InitialFuncArray[InitialFuncNum++]=Cur;
}
void begin()
{
	FuncFunctionArray(Global_Begin,Global_End);
}
void end()
{

	FuncFunctionArray(Initial_Begin,Global_End);
}
//Start Initial End

LRESULT CALLBACK ProgressDialogProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				INITCOMMONCONTROLSEX picce;
				picce.dwSize=sizeof(INITCOMMONCONTROLSEX);
				picce.dwICC=ICC_PROGRESS_CLASS;
				InitCommonControlsEx(&picce);
				HICON hicon=LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(106));
				SendMessage(hWnd,WM_SETICON,ICON_SMALL,(LPARAM)hicon);
				if(hicon) DestroyIcon(hicon);
			}

		default:
			break;
	}
	return 0;
}

bool GetPrivilege(ULONG Privilege)
{
	BOOLEAN Enabled;
	return GetMyCoreAPIArray()->RtlAdjustPrivilege(Privilege,TRUE,FALSE,&Enabled) == FALSE;
}

bool SeniorDeleteFile(LPWSTR szPath)
{
	WCHAR Path[1024];
	UNICODE_STRING usPath;
	wsprintfW(Path,L"%s\\%s",ssPath,PathSkipRootW(szPath));
	GetMyCoreAPIArray()->RtlInitUnicodeString(&usPath,Path);
	OBJECT_ATTRIBUTES oa;
	oa.Length=sizeof(OBJECT_ATTRIBUTES);
	oa.RootDirectory=NULL;
	oa.ObjectName=&usPath;
	oa.Attributes=OBJ_CASE_INSENSITIVE;
	oa.SecurityDescriptor=NULL;
	oa.SecurityQualityOfService=NULL;
	return GetMyCoreAPIArray()->ZwDeleteFile(&oa) == STATUS_SUCCESS;
}

bool JuniorDeleteFile(LPWSTR FileName,bool flag)
{
	DWORD attribbutes=GetFileAttributesW(FileName);
	if(attribbutes != INVALID_FILE_ATTRIBUTES)
	{
		attribbutes &= ~FILE_ATTRIBUTE_READONLY;
		SetFileAttributesW(FileName,attribbutes);
	}

	WCHAR OperateFrom[1024];
	SHFILEOPSTRUCTW FileOp;
	memset(&FileOp,0,sizeof(SHFILEOPSTRUCTW));
	wsprintfW(OperateFrom,L"%s",FileName);
	OperateFrom[lstrlenW(OperateFrom)+1]=L'\0';
	FileOp.wFunc=FO_DELETE;
	FileOp.pFrom=OperateFrom;
	FileOp.pTo=L"\0";
	FileOp.fFlags=FOF_NOERRORUI | FOF_ALLOWUNDO | FOF_NOCONFIRMATION;
	
	if(attribbutes & FILE_ATTRIBUTE_DIRECTORY)
	{
		if((flag && !SHFileOperationW(&FileOp)) || RemoveDirectoryW(FileName))
			return TRUE;
	}
	else
	{
		if((flag && !SHFileOperationW(&FileOp)) || DeleteFileW(FileName))
			return TRUE;
	}
	return FALSE;
}

bool MyDeleteFileSubFunc(LPWSTR FileName,bool flag)
{
	if(!JuniorDeleteFile(FileName,flag) && !(GetFileAttributesW(FileName) & FILE_ATTRIBUTE_REPARSE_POINT))
	{//if Junior fail
		if(SeniorDeleteFile(FileName))//if Senior Success
			return TRUE;

		WCHAR ObjName[1024],szLongPath[1024];
		wsprintfW(ObjName,L"\\\\?\\%s",FileName);
		if(JuniorDeleteFile(ObjName,flag))//if Junior Success
			return TRUE;

		lstrcpyW(szLongPath,ObjName);
		GetShortPathName(szLongPath,ObjName,1024);
		return JuniorDeleteFile(ObjName,flag);
	}
	return TRUE;
}

bool MyDeleteFile(LPWSTR ObjName,bool flag)
{
	bool nRet=MyDeleteFileSubFunc(ObjName,flag);
	if(!nRet && SetACL(ObjName))
		nRet=MyDeleteFileSubFunc(ObjName,flag);
	return nRet;
}

bool MoveFileJunior(LPWSTR ExistingFileName,LPWSTR Path,LPWSTR SrcString)
{
	SHFILEOPSTRUCTW FileOp;
	WCHAR StringFrom[1024],StringTo[1024],NewFileName[1024];
	memset(&FileOp,0,sizeof(SHFILEOPSTRUCTW));
	wsprintfW(StringFrom,L"%s",ExistingFileName);
	StringFrom[lstrlenW(StringFrom)+1]=L'\0';
	wsprintfW(StringTo,L"%s",Path);
	StringTo[lstrlenW(StringTo)+1]=L'\0';
	wsprintfW(NewFileName,L"%s\\%s",Path,SrcString);
	FileOp.pFrom=StringFrom;
	FileOp.pTo=StringTo;
	FileOp.wFunc=FO_MOVE;
	FileOp.fFlags=FOF_NOERRORUI | FOF_ALLOWUNDO | FOF_NOCONFIRMATION;
	return !SHFileOperationW(&FileOp) || MoveFileW(ExistingFileName,NewFileName);
}

bool MyMoveFileSubFunc(LPWSTR ExistingFileName,LPWSTR Path,LPWSTR FileName)
{
	if(!MoveFileJunior(ExistingFileName,Path,FileName))
	{
		WCHAR ObjName[1024],szLongPath[1024],szShortPath[1024];
		wsprintfW(ObjName,L"\\\\?\\%s",ExistingFileName);
		wsprintfW(szShortPath,L"\\\\?\\%s",Path);
		if(MoveFileJunior(ObjName,szShortPath,FileName))
			return TRUE;

		lstrcpyW(szLongPath,ObjName);
		GetShortPathNameW(szLongPath,ObjName,1024);
		lstrcpyW(szLongPath,szShortPath);
		GetShortPathNameW(szLongPath,szShortPath,1024);
		return MoveFileJunior(ObjName,szShortPath,FileName);
	}
	return TRUE;
}

bool MyMoveFile(LPWSTR ObjName,LPWSTR Path,LPWSTR FileName)
{
	bool nRet=MyMoveFileSubFunc(ObjName,Path,FileName);
	if(!nRet && SetACL(ObjName))
		nRet=MyMoveFileSubFunc(ObjName,Path,FileName);
	return nRet;
}

int GetWindowsVersion()
{
	OSVERSIONINFO VersionInformation;
	memset(&VersionInformation,0,sizeof(OSVERSIONINFO));
	VersionInformation.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	if(!GetVersionEx(&VersionInformation))
		return -1;

	switch(MAKELONG(VersionInformation.dwMajorVersion,VersionInformation.dwMinorVersion))
	{
		case 0x00004://Windows 95
			return WINDOWS95;

		case 0x00005://Windows 2000
			return WINDOWS2000;

		case 0x00006://Windows Vista or Windows Server 2008
			return WINDOWSVISTA;

		case 0x10005://Windows XP
		case 0x10006://Windows 7 or Windows Server 2008 R2
		case 0x20005://Windows Server 2003
			return WINDOWSXPLATER;

		default:
			break;
	}
	return -1;

}

bool ReNameFileJunior(LPWSTR OldName,LPWSTR NewName)
{
	WCHAR StringFrom[1024],StringTo[1024],szLongPath[1024];
	SHFILEOPSTRUCTW FileOp;
	memset(&FileOp,0,sizeof(SHFILEOPSTRUCTW));
	wsprintfW(StringFrom,L"%s",OldName);
	wsprintfW(StringTo,L"%s",NewName);
	StringFrom[lstrlenW(StringFrom)+1]=L'\0';
	StringTo[lstrlenW(StringTo)+1]=L'\0';
	FileOp.pFrom=StringFrom;
	FileOp.pTo=StringTo;
	FileOp.wFunc=FO_RENAME;
	FileOp.fFlags=FOF_NOERRORUI | FOF_ALLOWUNDO | FOF_NOCONFIRMATION;
	if(!SHFileOperationW(&FileOp)) return TRUE;

	wsprintfW(StringFrom,L"\\\\?\\%s",OldName);
	wsprintfW(StringTo,L"\\\\?\\%s\\%s",NewName);
	StringFrom[lstrlenW(StringFrom)+1]=L'\0';
	StringTo[lstrlenW(StringTo)+1]=L'\0';
	if(!SHFileOperationW(&FileOp)) return TRUE;

	lstrcpyW(szLongPath,StringFrom);
	GetShortPathNameW(szLongPath,StringFrom,1024);
	lstrcpyW(szLongPath,StringTo);
	GetShortPathNameW(szLongPath,StringTo,1024);
	FileOp.fFlags &= ~FOF_NOERRORUI;
	StringFrom[lstrlenW(StringFrom)+1]=L'\0';
	StringTo[lstrlenW(StringTo)+1]=L'\0';
	return !SHFileOperationW(&FileOp);
}

bool MyRenameFile(LPWSTR OldName,LPWSTR NewName)
{
	bool nRet=ReNameFileJunior(OldName,NewName);
	if(!nRet && SetACL(OldName))
		nRet=ReNameFileJunior(OldName,NewName);
	return nRet;
}

//Func1Inject Begin
DWORD WINAPI Func1ToInject(InjectStruct1* INJECT)
{
	HMODULE hmod=INJECT->MyGetModuleHanle(INJECT->UData);
	if(hmod)
	{
		GlobalFunc func=(GlobalFunc)INJECT->MyGetProcAddress(hmod,INJECT->AData);
		if(func) func();
		PPEB PEB;
		LIST_ENTRY* Flink=NULL;
		_asm
		{
			mov eax,fs:[0x30]
			mov PEB,eax
		}

		LIST_ENTRY* origin=PEB->LoaderData->InInitializationOrderModuleList.Flink;
		LIST_ENTRY* list=origin;
		int LoadCount=16;
		do 
		{
			LDR_MODULE* lm=((LDR_MODULE*)((DWORD)list-2*sizeof(LIST_ENTRY)));//功象LDR_MODULE嚥PEB_LDR_DATA議購狼
			if(hmod == lm->BaseAddress)
				LoadCount=lm->LoadCount;
			list=list->Flink;
		} 
		while (list != origin);
		while(LoadCount>0)
		{
			if(hmod)
			{
				INJECT->MyFreeLibrary(hmod);
				INJECT->MyCloseHandle(hmod);
				hmod=INJECT->MyGetModuleHanle(INJECT->UData);
			}
			LoadCount--;
		}
		if(hmod)
			INJECT->MyCloseHandle(hmod);
	}
	return 0;
}
void Func1End(void)
{
}
//Func1Inject End

//Func2Inject Begin
DWORD WINAPI Func2ToInject(InjectStruct1* INJECT)
{
	HANDLE file1=INJECT->MyCreateFile(INJECT->Data,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
	if(file1 != INVALID_HANDLE_VALUE)
	{
		OVERLAPPED Overlapped;
		memset(&Overlapped,0,sizeof(OVERLAPPED));
		LPVOID buf=INJECT->MyGlobalAlloc(GMEM_FIXED,1024);
		bool flag=TRUE;
		while(flag)
		{
			DWORD NumberOfBytesRead=0,NumberOfBytesWritten=0;
			if(!INJECT->MyReadFile(INJECT->handle,buf,1024,&NumberOfBytesRead,&Overlapped))
			{
				DWORD nRet=INJECT->MyGetLastError();
				if(nRet == ERROR_HANDLE_EOF) flag=FALSE;
				else if(nRet == ERROR_IO_PENDING)
				{
					INJECT->MySleep(1000);
					if(!INJECT->MyGetOverlappedResult(INJECT->handle,&Overlapped,&NumberOfBytesRead,TRUE) && 
							INJECT->MyGetLastError() == ERROR_IO_PENDING)
						flag=FALSE;
				}
			}
			if(NumberOfBytesRead)
			{
				Overlapped.Offset += NumberOfBytesRead;
				INJECT->MyWriteFile(file1,buf,NumberOfBytesRead,&NumberOfBytesWritten,NULL);
			}
		}
		INJECT->MyGlobalFree(buf);
		INJECT->MyCloseHandle(file1);
	}
	
	return 0;
}
void Func2End(void)
{

}
//Func2Inject End

void ListViewAddItems(HWND hWnd,int index)
{
	LVITEMW item;
	WCHAR str[256];
	memset(&item,0,sizeof(LVITEMW));
	item.iSubItem=0;
	item.iItem=index;
	item.iImage=index;
	item.lParam=index;
	item.pszText=str;
	item.mask=LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	item.cchTextMax=256;
	wsprintfW(str,L"%s",ListProcessGeneral.Buf[index].ProcessNameString);
	SendMessage(hWnd,LVM_INSERTITEMW,0,(LPARAM)&item);
	item.mask=LVIF_TEXT;
	item.iSubItem=1;
	wsprintfW(str,L"%s",ListProcessGeneral.Buf[index].PathLockedString);
	SendMessage(hWnd,LVM_SETITEMW,0,(LPARAM)&item);
	item.iSubItem=2;
	wsprintfW(str,L"%d",ListProcessGeneral.Buf[index].ProcessId);
	SendMessage(hWnd,LVM_SETITEMW,0,(LPARAM)&item);
	item.iSubItem=3;
	WORD id=ListProcessGeneral.Buf[index].HandleId;
	if(id == INVALID_HANDLE_VALUE)
		wsprintfW(str,L"DLL");
	else
		wsprintfW(str,L"%d",id);
	SendMessage(hWnd,LVM_SETITEMW,0,(LPARAM)&item);
	item.iSubItem=4;
	wsprintfW(str,L"%s",ListProcessGeneral.Buf[index].ProcessPathString);
	SendMessage(hWnd,LVM_SETITEMW,0,(LPARAM)&item);
}

int CALLBACK CompareFunc(LPARAM lParam1,LPARAM lParam2,int SubitemIndex)
{
	int result;
	switch(SubitemIndex)//SubitemIndex彿坿。。。。。。。。。。。。。。。
	{
		case 0:
			result=lstrcmpiW(ListProcessGeneral.Buf[lParam1].ProcessNameString,ListProcessGeneral.Buf[lParam2].ProcessNameString);
			break;

		case 1:
			result=lstrcmpiW(ListProcessGeneral.Buf[lParam1].PathLockedString,ListProcessGeneral.Buf[lParam2].PathLockedString);
			break;

		case 2:
			result=ListProcessGeneral.Buf[lParam1].ProcessId - ListProcessGeneral.Buf[lParam2].ProcessId;
			break;

		case 3:
			result=ListProcessGeneral.Buf[lParam1].HandleId - ListProcessGeneral.Buf[lParam2].HandleId;
			break;

		case 4:
			result=lstrcmpiW(ListProcessGeneral.Buf[lParam1].ProcessPathString,ListProcessGeneral.Buf[lParam2].ProcessPathString);
			break;
			
		default:
			return 0;	
	}
	if(ListSortDirection[SubitemIndex])
		result = -result;
	return result;
}

bool FindFile(LPCWSTR String,GeneralClass<LPWSTR>* StringGeneral)
{
	LPWSTR WptrArray=(LPWSTR)GlobalAlloc(GMEM_ZEROINIT,1024*sizeof(WCHAR));
	lstrcpyW(WptrArray,String);
	StringGeneral.Construct(&WptrArray);

	DWORD i=0;
	WCHAR FileName[1024];
	WIN32_FIND_DATAW FindFileData;
	bool flag=FALSE;
	while(i < StringGeneral->CurPos)
	{
		wsprintfW(FileName,L"%s\\*",StringGeneral->Buf[i]);
		if(!(GetFileAttributesW(StringGeneral->Buf[i]) & FILE_ATTRIBUTE_REPARSE_POINT))
		{
			HANDLE hFindFile=FindFirstFileW(FileName,&FindFileData);
			if(hFindFile != INVALID_HANDLE_VALUE)
			{
				do 
				{
					if(lstrcmpW(FindFileData.cFileName,L".") && lstrcmpW(FindFileData.cFileName,L".."))
					{
						LPWSTR curwptr=(LPWSTR)GlobalAlloc(GMEM_ZEROINIT,1024*sizeof(WCHAR));
						wsprintfW(curwptr,L"%s\\%s",StringGeneral->Buf[i],FindFileData.cFileName);
						if(!flag && lstrlenW(curwptr)>256)
							flag=TRUE;
						StringGeneral->Construct(&curwptr);
					}
				} 
				while (FindNextFileW(hFindFile,&FindFileData));
				FindClose(hFindFile);
			}
		}
		i++;
	}
	return flag;
}

bool ProgressFunc(LPCWSTR String,bool flagin)
{
	GeneralClass<LPWSTR> CurFind;
	bool IsLongPath=FindFile(String,&CurFind);
	HWND hWnd=NULL;
	if(!GetMyOperateFile()->IsNotShowGUItoUnlock)
	{
		hWnd=CreateDialogParam(GetModuleHandle(NULL),MAKEINTRESOURCE(110),NULL,(DLGPROC)ProgressDialogProc,0);//彿坿。。。。。。。。。。
		SendMessage(GetDlgItem(hWnd,1017),PBM_SETRANGE32,0,(LPARAM)CurFind.CurPos);//彿坿。。。。。。。。。。
		ShowWindow(hWnd,SW_SHOW);
	}
	bool flagout=TRUE;
	int i=CurFind.CurPos;
	
	while(i)
	{
		bool success=MyDeleteFile(CurFind.Buf[i-1],flagin & !IsLongPath);
		if(IsLongPath && GetFileAttributesW(CurFind.Buf[i-1]) != INVALID_FILE_ATTRIBUTES)
		{
			wsprintfW(szPath, L"C:\\TmpDelete\\%d\\",tmpnum++);
			SHCreateDirectoryExW(NULL,szPath,NULL);
			MyMoveFile(CurFind.Buf[i-1],szPath,L"\0");
		}
		flagout &= success;
		if(CurFind.Buf[i-1]) 
			GlobalFree((HGLOBAL)CurFind.Buf[i-1]);
		if(hWnd && !(i&7))//i葎8議蔚方
		{
			MSG Msg;
			SendMessage(GetDlgItem(hWnd,1017),PBM_SETPOS,WPARAM(CurFind.CurPos-i),0);//彿坿。。。。。。。。。。
			while(PeekMessage(&Msg,hWnd,0,0,PM_REMOVE))
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
		}
		i--;
	}

	if(hWnd) 
		DestroyWindow(hWnd);
	CurFind.Destruct();
	return flagout;
}

void OutputLog(HANDLE hFile,ProcessInfo* process)
{
	if(hFile)
	{
		TCHAR String[1024];
		DWORD NumberOfBytesWritten;
		wsprintf(String,"Handle %d found:\n",ListProcessGeneral.CurPos);
		WriteFile(hFile,String,lstrlen(String),&NumberOfBytesWritten,NULL);
		wsprintf(String,"Path locked: %S\n",process->PathLockedString);
		WriteFile(hFile,String,lstrlen(String),&NumberOfBytesWritten,NULL);
		wsprintf(String,"PID: %d / Handle: %d / Process Name: %S\n",process->ProcessId,process->HandleId,process->ProcessNameString);
		WriteFile(hFile,String,lstrlen(String),&NumberOfBytesWritten,NULL);
		wsprintf(String,"Process Path: %S\n\n",process->ProcessPathString);
		WriteFile(hFile,String,lstrlen(String),&NumberOfBytesWritten,NULL);
	}
}

BOOL ReadFileToMemory(LPWSTR filename,GeneralClass<WCHAR [4][1024]>* general)
{
	HANDLE hfile=CreateFileW(filename,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hfile == INVALID_HANDLE_VALUE) return FALSE;
	
	WCHAR WideCharStr[4][1024];
	char MultiByteStr[1024];
	NULLString(WideCharStr);

	if(GetMyOperateFile()->IsFiletoUnlockUnicode)
	{
		while(MyReadFileW(WideCharStr[0],1024,hfile))
		{
			for(int i=0;i<lstrlenW(WideCharStr[0]);i++)//丼楕詰。。。
			{
				if(WideCharStr[0][i] == '\n')
				{
					WideCharStr[0][i]=L'\0';
					break;
				}
			}
			if(lstrlenW(WideCharStr[0]))
			{
				general->Construct(WideCharStr);
			}
		}
	}
	else
	{
		while(MyReadFileA(MultiByteStr[0],1024,hfile))
		{
			for(int j=0;j<lstrlen(MultiByteStr);j++)
			{
				if(MultiByteStr[j] == '\n')
				{
					MultiByteStr[j]='\0';
					break;
				}
			}
			if(lstrlen(MultiByteStr))
			{
				MultiByteToWideChar(CP_ACP,0,MultiByteStr,-1,WideCharStr);
				general->Construct(WideCharStr);
			}
		}
	}

	CloseHandle(hfile);
	return 1;
}

void ProcessRequestByteOperationType(int Operation)
{
	switch (Operation)
	{
		case FILEDELETE:
			{
				int i=0,ret=IDYES;
				bool flag1=TRUE,flag2=TRUE;
				while(i < StringGeneral1.CurPos)
				{
					bool success=ProgressFunc(StringGeneral1.Buf[i][0],TRUE);
					int j=0;
					while(j <= 8)
					{
						if(GetFileAttributesW(L"C:\\TmpDelete") == INVALID_FILE_ATTRIBUTES)
							break;
						JuniorDeleteFile(L"C:\\TmpDelete",FALSE);
						ProgressFunc(L"C:\\TmpDelete",NULL);
						JuniorDeleteFile(L"C:\\TmpDelete",FALSE);
						j++;
					}
					flag1 &= success;
					if(!success)
					{
						char String[1024];
						wsprintf(String, "%s\n\n%s\n",GlobalStrings[10],GlobalStrings[34]);
						if(flag2)
						{
							if(!GetMyOperateFile()->IsNotShowGUItoUnlock)
								ret=MessageBox(NULL,String,GlobalStrings[11],MB_YESNO);
							flag2=FALSE;
						}
						if(ret == IDYES)
						{
							GeneralClass<LPWSTR> cur;
							FindFile(StringGeneral1.Buf[i],&cur);
							j=cur.CurPos;
							while(j)
							{
								MoveFileExW(cur.Buf[j-1],NULL,MOVEFILE_DELAY_UNTIL_REBOOT);//嶷尼評茅
								if(cur.Buf[j-1]) 
									GlobalFree((HGLOBAL)cur.Buf[j-1]);
								j--;
							}
							cur.Destruct();
						}
					}
					i++;
				}
				if(flag1 && !GetMyOperateFile()->IsNotShowGUItoUnlock)
				{
					MessageBox(NULL,GlobalStrings[12],GlobalStrings[13],MB_OK);
					return ;
				}
			}
			break;

		case FILERENAME:
			{
				if(lstrlenW(EditString) || GetMyOperateFile()->IsNotShowGUItoUnlock)
				{
					WCHAR NewName[1024];
					if(GetMyOperateFile()->IsNotShowGUItoUnlock)
						wsprintfW(NewName,L"%s",(LPWSTR)StringGeneral2.Buf);
					else
						wsprintf(NewName,L"%s\\%s",ADsPath,EditString);
					if(MyRenameFile((LPWSTR)StringGeneral2.Buf,NewName))
					{
						if(GetMyOperateFile()->IsNotShowGUItoUnlock)
							return;
						MessageBox(NULL,GlobalStrings[26],GlobalStrings[13],MB_OK);
					}

					TCHAR String[1024];
					wsprintf(String,"%s\n\n%s\n",GlobalStrings[27],GlobalStrings[35]);
					if(GetMyOperateFile()->IsNotShowGUItoUnlock)
					{
						GeneralClass<LPWSTR> cur;
						FindFile((LPWSTR)StringGeneral1.Buf,&cur);
						int i=cur.CurPos;
						while(i)
						{
							LPWSTR findpos=StrStrW(cur.Buf[i-1],(LPWSTR)StringGeneral1.Buf);
							if(findpos)
							{
								WCHAR WString[1024];
								if(i == 1)
								{
									wsprintfW(WString,L"%s\\%s",ADsPath,EditString);
								}
								else
								{
									wsprintfW(WString,L"%s\\%s\\%s",ADsPath,EditString,findpos+lstrlenW((LPWSTR)StringGeneral1.Buf));
								}
								MoveFileExW(cur.Buf[i-1],WString,MOVEFILE_DELAY_UNTIL_REBOOT);
							}
							if(cur.Buf[i-1])
								GlobalFree((HGLOBAL)cur.Buf[i-1]);
							i--;
						}
						cur.Destruct();
					}
				}
			}
			break;

		case FILEMOVE:
			{
				if(lstrlenW(szPath) || GetMyOperateFile()->IsNotShowGUItoUnlock)
				{
					int i=0,ret=IDYES;
					bool flag1=TRUE,flag2=TRUE;
					WCHAR String[1024];
					while(i < StringGeneral1.CurPos)
					{
						LPWSTR ptr;
						MergeString(String,StringGeneral1.Buf[i][1],StringGeneral1.Buf[i][3]);
						if(GetMyOperateFile()->IsNotShowGUItoUnlock)
							ptr=(LPWSTR)StringGeneral2.Buf[i];
						else
							ptr=szPath;
						bool success=MyMoveFile((LPWSTR)StringGeneral1.Buf[i],ptr,String);

						flag1 &= success;
						if(!success)
						{
							TCHAR AString[1024];
							wsprintf(AString, "%s\n\n%s\n",GlobalStrings[29],GlobalStrings[36]);
							if(flag2)
							{
								if(!GetMyOperateFile()->IsNotShowGUItoUnlock)
									ret=MessageBox(NULL,AString,GlobalStrings[11],MB_YESNO);
								flag2=FALSE;
							}
							if(ret == IDYES)
							{
								GeneralClass<LPWSTR> cur;
								FindFile(StringGeneral1.Buf[i],&cur);
								int j=cur.CurPos;
								while(j)
								{
									LPWSTR findpos=StrStrW(cur.Buf[i-1],(LPWSTR)StringGeneral1.Buf[i]);
									if(findpos)
									{
										WCHAR NewName[1024],WString[1024];
										memset(NewName,0,2048);
										MergeString(NewName,StringGeneral1.Buf[i][1],StringGeneral1.Buf[i][2]);
										if(i == 1)
										{
											wsprintfW(WString,L"%s\\%s",ptr,NewName);
										}
										else
										{
											wsprintfW(WString,L"%s\\%s\\%s",ptr,NewName,findpos+lstrlenW((LPWSTR)StringGeneral1.Buf[i]));
										}
										MoveFileExW(cur.Buf[i-1],WString,MOVEFILE_DELAY_UNTIL_REBOOT);
									}
									if(cur.Buf[i-1])
										GlobalFree((HGLOBAL)cur.Buf[i-1]);
									i--;
								}
								cur.Destruct();
							}
						}
						i++;
					}
					if(flag1 && !GetMyOperateFile()->IsNotShowGUItoUnlock)
					{
						MessageBox(NULL,GlobalStrings[28],GlobalStrings[13],MB_OK);
						return ;
					}
				}
			}
			break;

		default:
			break;
	}
	return;
}


//RemoteInject Func Begin
void RemoteInject2(LPCWSTR String,DWORD dwProcessId,HANDLE hFile)
{
	HMODULE hmod=GetModuleHandle("kernel32");
	if(hmod)
	{
		HANDLE hRemoteProcess=OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_VM_OPERATION | 
											PROCESS_CREATE_THREAD,FALSE,dwProcessId);
		if(hRemoteProcess)
		{
			DWORD len=(int)Func2End-(int)Func2ToInject;
			LPVOID pfnStartAddr=VirtualAllocEx(hRemoteProcess,NULL,len,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
			if(pfnStartAddr)
			{
				DWORD NumberOfBytesWritten=0;
				WriteProcessMemory(hRemoteProcess,pfnStartAddr,(LPVOID)Func2ToInject,len,&NumberOfBytesWritten);
				LPVOID ParamBuffer=VirtualAllocEx(hRemoteProcess,NULL,sizeof(InjectStruct2),MEM_COMMIT,PAGE_EXECUTE_READWRITE);
				if(ParamBuffer)
				{
					InjectStruct2 RemoteFunc;
					lstrcpyW(RemoteFunc.Data,String);
					RemoteFunc.handle=hFile;
					RemoteFunc.MyCreateFile=(MYCREATEFILE)GetProcAddress(hmod,"CreateFileW");
					RemoteFunc.MySetFilePointer=(MYSETFILEPOINTER)GetProcAddress(hmod,"SetFilePointer");
					RemoteFunc.MyReadFile=(MYREADFILE)GetProcAddress(hmod,"ReadFile");
					RemoteFunc.MyWriteFile=(MYWRITEFILE)GetProcAddress(hmod,"WriteFile");
					RemoteFunc.MyCloseHandle=(MYCLOSEHANDLE)GetProcAddress(hmod,"CloseHandle");
					RemoteFunc.MyGlobalAlloc=(MYGLOBALALLOC)GetProcAddress(hmod,"GlobalAlloc");
					RemoteFunc.MyGlobalFree=(MYGLOBALFREE)GetProcAddress(hmod,"GlobalFree");
					RemoteFunc.MyGetFileSize=(MYGETFILESIZE)GetProcAddress(hmod,"GetFileSize");
					RemoteFunc.MySleep=(MYSLEEP)GetProcAddress(hmod,"Sleep");
					RemoteFunc.MyGetOverlappedResult=(MYGETOVERLAPPEDRESULT)GetProcAddress(hmod,"GetOverlappedResult");
					RemoteFunc.MyGetLastError=(MYGETLASTERROR)GetProcAddress(hmod,"GetLastError");
					WriteProcessMemory(hRemoteProcess,ParamBuffer,(LPVOID)&RemoteFunc,sizeof(InjectStruct2),&NumberOfBytesWritten);
					HANDLE NewThread=CreateRemoteThread(hRemoteProcess,NULL,0,pfnStartAddr,ParamBuffer,0,NULL);
					if(NewThread) 
						WaitForSingleObject(NewThread,INFINITE);
					VirtualFreeEx(hRemoteProcess,ParamBuffer,0,MEM_RELEASE);
					CloseHandle(NewThread);
				}
				VirtualFreeEx(hRemoteProcess,pfnStartAddr,0,MEM_RELEASE);
			}
			CloseHandle(hRemoteProcess);
		}
	}
}

void RemoteInject1(SYSTEM_HANDLE_INFORMATION_EX* processdata)
{
	HMODULE hmod=GetModuleHandle("kernel32");
	HANDLE hRemoteProcess;
	WCHAR String[1024];
	int i=0;

	while(i < ListItemGeneral.CurPos)
	{
		ProcessInfo* curProcess=ListProcessGeneral.Buf+ListItemGeneral.Buf[i];
		if(IsToKillProcess) 
		{	
			DWORD id;
			if(curProcess->Index == -1)
				id=curProcess->ProcessId;
			else
				id=processdata->Information[curProcess->Index].ProcessID;
			HANDLE curp=OpenProcess(PROCESS_TERMINATE,FALSE,id);
			if(curp)
			{
				TerminateProcess(curp,0);
				CloseHandle(curp);
			}
		}
		else if(curProcess->Index == -1)
		{
			hRemoteProcess=OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_VM_OPERATION | 
											PROCESS_CREATE_THREAD,FALSE,curProcess->ProcessId);
			if(hRemoteProcess)
			{
				GetModuleFileNameExW(hRemoteProcess,curProcess->hmod,String,1024);
				LPWSTR ptr=PathFindExtension(String);
				DWORD len=(int)Func1End-(int)Func1ToInject;
				if(ptr && !lstrcmpiW(ptr,L".DLL"))
				{
					LPVOID pfnStartAddr=VirtualAllocEx(hRemoteProcess,NULL,len,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
					if(pfnStartAddr)
					{
						DWORD NumberOfBytesWritten=0;
						WriteProcessMemory(hRemoteProcess,pfnStartAddr,(LPVOID)Func1ToInject,len,&NumberOfBytesWritten);
						LPVOID ParamBuffer=VirtualAllocEx(hRemoteProcess,NULL,sizeof(InjectStruct1),MEM_COMMIT,PAGE_EXECUTE_READWRITE);
						if(ParamBuffer)
						{
							InjectStruct1 RemoteFunc;
							if(hmod)
							{
								RemoteFunc.MyFreeLibrary=(MYFREELIBRARY)GetProcAddress(hmod,"FreeLibrary");
								RemoteFunc.MyGetModuleHanle=(MYGETMODULEHANDLE)GetProcAddress(hmod,"GetModuleHandleW");
								RemoteFunc.MyCloseHandle=(MYCLOSEHANDLE)GetProcAddress(hmod,"CloseHandle");
								RemoteFunc.MyGetProcAddress=(MYGETPROCADDRESS)GetProcAddress(hmod,"GetProcAddress");
							}
							lstrcpy(RemoteFunc.AData,"DllUnregisterServer");
							lstrcpy(RemoteFunc.UData,String);
							WriteProcessMemory(hRemoteProcess,ParamBuffer,(LPVOID)&RemoteFunc,sizeof(InjectStruct1),&NumberOfBytesWritten);
							HANDLE NewThread=CreateRemoteThread(hRemoteProcess,NULL,0,pfnStartAddr,ParamBuffer,0,NULL);
							if(NewThread) 
								WaitForSingleObject(NewThread,INFINITE);
							VirtualFreeEx(hRemoteProcess,ParamBuffer,0,MEM_RELEASE);
							CloseHandle(NewThread);
						}
						VirtualFreeEx(hRemoteProcess,pfnStartAddr,0,MEM_RELEASE);
					}
				}
				CloseHandle(curProcess->Index);
			}
		}
		else
		{
			SYSTEM_HANDLE_INFORMATION* cur=&processdata->Information+curProcess->Index;
			HANDLE curp=OpenProcess(PROCESS_DUP_HANDLE,FALSE,cur->ProcessID);
			if(curp)
			{
				HANDLE target;
				if(DuplicateHandle(curp,cur->ProcessID,GetCurrentProcess(),&target,0,FALSE,DUPLICATE_CLOSE_SOURCE))
					CloseHandle(target);
			}
			CloseHandle(curp);
		}
		i++;
	}
}
//RemoteInject Func End

int UnlockProcess(PSYSTEM_HANDLE_INFORMATION_EX* sysinfo,LPWSTR devicepath,int devicestrlen,LPWSTR path)
{
	ULONG ReturnLength;
	NTSTATUS state;
	PSYSTEM_HANDLE_INFORMATION_EX hMem=(PSYSTEM_HANDLE_INFORMATION_EX)GlobalAlloc(GMEM_ZEROINIT,4*ProcessGuage);
	for(state=GetMyCoreAPIArray()->ZwQuerySystemInformation(SystemHandleInformation,(PVOID)hMem,4*ProcessGuage,&ReturnLength); 
			state == STATUS_INFO_LENGTH_MISMATCH;
			state=GetMyCoreAPIArray()->ZwQuerySystemInformation(SystemHandleInformation,(PVOID)hMem,4*ProcessGuage,&ReturnLength))
	{
		ProcessGuage*=2;
		GlobalFree((HGLOBAL)hMem);
		hMem=(PSYSTEM_HANDLE_INFORMATION_EX)GlobalAlloc(GMEM_ZEROINIT,4*ProcessGuage);
	}
	ReturnLength /= 16;
	SYSTEM_HANDLE_INFORMATION* process=&hMem->Information;
	ListItemGeneral.CurPos=0;
	ListProcessGeneral.CurPos=0;
	HANDLE hsnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hsnapshot == INVALID_HANDLE_VALUE)
		return 1;

	PROCESSENTRY32W pe;
	if(!Process32FirstW(hsnapshot,&pe))
	{
		CloseHandle(hsnapshot);
		return 1;
	}
	HANDLE hFile=NULL;
	ProcessSInfo sinfo;
	WCHAR FileName[1024];
	if(GetMyOperateFile()->IsOutputLog)
	{
		GetModuleFileNameW(NULL,sinfo.ProcessName,1024);
		PathRemoveFileSpecW(sinfo.ProcessName));
		wsprintfW(FileName,L"%s\\Unlocker-Log.txt",sinfo.ProcessName);
		hFile=CreateFileW(FileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			TCHAR str[1024];
			DWORD NumberOfBytesWritten;
			wsprintf(str,"================================================================================\n\n");
			WriteFile(hFile,str,lstrlen(str),&NumberOfBytesWritten,NULL);
			wsprintf(str,"Looking for handles on following %d object(s):\n",StringGeneral1.CurPos);
			WriteFile(hFile,str,lstrlen(str),&NumberOfBytesWritten,NULL);
			wsprintf(str,"\n");
			WriteFile(hFile,str,lstrlen(str),&NumberOfBytesWritten,NULL);
			int i=0;
			while(i < StringGeneral1.CurPos && i >= 0)
			{
				wsprintf(str,"Object %d: %S\n",i+1,StringGeneral1.Buf[i][3]);
				WriteFile(hFile,str,lstrlen(str),&NumberOfBytesWritten,NULL);
				i++;
			}
			wsprintf(str,"\n================================================================================\n\n");
			WriteFile(hFile,str,lstrlen(str),&NumberOfBytesWritten,NULL);
		}
	}

	ProcessSInfo sinfo;
	do 
	{//斤噐耽倖序殻
		sinfo.ProcessID=pe.th32ProcessID;
		lstrcpyW(sinfo.ProcessName,pe.szExeFile);
		ProcessSInfoGeneral.Construct(&sinfo);
		HANDLE dllshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pe.th32ProcessID);
		if(dllshot != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32W me;
			me.dwSize=sizeof(MODULEENTRY32W);
			if(Module32FirstW(dllshot,&me))
			{//斤噐匯倖序殻議耽倖庁翠
				do 
				{
					lstrcpyW(FileName,me.szExePath);
					CharUpperW(FileName);
					LPWSTR ptr=PathFindExtensionW(me.szExePath);
					if(ptr && !lstrcmpiW(ptr,L".DLL"))
					{
						int j=0;
						while(j < StringGeneral1.CurPos)
						{
							if(StrStrW(FileName,StringGeneral1.Buf[j][3]) == FileName)
							{
								ProcessInfo cur;
								cur.Index=-1;
								lstrcpyW(cur.ProcessNameString,pe.szExeFile);
								cur.HandleId=-1;
								cur.ProcessId=me.th32ProcessID;
								lstrcpyW(cur.PathLockedString,me.szExePath);
								HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pe.th32ProcessID);
								if(hProcess)
								{
									WCHAR String[1024];
									HMODULE hModule;
									DWORD cbNeed;
									memset(String,0,1024);
									if(EnumProcessModules(hProcess,&hModule,sizeof(HMODULE),&cbNeed))
									{
										GetModuleBaseNameW(hProcess,hModule,cur.ProcessNameString,1024);
										GetModuleFileNameExW(hProcess,hModule,String,1000);
									}
									lstrcpyW(cur.ProcessPathString,lstrlenW(String)?String:pe.szExeFile);
									CloseHandle(hProcess);
								}
								else
								{
									lstrcpyW(cur.ProcessPathString,pe.szExeFile);
								}
								cur.hmod=me.hModule;
								ListProcessGeneral.Construct(&cur);
								if(GetMyOperateFile()->IsOutputLog)
									OutputLog(hFile,&cur);
							}
							j++;
						}
					}
				}
				while(Module32NextW(dllshot));
			}
			CloseHandle(dllshot);
		}
	} 
	while(Process32NextW(hsnapshot,&pe));
	CloseHandle(hsnapshot);

	int version=GetWindowsVersion();
	for(int i=0;i<ReturnLength;i++,process++)
	{
		if(version == process->ObjectTypeNumber)
		{
			HANDLE hProcess=OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_DUP_HANDLE | PROCESS_VM_READ,FALSE,process->ProcessID);
			if(!hProcess && !(hProcess=OpenProcess(PROCESS_DUP_HANDLE,FALSE,process->ProcessID)))
				continue;
			HANDLE TargetHandle;
			if(DuplicateHandle(hProcess,process->Handle,GetCurrentProcess(),&TargetHandle,0,FALSE,0))
			{
				HANDLE newfile=CreateFileW(L"\\\\?\\UnlockerDriver5",GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
				if(newfile != INVALID_HANDLE_VALUE)
				{
					DWORD newdata[2]={(DWORD)TargetHandle,(DWORD)process->Object_Pointer};
					DWORD NumberOfRead=0;
					OBJECT_NAME_INFORMATION objname;
					WCHAR String[1024];
					WriteFile(newfile,newdata,8,&NumberOfRead,NULL);
					memset(objname,0,1024);
					NumberOfRead=0;
					if(ReadFile(newfile,(LPVOID)objname,1024,&NumberOfRead,NULL))
					{
						objname.Name.Buffer=objname.NameBuffer;
						if(objname.Name.Length > 0)
							objname.NameBuffer[objname.Name.Length/2]=L'\0';
						memset(String,0,2048);
						if(StrStrW(objname.Name.Buffer,devicepath) == objname.Name.Buffer)
						{
							if(devicestrlen >= objname.Name.Length/2)
							{
								lstrcpyW(String,path);
							}
							else
							{
								wsprintfW(String,L"%s%s",path,objname.Name.Buffer+devicestrlen);
							}
						}
						if(lstrlenW(String) > 0)
						{
							WCHAR String1[1024],String2[1024];
							HMODULE hModule;
							DWORD cbNeed;
							memset(String2,0,2048);
							GetPath(String2,String);
							lstrcpyW(sinfo.ProcessName,String2);
							CharUpperW(sinfo.ProcessName);
							int k=0;
							while(k < StringGeneral1.CurPos)
							{
								if(StrStrW(sinfo.ProcessName,StringGeneral1.Buf[k][3]))
								{
									memcpy(String1,L"<UNKNOWN NAME>",30);
									memset(String1+15,0,2018);
									memcpy(FileName,L"<UNKNOWN PATH>",30);
									memset(FileName+15,0,2018);
									if(EnumProcessModules(hProcess,&hModule,sizeof(HMODULE),&cbNeed))
									{
										GetModuleBaseNameW(hProcess,hModule,cur.String1,1024);
										GetModuleFileNameExW(hProcess,hModule,FileName,1000);
									}
									else
									{
										int j=0;
										while(j < ProcessSInfoGeneral.CurPos)
										{
											if(ProcessSInfoGeneral.Buf[j].ProcessID == process->ProcessID)
											{
												lstrcpyW(String1,ProcessSInfoGeneral.Buf[j].ProcessName);
												lstrcpyW(FileName,ProcessSInfoGeneral.Buf[j].ProcessName);
											}
											j++;
										}
									}
									ProcessInfo cur;
									cur.Index=i;
									lstrcpyW(cur.ProcessNameString,String1);
									cur.ProcessId=process->ProcessID
									cur.HandleId=process->Handle;
									if(StrStrW(String2,L"0:\\") == String2)
									{
										wsprintfW(cur.PathLockedString,L"\\\\%s",String2+3);
									}
									else
									{
										lstrcpyW(cur.PathLockedString,String2);
									}
									lstrcpyW(cur.ProcessPathString,FileName);
									ListProcessGeneral.Construct(&cur);
									if(GetMyOperateFile()->IsOutputLog)
										OutputLog(hFile,&cur);

								}
								k++;
							}
						}
					}
					CloseHandle(newfile);
				}
				CloseHandle(TargetHandle);
			}
			CloseHandle(hProcess);
		}
	}
	if(GetMyOperateFile()->IsOutputLog && hFile)
	{
		CloseHandle(hFile);
	}
	*sysinfo=hMem;
	return 0;
}

void OnComboBoxSelectChange(HWND hDlg)
{
	ComboBoxSelectIndex=SendDlgItemMessage(hDlg,1015,CB_GETCURSEL,0,0);//彿坿。。。。。。。。。。。
	switch(ComboBoxSelectIndex)
	{
		case 2://??????????????????貫2蝕兵
			DialogBoxParam(GetModuleHandle(NULL),107,0,(DLGPROC)RenameDialogFunc,0);//彿坿。。。。。。。。。。。。。
			break;
			
		case 3:
			ChooseFolderForMovingObject();
			break;

		case 4:
			if(StringGeneral1.CurPos == 1 && ListProcessGeneral.CurPos == 1 && !PathIsDirectoryW((LPWSTR)StringGeneral1.Buf))
			{
				WCHAR String[1024];
				OPENFILENAMEW openfilename;
				lstrcpyW(String,(LPWSTR)StringGeneral1.Buf));
				memset(&openfilename,0,sizeof(OPENFILENAMEW));
				openfilename.lpstrFile=String;
				openfilename.lStructSize=sizeof(OPENFILENAMEW);
				openfilename.lpstrFilter=L"All Files";
				openfilename.nMaxFile=1024;
				if(GetSaveFileNameW(&openfilename))
				{
					RemoteInject2(String,ListProcessGeneral.Buf[0].ProcessId,(HANDLE)ListProcessGeneral.Buf[0]..HandleId);
					if(!GetMyOperateFile()->IsNotShowGUItoUnlock)
						MessageBox(NULL,GlobalStrings[39],GlobalStrings[13],MB_OK);
				}
			}
			break;

		default:
			break;
	}
}

int CALLBACK LockedDialogProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{//孀欺迄協議序殻
	switch(uMsg)
	{
		case WM_SIZE:
			{
				RECT rt;
				GetClientRect(hWnd,&rt);
				InvalidateRect(hWnd,&rt,TRUE);
				MoveWindow(ListCtrlhWnd,		ListCtrlRect.left,
												ListCtrlRect.top,
												rt.right-ListCtrlRect.right-rt.left,
												rt.bottom-ListCtrlRect.bottom-rt.top,TRUE);
				MoveWindow(QuitButtonhWnd,		rt.right+QuitButtonRect.left-rt.left,
												rt.bottom+QuitButtonRect.top-rt.top,
												QuitButtonRect.right,
												QuitButtonRect.bottom,TRUE);
				MoveWindow(UnlockAllButtonhWnd,	rt.right+UnlockAllButtonRect.left-rt.left,
												rt.bottom+UnlockAllButtonRect.top-rt.top,
												UnlockAllButtonRect.right,
												UnlockAllButtonRect.bottom,TRUE);
				MoveWindow(UnlockButtonhWnd,	rt.right+UnlockButtonRect.left-rt.left,
												rt.bottom+UnlockButtonRect.top-rt.top,
												UnlockButtonRect.right,	
												UnlockButtonRect.bottom,TRUE);
				MoveWindow(KillProcessButtonhWnd,	rt.right+KillProcessButtonRect.left-rt.left,
													rt.bottom+KillProcessButtonRect.top-rt.top,
													KillProcessButtonRect.right,
													KillProcessButtonRect.bottom,TRUE);
				MoveWindow(ComboBoxhWnd,		ComboBoxRect.left,	
												rt.bottom+ComboBoxRect.top-rt.top,
												ComboBoxRect.right,
												ComboBoxRect.bottom,TRUE);
				UpdateWindow(hWnd);
			}
			break;

		case WM_NOTIFY:
			{
				LPNMLISTVIEW items=(LPNMLISTVIEW)lParam;
				if(items->hdr.hwndFrom != ListCtrlhWnd || items->hdr.code != LVN_COLUMNCLICK)
					return 0;
				ListSortDirection[items->iSubItem] ^= 1;
				SendMessage(ListCtrlhWnd,LVM_SORTITEMS,(WPARAM)items->iSubItem,(LPARAM)CompareFunc);
			}
			break;

		case WM_INITDIALOG:
			{
				HICON mainicon=LoadIcon(GetModuleHandle(NULL),106);//夕炎彿坿。。。。。。。。。。。。
				SendMessage(hWnd,WM_SETICON,ICON_SMALL,(LPARAM)mainicon);
				if(mainicon) DestroyIcon(mainicon);
				SetWindowText(hWnd,"Unlocker 1.9.1");
				ListCtrlhWnd=GetDlgItem(hWnd,1012);//彿坿。。。。。。。。。。。。。。。。
				QuitButtonhWnd=GetDlgItem(hWnd,2);//彿坿。。。。。。。。。。。。。。。。
				SetDlgItemText(hWnd,2,GlobalStrings[18]);//彿坿。。。。。。。。。。。。。。。。
				UnlockAllButtonhWnd=GetDlgItem(hWnd,1);//彿坿。。。。。。。。。。。。。。。。
				SetDlgItemText(hWnd,1,GlobalStrings[17]);//彿坿。。。。。。。。。。。。。。。。
				UnlockButtonhWnd=GetDlgItem(hWnd,3);//彿坿。。。。。。。。。。。。。。。。
				SetDlgItemText(hWnd,3,GlobalStrings[16]);//彿坿。。。。。。。。。。。。。。。。
				KillProcessButtonhWnd=GetDlgItem(hWnd,4);//彿坿。。。。。。。。。。。。。。。。
				SetDlgItemText(hWnd,4,GlobalStrings[15]);//彿坿。。。。。。。。。。。。。。。。
				ComboBoxhWnd=GetDlgItem(hWnd,1015);
				SendDlgItemMessage(hWnd,1015,CB_ADDSTRING,0,(LPARAM)GlobalStrings[21]);//"No action"//彿坿。。。。。。。。。。。。。。。。
				SendDlgItemMessage(hWnd,1015,CB_ADDSTRING,0,(LPARAM)GlobalStrings[14]);//"Delete"//彿坿。。。。。。。。。。。。。。。。		
				SendDlgItemMessage(hWnd,1015,CB_ADDSTRING,0,(LPARAM)GlobalStrings[22]);//"Rename"//彿坿。。。。。。。。。。。。。。。。
				SendDlgItemMessage(hWnd,1015,CB_ADDSTRING,0,(LPARAM)GlobalStrings[23]);//"Move"//彿坿。。。。。。。。。。。。。。。。
				SendDlgItemMessage(hWnd,1015,CB_ADDSTRING,0,(LPARAM)GlobalStrings[38]);//"Move"//彿坿。。。。。。。。。。。。。。。。
				SendDlgItemMessage(hWnd,1015,CB_SETCURSEL,0,0);//"Copy"//彿坿。。。。。。。。。。。。。。。。
				
				POINT DlgPos={0,0};
				RECT Rect,CurRect;
				int len1,len2,len3,len4;
				ClientToScreen(hWnd,&DlgPos);
				GetClientRect(hWnd,&Rect);
				len1=DrawMessage(hWnd,GlobalStrings[18]);//"Quit"
				len2=DrawMessage(hWnd,GlobalStrings[17]);//"Unlock All"
				len3=DrawMessage(hWnd,GlobalStrings[16]);//"Unlock"
				len4=DrawMessage(hWnd,GlobalStrings[15]);//"Kill Process"
				GetWindowRect(QuitButtonhWnd,&CurRect);
				int ii=CurRect.right-DlgPos.x-len1;
				MoveWindow(hWnd,ii,CurRect.top-DlgPos.y,len1,CurRect.bottom-CurRect.top,TRUE);
				GetWindowRect(UnlockAllButtonhWnd,&CurRect);
				MoveWindow(UnlockAllButtonhWnd,ii-len2-6,CurRect.top-DlgPos.y,len2,CurRect.bottom-CurRect.top,TRUE);
				ii -= len2+len3+12;
				GetWindowRect(UnlockButtonhWnd,&CurRect);
				MoveWindow(UnlockButtonhWnd,ii,CurRect.top-DlgPos.y,len3,CurRect.bottom-CurRect.top,TRUE);
				GetWindowRect(KillProcessButtonhWnd,&CurRect);
				MoveWindow(KillProcessButtonhWnd,ii-len4-6,CurRect.top-DlgPos.y,len4,CurRect.bottom-CurRect.top,TRUE);
				ii=max( max(max(DrawMessage(hWnd,21),DrawMessage(hWnd,14)),DrawMessage(hWnd,22)),DrawMessage(hWnd,GlobalStrings[23]));
				//21="This message only appears once for each new version."/14="Delete"/22="No action"/23="Rename"
				GetWindowRect(ComboBoxhWnd,&CurRect);
				MoveWindow(ComboBoxhWnd,CurRect.left-DlgPos.x,CurRect.top-DlgPos.y,len1,CurRect.bottom-CurRect.top,TRUE);
				
				RECT DstRect;
				RECT* newRect=MakeRect1(&DstRect,ListCtrlhWnd,DlgPos,Rect);
				ListCtrlRect.left=newRect->left;
				ListCtrlRect.top=newRect->top;
				ListCtrlRect.right=newRect->right;
				ListCtrlRect.bottom=newRect->bottom;

				newRect=MakeRect3(&DstRect,QuitButtonhWnd,DlgPos,Rect);
				QuitButtonRect.left=newRect->left;
				QuitButtonRect.top=newRect->top;
				QuitButtonRect.right=newRect->right;
				QuitButtonRect.bottom=newRect->bottom;

				newRect=MakeRect3(&DstRect,UnlockAllButtonhWnd,DlgPos,Rect);
				UnlockAllButtonRect.left=newRect->left;
				UnlockAllButtonRect.top=newRect->top;
				UnlockAllButtonRect.right=newRect->right;
				UnlockAllButtonRect.bottom=newRect->bottom;

				newRect=MakeRect3(&DstRect,UnlockButtonhWnd,DlgPos,Rect);
				UnlockButtonRect.left=newRect->left;
				UnlockButtonRect.top=newRect->top;
				UnlockButtonRect.right=newRect->right;
				UnlockButtonRect.bottom=newRect->bottom;

				newRect=MakeRect3(&DstRect,KillProcessButtonhWnd,DlgPos,Rect);
				KillProcessButtonRect.left=newRect->left;
				KillProcessButtonRect.top=newRect->top;
				KillProcessButtonRect.right=newRect->right;
				KillProcessButtonRect.bottom=newRect->bottom;

				newRect=MakeRect2(&DstRect,ComboBoxhWnd,DlgPos,Rect);
				ComboBoxRect.left=newRect->left;
				ComboBoxRect.top=newRect->top;
				ComboBoxRect.right=newRect->right;
				ComboBoxRect.bottom=newRect->bottom;

				SendMessage(ListCtrlhWnd,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_LABELTIP | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
				HWND ToolTip=(HWND)SendMessage(ListCtrlhWnd,LVM_GETTOOLTIPS,0,0);
				SendMessage(ToolTip,TTM_SETDELAYTIME,TTDT_AUTOMATIC,MAKELONG(1,0));
				SendMessage(ToolTip,TTM_SETDELAYTIME,TTDT_AUTOPOP,MAKELONG(5000,0));
				ListViewInsertColumn(ListCtrlhWnd,GlobalStrings[0],86,0);//"Process"
				ListViewInsertColumn(ListCtrlhWnd,GlobalStrings[1],200,1);//"Path locked"
				ListViewInsertColumn(ListCtrlhWnd,GlobalStrings[2],40,2);//"PID"
				ListViewInsertColumn(ListCtrlhWnd,GlobalStrings[3],50,3);//"Handle"
				ListViewInsertColumn(ListCtrlhWnd,GlobalStrings[4],200,4);//"Process Path"
				HIMAGELIST imagelist=ImageList_Create(16,16,ILC_MASK | ILC_COLOR32,ListProcessGeneral.CurPos,1);
				ImageList_SetBkColor(imagelist,GetSysColor(COLOR_WINDOW));
				int i=0;
				HICON hicon=NULL;
				while(i < ListProcessGeneral.CurPos)
				{
					if(i == 0 || ListProcessGeneral.Buf[i].ProcessId != ListProcessGeneral.Buf[i-1].ProcessId)
					{
						if(hicon)
						{
							DestroyIcon(hicon);
							hicon=NULL;
						}
						ExtractIconExW(ListProcessGeneral.Buf[i].ProcessPathString,0,NULL,&hicon,1);
						if(!hicon)
							hicon=LoadIcon(NULL,IDI_APPLICATION);
					}
					ImageList_ReplaceIcon(imagelist,-1,hicon);
					i++;
				}
				if(icon)
				{
					DestroyIcon(hicon);
					hicon=NULL;
				}
				SendMessage(ListCtrlhWnd,LVM_SETIMAGELIST,LVSIL_SMALL,(LPARAM)imagelist);
				
				i=0;
				while(i < ListProcessGeneral.CurPos)
				{
					ListViewAddItems(ListCtrlhWnd,i++);
				}
				ListSortDirection[0]=0;
				SendMessage(ListCtrlhWnd,LVM_SORTITEMS,0,(LPARAM)CompareFunc);
				SendMessage(ListCtrlhWnd,WM_SETREDRAW,TRUE,0);
				PostMessage(hWnd,WM_UPDATEUISTATE,MAKEWPARAM(UIS_CLEAR,UISF_HIDEFOCUS),NULL);
				SetWindowApparenceFromFile(hWnd);
				ShowWindow(hWnd,SW_SHOWNORMAL);
			}
			break;

		case WM_COMMAND:
			{
				switch(LOWORD(wParam))//彿坿。。。。。。。。。。。。。。
				{
					case 1:
						{
							int i=0;
							while(i < ListProcessGeneral.CurPos)
							{
								ListItemGeneral.Construct(i++);
								WriteWindowApparenceToFile(hWnd,1);
							}
						}
						break;

					case 2:
						ListItemGeneral.CurPos=0;
						WriteWindowApparenceToFile(hWnd,2);
						break;

					case 3:
						{
							int nextitem=SendMessage(ListCtrlhWnd,LVM_GETNEXTITEM,-1,LVNI_SELECTED);
							ListItemGeneral.CurPos=0;
							if(nextitem != -1)
							{
								while(nextitem != -1)
								{
									ListItemGeneral.Construct(nextitem);
									nextitem=SendMessage(ListCtrlhWnd,LVM_GETNEXTITEM,nextitem,LVNI_SELECTED)
								}
								if(!ListItemGeneral.CurPos)
									return 1;
								IsToKillProcess=FALSE;
								WriteWindowApparenceToFile(hWnd,3);
							}
						}
						break;
					
					case 4:
						{
							int nextitem=SendMessage(ListCtrlhWnd,LVM_GETNEXTITEM,-1,LVNI_SELECTED);
							ListItemGeneral.CurPos=0;
							if(nextitem != -1)
							{
								while(nextitem != -1)
								{
									ListItemGeneral.Construct(nextitem);
									nextitem=SendMessage(ListCtrlhWnd,LVM_GETNEXTITEM,nextitem,LVNI_SELECTED)
								}
								if(!ListItemGeneral.CurPos)
									return 1;
								IsToKillProcess=TRUE;
								WriteWindowApparenceToFile(hWnd,4);
							}
						}

					case 1015:
						if(HIWORD(wParam) == CBN_SELCHANGE)
							OnComboBoxSelectChange(hWnd);
						break;

					default:
						return 0;
				}
			}
			break;

		default:
			return 0;
	}
	return 1;
}

int CALLBACK UnlockedDialogProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{//短嗤孀欺迄協議序殻
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				HICON icon=LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(106));//ICON彿坿。。。。。。。。。。。。。。。
				SendMessage(hWnd,WM_SETICON,ICON_SMALL,(LPARAM)icon);
				if(icon) DestroyIcon(icon);
				SetWindowText(hWnd,"Unlocker 1.9.1");
				SetDlgItemText(hWnd,2,GlobalStrings[18]);//彿坿。。。。。。。。。。。。。。。。。。。。。。
				SetDlgItemText(hWnd,1,GlobalStrings[30]);//彿坿。。。。。。。。。。。。。。。。。。。。。。
				ComboBoxhWnd=GetDlgItem(hWnd,1015);//彿坿。。。。。。。。。。。。。。。。。。。。。。
				SendDlgItemMessage(hWnd,1015,CB_ADDSTRING,0,(LPARAM)GlobalStrings[21]);//"No action"//彿坿。。。。。。。。。。。。。。。。
				SendDlgItemMessage(hWnd,1015,CB_ADDSTRING,0,(LPARAM)GlobalStrings[14]);//"Delete"//彿坿。。。。。。。。。。。。。。。。		
				SendDlgItemMessage(hWnd,1015,CB_ADDSTRING,0,(LPARAM)GlobalStrings[22]);//"Rename"//彿坿。。。。。。。。。。。。。。。。
				SendDlgItemMessage(hWnd,1015,CB_ADDSTRING,0,(LPARAM)GlobalStrings[23]);//"Move"//彿坿。。。。。。。。。。。。。。。。
				SendDlgItemMessage(hWnd,1015,CB_SETCURSEL,0,0);//彿坿。。。。。。。。。。。。。。。。
				
				TCHAR String[2048];
				wsprintf(String,"%s\n\n%s\n\n%s",GlobalStrings[9],GlobalStrings[32],GlobalStrings[33]);
				SetDlgItemText(hWnd,1017,String);
				PostMessage(hWnd,WM_UPDATEUISTATE,MAKEWPARAM(UIS_CLEAR,UISF_HIDEFOCUS),NULL);			
			}
			break;
			
		case WM_COMMAND:
			{
				switch(LOWORD(wParam))
				{
					case 1://彿坿。。。。。。。。。。。。。OK。。。。。。。。。
						EndDialog(hWnd,1);
						break;
						
					case 2://彿坿。。。。。。。。。。。。。QUIT。。。。。。。。。
						ComboBoxSelectIndex=0;
						EndDialog(hWnd,2);
						break;

					case 1015://彿坿。。。。。。。。。。。。。。。。。。。。。。
						if(HIWORD(wParam) == CBN_SELCHANGE)
							sub_414299(hWnd);
						break;

					default:
						return 0;
				}
			}
			break;
			
		default:
			return 0;
	}
	return 1;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd)
{
	WCHAR Stringu[4][1024],String0[2][512],szPath[1024],String1[1024],sz[1024];
	TCHAR String2[1024],FileName[256];

	GetMyOperateFile()->GetOperateFileParamFromCmdLine();
	InitString();
	if(!GetMyOperateFile()->IsNotShowGUItoUnlock)
		ShowUpdateWindow("Unlocker 1.9.1");
	if(!GetMyCoreAPIArray()->GetCoreAPIArray())
		return 1;
	if(!GetPrivilege(SE_DEBUG_PRIVILEGE))
	{
		ShowMessageBox("%s\n\n%s",GlobalStrings[6],GlobalStrings[7]);//"Error Backup Privileges" ...吾郡阻杏...
		return 1;
	}
	if(!GetPrivilege(SE_LOAD_DRIVER_PRIVILEGE))
	{
		ShowMessageBox("%s\n\n%s",GlobalStrings[37],GlobalStrings[7]);//"Error Load Driver Privileges"
		return 1;
	}
	if(!GetPrivilege(SE_BACKUP_PRIVILEGE))
	{
		ShowMessageBox("%s\n\n%s",GlobalStrings[5],GlobalStrings[7]);//"Error Debug Privileges"  ...吾郡阻杏...
		return 1;
	}
	if(!GetPrivilege(SE_TAKE_OWNERSHIP_PRIVILEGE))
	{
		CangetOwnership=FALSE;
	}
	
	GetModuleFileNameW(NULL,szPath,1024);
	PathRemoveFileSpecW(szPath);
	wsprintfW(GlobalFileName, L"%s\\Unlocker.cfg",szPath);
	if(!DriverInit(szPath))
		return 1;

	INITCOMMONCONTROLSEX picce={sizeof(INITCOMMONCONTROLSEX),ICC_LISTVIEW_CLASSES};
	if(!InitCommonControlsEx(&picce))
		return 1;

	if(!GetMyOperateFile()->IsListOfFiletoUnlock)
	{
		LPMALLOC hMem;

		if(GetMyOperateFile()->Command)
		{
			NULLString(Stringu);
			lstrcpyW(Stringu[0],GetMyOperateFile()->Command);
			StringGeneral1.Construct((BYTE*)Stringu[0]);
		}
		else if(!SHGetMalloc(&hMem))
		{
			BROWSEINFOW bi;
			CoInitialize(NULL);
			memset(&bi,0,sizeof(BROWSEINFOW));
			memset(String0,0,sizeof(String0));
			bi.ulFlags=BIF_BROWSEINCLUDEFILES | BIF_NONEWFOLDERBUTTON | BIF_NEWDIALOGSTYLE;
			bi.lpfn=(BFFCALLBACK)BrowseCallbackProc;
			LPITEMIDLIST itemlist=SHBrowseForFolderW(&bi);
			if(itemlist == NULL) return 1;
			SHGetPathFromIDListW(itemlist,String0[0]);
			NULLString(Stringu[0]);
			lstrcpyW(Stringu[0],String0[0]);
			StringGeneral1.Construct((BYTE*)Stringu[0]);
			hMem->lpVtbl->Free(hMem,itemlist);
			CoUninitialize();
			hMem->lpVtbl->Release(hMem);
		}
	}
	else if(!ReadFileToMemory(GetMyOperateFile()->Command,&StringGeneral1))
		return 1;

	if(GetMyOperateFile()->Operation == FILERENAME || GetMyOperateFile()->Operation == FILEMOVE)
	{
		if(GetMyOperateFile()->IsListOfFiletoUnlock)
		{
			if(!ReadFileToMemory(GetMyOperateFile()->ObjectName,&StringGeneral2))
				return 1;
		}
		else
		{
			NULLString(Stringu[0]);
			lstrcpyW(Stringu[0],GetMyOperateFile()->ObjectName);
			StringGeneral2.Construct(Stringu[0]);
		}
	}
	memset(String1[0],0,2048);

	SIZE_T i=0;

	while(StringGeneral1.CurPos > i)
	{
		if(StrStrW((LPWSTR)StringGeneral1.Buf[i],L"\\\\") == (LPWSTR)StringGeneral1.Buf[i])
		{
			LPWSTR str=GlobalAlloc(GMEM_ZEROINIT,2048);
			wsprintfW(str,L"0:\\%s",StringGeneral1.Buf[i][0]+2);
			GetPath(String0[0],str);
			if(str) GlobalFree((HGLOBAL)str);
		}
		else
		{
			GetPath(String0[0],StringGeneral1.Buf[i][0]);
		}
		lstrcpyW(sz,String0[0]);
		PathStripToRootW(sz);
		sz[2]=L'\0';
		if(!lstrlenW(sz)) return 1;
		CharUpperW(sz);
		if(i)
		{
			if(lstrcmpW(sz,String1)) return 1;
		}
		else
		{
			lstrcpyW(String1,sz);
			lstrcpyW(ADsPath,String0[0]);
			PathRemoveFileSpecW(ADsPath);
			lstrcpyW(EditString,String0[0]);
			PathStripPathW(EditString);
		}
		LPWSTR wptr=PathSkipRootW(String0[0]),pptr=sz;
		if(wptr && lstrcmpW(wptr,L"\""))
		{
			pptr=String0;
		}
		lstrcpyW(StringGeneral1.Buf[i][3],pptr);
		CharUpperW(StringGeneral1.Buf[i][3]);
		lstrcpyW(StringGeneral1.Buf[i][1],String0[0]);
		PathStripPathW(StringGeneral1.Buf[i][1]);
		PathRemoveExtensionW(StringGeneral1.Buf[i][1]);
		wptr=PathFindExtensionW(String0[0]);
		if(wptr) pptr=wptr+1;
		else pptr=L"\0";
		lstrcpyW(StringGeneral1.Buf[i][2],pptr);
		i++;
	}
	
	wsprintfA(FileName,"\\\\.\\%c:",sz[0]);
	OBJECT_NAME_INFORMATION objinfo;
	ULONG retlen;
	LPWSTR wptr=NULL;
	HANDLE hfile=CreateFile(FileName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,
					FILE_FLAG_BACKUP_SEMANTICS,NULL);
	if(hfile == INVALID_HANDLE_VALUE)
	{
		hfile=CreateFile(FileName,0,0,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
		if(hfile == INVALID_HANDLE_VALUE)
			goto ProcessError;
	}

	if(!GetMyCoreAPIArray()->ZwQueryObject(hfile,ObjectNameInformation,&objinfo,sizeof(OBJECT_NAME_INFORMATION),&retlen) && 
			objinfo.Name.Length > 0)
	{
		wptr=(LPWSTR)GlobalAlloc(GMEM_ZEROINIT,2048);
		objinfo.Name.Buffer[objinfo.Name.Length/2]=L'\0';
		lstrcpyW(wptr,objinfo.Name.Buffer);
	}
	CloseHandle(hfile);

ProcessError:
	if(!wptr)
	{
		wsprintf(String0[1],"%c:",sz[0]);
		if(QueryDosDevice(String0[1],String2,1024) && lstrlen(String2))
		{
			LPSTR findpos=StrStr(String2,"LanmanRedirector");
			if(findpos)
			{

				*(findpos+lstrlen("LanmanRedirector\\"))='\0';
				LPSTR findpos2=StrStr(findpos,L"\\");
				if(findpos2)//圻殻會嶄葎findpos2+1...状誼音斤
				{
					wptr=(LPWSTR)GlobalAlloc(GMEM_ZEROINIT,2048);
					wsprintfW(wptr,L"%S%S",findpos,findpos2+1);
				}
			}
			else if(!StrStr(String2,"\\??\\"))
			{
				wptr=(LPWSTR)GlobalAlloc(GMEM_ZEROINIT,2048);
				wsprintfW(wptr,L"%S",String2);
			}
		}
	}
	if(sz[0] == L'0')
	{
		wptr=(LPWSTR)GlobalAlloc(GMEM_ZEROINIT,2048);
		wsprintfW(wptr,L"\\Device\\LanmanRedirector");
	}
	if(!wptr)
	{
		ShowMessageBox(GlobalStrings[8]);//"Drive access denied"
		return 1;
	}
	lstrcpyW(ssPath,wptr);
	int len=lstrlenW(wptr);
	if(*(wptr+len-1) == L'\\')
	{
		*(wptr+len-1) == L'\0';
		len--;
	}

	bool flag=FALSE,Itemleft;

	do 
	{
		SYSTEM_HANDLE_INFORMATION_EX* hMem;
		Itemleft=FALSE;
		UnlockProcess(&hMem,wptr,len,String1);
		if(GetMyOperateFile()->IsNotShowGUItoUnlock)
		{
			if(ListProcessGeneral.CurPos)
			{
				int i=0;
				while(i < ListProcessGeneral.CurPos)
				{
					ListItemGeneral.Construct(i++);
				}
				if(ListItemGeneral.CurPos)
					RemoteInject1(hMem);
			}
			ProcessRequestByteOperationType(GetMyOperateFile()->Operation);
			GlobalFree(hMem);
		}
		else
		{
			if(!ListProcessGeneral.CurPos)
			{
				if(!flag)
				{
					ComboBoxSelectIndex=0;
					DialogBoxParam(GetModuleHandle(NULL),109,NULL,(DLGPROC)UnlockedDialogProc,0);
				}
				ProcessRequestByteOperationType(ComboBoxSelectIndex);
				GlobalFree(hMem);
			}
			DialogBoxParam(GetModuleHandle(NULL),101,NULL,(DLGPROC)LockedDialogProc,0);
			if(ListItemGeneral.CurPos)
			{
				RemoteInject1(hMem);
				Itemleft=TRUE;
			}
			GlobalFree(hMem);
		}
	} 
	while(Itemleft);

	DriverUnload();
	EndUpdateThread();
	return 0;
}

void start()
{
	begin();
	int nRet=WinMain(NULL,NULL,NULL,0);
	end();
	ExitProcess(nRet);
}

LRESULT CALLBACK UpdateWindowNewProcedure(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_SETCURSOR:
			SetCursor(GlobalCursor);
			return 1;
			
		case WM_NCDESTROY:
			SetWindowLong(hwnd,GWL_WNDPROC,UpdateWindowOldProcedure);
			if(GlobalCursor) 
				DestroyCursor(GlobalCursor);
			break;
			
		case WM_NCHITTEST:
			return 1;
			
		case WM_LBUTTONDOWN:
			SetCapture(hwnd);
			IsCaptured=TRUE;
			break;
			
		case WM_LBUTTONUP:
			ReleaseCapture();
			if(IsCaptured)
			{
				POINT Point={LOWORD(lParam),HIWORD(lParam)};
				RECT Rect;
				IsCaptured=FALSE;
				ClientToScreen(hwnd,&Point);
				GetWindowRect(hwnd,&Rect);
				if(PtInRect(&Rect,Point))
				{
					SetCursor(LoadCursor(NULL,IDC_WAIT));
					ShellExecute(NULL,"open","http://www.emptyloop.com/unlocker/",NULL,NULL,SW_SHOWNORMAL);
				}
			}
			break;
			
		default:
			break;
	}
	return CallWindowProc((WNDPROC)UpdateWindowOldProcedure,hwnd,uMsg,wParam,lParam);
}

INT_PTR CALLBACK UPDATEDIALOGPROC(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				char chText[256],shText[256];
				SetWindowText(hwndDlg,WindowTitle);
				SetDlgItemText(hwndDlg,1017,GlobalStrings[20]);//斤三崇108議陣周1017.。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
				SetDlgItemText(hwndDlg,1,GlobalStrings[30]);//斤三崇108議陣周1.。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
				LPSTR pos=StrStr(GlobalStrings[19],"http://www.emptyloop.com/unlocker/");
				//"There is a new version of Unlocker available at http://unlocker.emptyloop.com/"
				if(pos)
				{
					int i=0;
					while(GlobalStrings[19]+i != pos)
					{
						chText[i]=GlobalStrings[19][i];
						i++;
					}
					chText[i]='\0';
				}
				int totallen=lstrlen(chText)+lstrlen("http://www.emptyloop.com/unlocker/");
				if(totallen < lstrlen(GlobalStrings[19]))
				{
					int i=0;
					for(int i=0,j=totallen;j<lstrlen(GlobalStrings[19];j++)
						shText[i++]=GlobalStrings[19][j];
				}
				else
				{
					shText[0]='\0';
				}
				
				SetDlgItemText(hwndDlg,1018,chText);//斤三崇108議陣周1018.。。。。。。。。。。。。。。。。。。。。。。。。。。。。
				SetDlgItemText(hwndDlg,1019,"http://www.emptyloop.com/unlocker/");//斤三崇108議陣周1019.。。。。。。。。。。。。。
				SetDlgItemText(hwndDlg,1020,shText);//斤三崇108議陣周1020.。。。。。。。。。。。。。。。。。。。。。。。。。。。。
				POINT Point={0,0};
				RECT Rect,rc1={0,0,0,0},rc2={0,0,0,0},rc3={0,0,0,0},rc4={0,0,0,0};
				int x1,x2,y1,y2,x3,y3;
				ClientToScreen(hwndDlg,&Point);
				GetWindowRect(GetDlgItem(hwndDlg,1017),&Rect);//斤三崇108議陣周1017.。。。。。。。。。。update1
				x1=Rect.left-Point.x;
				y1=Rect.top-Point.y;
				GetWindowRect(GetDlgItem(hwndDlg,1),&Rect);//斤三崇108議陣周1.。。。。。。。。。。。。。ok
				HDC hdc=GetDC(hwndDlg);
				HFONT newfont=(HFONT)SendMessage(hwndDlg,WM_GETFONT,0,0);
				HFONT oldfont=(HFONT)SelectObject(hdc,(HGDIOBJ)SendMessage(hwndDlg,HFONT));
				DrawText(hdc,GlobalStrings[20],-1,&rc1,DT_CALCRECT);//"This message only appears once for each new version."
				MoveWindow(GetDlgItem(hwndDlg,1017),x1,y1,x1+rc1.right,y1+rc1.bottom,TRUE);//斤三崇108議陣周1017.。。。。。。。。。。。。。。。。。。
				y3=y1+2*rc1.bottom;
				DrawText(hdc,chText,-1,&rc2,DT_CALCRECT);
				MoveWindow(GetDlgItem(hwndDlg,1018),x1,y3,rc2.right,rc2.bottom,TRUE);//斤三崇108議陣周1018.。。。。。。。。。。。。
				x3=x1+rc2.right;
				DrawText(hdc,"http://www.emptyloop.com/unlocker/",-1,&rc3,DT_CALCRECT);
				MoveWindow(GetDlgItem(hwndDlg,1019),x3,y3,rc3.right,rc3.bottom,TRUE);//斤三崇108議陣周1019.。。。
				x3+=rc3.right;
				DrawText(hdc,shText,-1,&rc4,DT_CALCRECT);
				MoveWindow(GetDlgItem(hwndDlg,1020),x3.right,y3,rc4.right,rc4.bottom,TRUE);//斤三崇108議陣周1020.。。
				
				int delta11=y3+2*max(max(rc2.bottom,rc3.bottom),rc4.bottom);
				int delta22=max(x1+rc4.right+x3,rc1.right+2*x1)+2*GetSystemMetrics(SM_CYBORDER);
				int delta33=y1+y2+delta11+GetSystemMetrics(SM_CYMENU)+2*x2;
				MoveWindow(GetDlgItem(hwndDlg,1),(delta22-x2)/2,delta11,x2,y2,TRUE);//斤三崇108議陣周1.。。。。。。。。。。。
				MoveWindow(hwndDlg,(GetSystemMetrics(SM_CXSCREEN)-delta11)/2,(x3-delta33)/2,delta22,delta33,TRUE);
				SelectObject(hdc,oldfont);
				LOGFONT logfont;
				GetObject((HGDIOBJ)newfont,sizeof(LOGFONT),&logfont);
				logfont.lfUnderline=TRUE;
				SendMessage(GetDlgItem(hwndDlg,1019),WM_SETFONT,(WPARAM)CreateFontIndirect(&logfont),FALSE);//斤三崇108議陣周1019.。。。
				GlobalCursor=LoadCursor(NULL,IDC_HAND);
				UpdateWindowOldProcedure=SetWindowLong(GetDlgItem(hwndDlg,1019),GWL_WNDPROC,(LONG)UpdateWindowNewProcedure);
			}
			break;
			
		case WM_COMMAND:
			if(LOWORD(wParam) == 1)//斤三崇108議陣周1.。。。。。。。。。。。
				EndDialog(hwndDlg,0);
			break;
			
		default:
			return 0;			
	}
	return 1;
}

void EndUpdateThread()
{
	if(UpdateThread)
	{
		WaitForSingleObject(UpdateThread,INFINITE);
		CloseHandle(UpdateThread);
	}
}

DWORD WINAPI CheckUpdate(LPVOID lpThreadParameter)
{
	HKEY hKey;
	DWORD cbData=1024;
	BYTE Data[1024];
	WSADATA WSAData;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Unlocker",0,
								KEY_QUERY_VALUE | KEY_SET_VALUE,&hKey);
	if(RegQueryValueEx(hKey,"OnlineVersion",NULL,NULL,Data,&cbData)) return 0;
	
	if(!WSAStartup(MAKEWORD(1,1),&WSAData)) 
	{
		SOCKET sock=socket(AF_INET,SOCK_STREAM,0);
		if(sock != INVALID_SOCKET)
		{
			HOSTENT*hostinfo= gethostbyname("www.emptyloop.com");
			SOCKADDR_IN sai;
			sai.sin_family=AF_INET;
			sai.sin_addr.s_addr=*(DWORD*)hostinfo->h_addr_list[0];
			sai.sin_port=htons(80);
			
			if(!connect(sock,(SOCKADDR*)&sai,sizeof(SOCKADDR)))
			{
				TCHAR buf[64],revbuf[512];
				memcpy(buf,"GET /unlocker/version.txt HTTP/1.0\r\nHost: www.emptyloop.com\r\n\r\n",64);
				int len=strlen(buf),revnum;
				if(len == send(sock,buf,len,0))
				{
					char ch='\0';
					while(recv(sock,revbuf,1,) > 0)//響函欺銭偬議'\n'
					{
						if(revbuf[0] == '\n' && ch == '\n') break;
						if(revbuf[0] != '\r') ch=revbuf[0];
					}
					GeneralClass<BYTE> Update;
					while((revnum=recv(sock,revbuf,512,0)) > 0)
					{
						for(int i=0;i<revnum;i++)
							Update.Construct(revbuf+i);
					}
					if(lstrlen(Update.Buf) > 0 && lstrcmp((LPSTR)Data,Update.Buf))
					{
						DialogBoxParam(GetModuleHandle(NULL),MAKEINTRESOURCE(108),NULL,(DLGPROC)UPDATEDIALOGPROC,0);//斤三崇庁医108。。。。。。。。。。
						RegSetValueEx(hKey,"OnlineVersion",0,REG_SZ,(BYTE*)Update.Buf,lstrlen(Update.Buf));
					}
					Update.Destruct();
				}
			}
		}
	}
	RegCloseKey(hKey);
	return 0;
}

void ShowUpdateWindow(LPCSTR title)
{
	lstrcpy(WindowTitle,title);
	UpdateThread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CheckUpdate,NULL,0,NULL);
}

//Global Start Func Begin
void GlobalStart1()
{
	ListItemGeneral.Destruct();
}
void GlobalStart2()
{
	ListProcessGeneral.Destruct();
}
void GlobalStart3()
{
	StringGeneral1.Destruct();
}
void GlobalStart4()
{
	StringGeneral2.Destruct();
}
void GlobalStart5()
{
	ProcessSInfoGeneral.Destruct();
}
//Global Start Func End
