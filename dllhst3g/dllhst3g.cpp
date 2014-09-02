#include <windows.h>
#pragma comment(lib,"ole32.lib")
extern "C" HRESULT WINAPI CoRegisterSurrogateEx(REFGUID rguidProcessID,void* reserved);
//本人在原始汇编代码上稍作相应c语言优化，不会降低影响代码运行效率
int WINAPI GetCommandLineArguments(LPCSTR lpString,char** matrix,int MatrixWidth,int MatrixLength)
{//这段代码经过分析，可以知道是将lpString中的字符串以空格为分界分成子字符串放入字符串矩阵matrix中
//matrix大小为MarixWidth*MatrixLength
//这种微软内部的代码都是极好的，适合用来分析，遇到类似情况可以直接用此简洁的代码，具体的大家可以分析，不懂得地方可以问我
	LPCSTR curpos=lpString;
	int len=lstrlenA(lpString);
	int i=0,j=0,somenum=0;
	if(len>0)
	{
		while(somenum<len)
		{
			if(j>MatrixLength) return 0;
			char curchar=*curpos;
			curpos++;
			if(curchar == ' ')
			{
				if(j!=0)
				{
					matrix[i][j]=0;
					i++;
					j=0;
					if(i == MatrixWidth) return i;
				}
			}
			else 
			{
				matrix[i][j]=curchar;
				j++;
			}
			somenum++;
		}

		if(j!=0) matrix[i][j]=0;
		i++;
	}
	return i;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
#define WIDTH	1
#define LENGTH	260
	char command[LENGTH];

	if(GetCommandLineArguments(lpCmdLine,(char**)&command,WIDTH,LENGTH) < 1)
		return 0;

	char* lpMultiByteStr=command;
	if(command[0] != '\0')
	{
		while(*lpMultiByteStr != ':')
		{
			lpMultiByteStr++;
			if(*lpMultiByteStr == '\0') break;
		}
		if(*lpMultiByteStr != '\0')
		{
			*lpMultiByteStr = '\0';
			lpMultiByteStr++;
		}
		if(*lpMultiByteStr == '\0' || lstrcmpi(command,"/ProcessID")!=0)
			lpMultiByteStr=command;
	}


	WCHAR WideCharStr[41];
	CLSID clsid;
	if(MultiByteToWideChar(CP_ACP,0,lpMultiByteStr,lstrlen(lpMultiByteStr)+1,WideCharStr,41) !=0 &&
		CLSIDFromString(WideCharStr,&clsid) >= 0 && CoInitializeEx(NULL,COINIT_MULTITHREADED) >=0 )
	{
		CoRegisterSurrogateEx(clsid,NULL);
		CoUninitialize();
		TerminateProcess(GetCurrentProcess(),0);
	}
	return 0;
}