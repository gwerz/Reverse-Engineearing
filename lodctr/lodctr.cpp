// LODCTR:更新与性能计数器相关的注册表值。
// 
// 用法:
// LODCTR <INI-FileName>
// 	INI-FileName 是所包含的的初始文件的名称计数器 DLL 的计数器名称定义和说明文字。
// 
// 	LODCTR /S:<Backup-FileName> 将当前的 perf 注册表字符串和信息保存到 <Backup-FileName>
// 
// 	LODCTR /R:<Backup-FileName> 使用 <Backup-FileName> 还原 perf 注册表字符串和信息
// 
// 	LODCTR /R 基于当前情况从头开始重建 perf 注册表字符串和信息注册表设置和备份 INI 文件。
// 
// 	LODCTR /T:<Service-Name> 将性能计数器服务设置为受信任。
// 
// 	LODCTR /E:<Service-Name> 启用性能计数器服务。
// 
// 	LODCTR /D:<Service-Name> 禁用性能计数器服务。
// 
// 	LODCTR /Q
// 
// 	LODCTR /Q:<Service-Name> 通过查询全部计数器或者查询指定的一个计数器，查询性能计数器服务信息。
// 
// 	LODCTR /M:<Counter-Manifest> 安装 Windows Vista 性能计数器提供程序定义 XML 文件到系统知识库。
// 
// 注意: 任何名称中带空格的参数都必须外加双引号。

#include <windows.h>
#include <loadperf.h>

#pragma comment(lib,"loadperf.lib")

char Buffer[520];
WCHAR szCommentString[2]=L"\0";

char* __fastcall restoreinfo(char* command)
{
	char* dest=NULL;

	if((command[0] == '-' || command[0] == '/') && (command[1] == 'r' || command[1] == 'R') && (command[2] == ':'))
	{//LODCTR /R:<Backup-FileName> 使用 <Backup-FileName> 还原 perf 注册表字符串和信息
		if(!SearchPath(NULL,&command[3],NULL,sizeof(Buffer),Buffer,NULL))//没搜到
			dest=Buffer;
		else
			dest=&command[3];		
	}
	return dest;
}

char* __fastcall unknownsub3(char* command)
{
	char* dest=NULL;

	if((command[0] == '-' || command[0] == '/') && (command[1] == 's' || command[1] == 'S') && (command[2] == ':'))
	{//LODCTR /S:<Backup-FileName> 将当前的 perf 注册表字符串和信息保存到 <Backup-FileName>
		if(!SearchPath(NULL,&command[3],NULL,sizeof(Buffer),Buffer,NULL))
			dest=Buffer;
		else
			dest=&command[3];
	}
	return dest;
}

bool WINAPI saveinfo(char** argv,LPSTR* szNewCtrlFilePath,LPSTR* szNewHlpFilePath,LPSTR* szLanguageID)
{
	DWORD i,FLAG;
	for(i=1,FLAG=0;i<=3;i++)
	{
		if(argv[i][0] == '-' || argv[i][0] == '/')
		{
			switch(argv[i][1])
			{
				case 'c':
				case 'C':
					if(argv[i][2] == ':')
					{
						*szNewCtrlFilePath=&argv[i][3];
						FLAG |= 1;
					}
					break;

				case 'h':
				case 'H':
					if(argv[i][2] == ':')
					{
						*szNewHlpFilePath=&argv[i][3];
						FLAG |=2;
					}
					break;

				case 'l':
				case 'L':
					if(argv[i][2] == ':')
					{
						*szLanguageID=&argv[i][3];
						FLAG |=4;
					}
					break;

				default:
					break;
			}
		}
	}
	return FLAG == (1|2|4);
}



DWORD main(int argc,char** argv)
{
	LPSTR szNewCtrlFilePath,szNewHlpFilePath;

	if(argc >= 2)
	{
		if(argc >= 4)
		{
			szNewCtrlFilePath=NULL;
			szNewHlpFilePath=NULL;
			argc=0;
			LPSTR szLanguageID;
			if(saveinfo(argv,&szNewCtrlFilePath,&szNewHlpFilePath,&szLanguageID))
				return UpdatePerfNameFiles(szNewCtrlFilePath,szNewHlpFilePath,szLanguageID,0);
		}
		else//if(argc == 3)
		{
			DWORD DATA;
			szNewHlpFilePath=unknownsub3(argv[1]);
			if(szNewHlpFilePath)
			{
				int len=lstrlen(szNewHlpFilePath)+1;
				WCHAR* pchar=(WCHAR*)HeapAlloc(GetProcessHeap(),0,2*len);
				if(pchar)
				{
					mbstowcs(pchar,szNewHlpFilePath,len);
					DATA=BackupPerfRegistryToFileW(pchar,szCommentString);
					HeapFree(GetProcessHeap(),0,pchar);
					return DATA;
				}
			}

			szNewHlpFilePath=restoreinfo(argv[1]);
			if(szNewHlpFilePath)
			{
				int len=lstrlen(szNewHlpFilePath)+1;
				WCHAR* pchar=(WCHAR*)HeapAlloc(GetProcessHeap(),0,2*len);
				if(pchar)
				{
					mbstowcs(pchar,szNewHlpFilePath,len);
					DATA=RestorePerfRegistryFromFileW(pchar,NULL);
					HeapFree(GetProcessHeap(),0,pchar);
					return DATA;
				}
			}

			if((argv[1][0] == '-' || argv[1][0] == '/') && (argv[1][1] == 't' || argv[1][1] == 'T') && (argv[1][2] == ':'))
			{
				char* ptr=&argv[1][3];
				if(ptr)
				{
					int len=lstrlen(ptr)+1;
					WCHAR* pchar=(WCHAR*)HeapAlloc(GetProcessHeap(),0,2*len);
					if(pchar)
					{
						mbstowcs(pchar,ptr,len);
						DATA=SetServiceAsTrustedW(NULL,pchar);
						HeapFree(GetProcessHeap(),0,pchar);
						return DATA;
					}
				}
			}
		}
	}

	LoadPerfCounterTextStringsW(GetCommandLineW(),FALSE);
	return 0;
}
