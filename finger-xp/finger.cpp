//#include "C:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v7.0A\\Include\\WinSock2.h"
#include "WinSock2.h"
//#include "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include\WS2tcpip.h"
#include "WS2tcpip.h"
//#pragma comment(lib,"C:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v7.0A\\Lib\\MsWSock.Lib")
#pragma comment(lib,"MsWSock.Lib")
//#pragma comment(lib,"C:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v7.0A\\Lib\\WS2_32.Lib")
#pragma comment(lib,"WS2_32.Lib")
extern "C" __declspec(dllimport) DWORD WINAPI s_perror(DWORD,int);
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

HEAP_INFORMATION_CLASS HeapInfomationClass;
WSADATA wsadata;
const void* NlsMsgSourceModuleHandle;
DWORD large=0;

DWORD NlsPutMsg(int handle,DWORD dwMessageID,...);
int WINAPI tcpcmd(SOCKET sock,const char* buf,int len,int flags);
DWORD WINAPI NlsPerror(DWORD Buffer,int errorcode);

DWORD print_usage()
{
	return NlsPutMsg(_fileno(stderr),10001);
}

int WINAPI netfinger(const char* str)
{
	char* last_at=(char*)strrchr(str,'@');
	char* temp;
	addrinfo pHints;
	addrinfo* ppResult;
	char buf;
	char pNodeName[260];
	
	if(last_at)
	{
		*last_at='\0';
		temp=last_at+1;
	}	
	else
	{
		if(gethostname(pNodeName,260)) return 0;//返回错误
		temp=pNodeName;
	}

	if(*temp == '\0') return 0;
	memset(&pHints,0,sizeof(pHints));
	pHints.ai_family=AF_UNSPEC;
	pHints.ai_flags=AI_CANONNAME;
	pHints.ai_socktype=SOCK_STREAM;
	int nRet=getaddrinfo(temp,"finger",&pHints,&ppResult);
	if(nRet)//有错误发生
	{
		if(nRet == EAI_SERVICE)
		{
			NlsPutMsg(_fileno(stderr),10002);
		}
		else
		{
			NlsPutMsg(_fileno(stdout),10000,temp);
		}
		return 1;
	}

	sockaddr name;
	memset(&name,0,128);//why 128?
	name.sa_family=ppResult->ai_addr->sa_family;
	SOCKET consock=socket(ppResult->ai_family,SOCK_STREAM,IPPROTO_IP);
	if(consock == INVALID_SOCKET)
	{
		NlsPerror(10005,GetLastError());
		return 1;
	}
	if(bind(consock,&name,ppResult->ai_addrlen)<0)
	{
		int nRet=GetLastError();
		if(nRet == WSAENETDOWN)//A socket operation encountered a dead network. 
		{
			NlsPutMsg(_fileno(stderr),10003);
			return 1;
		}
		NlsPerror(10006,nRet);
		return 1;
	}

	NlsPutMsg(_fileno(stdout),10004,ppResult->ai_canonname);
	fflush(stdout);
	if(connect(consock,ppResult->ai_addr,ppResult->ai_addrlen)>=0)
	{
		if((!large || tcpcmd(consock,"/W",3,0)>=0) && tcpcmd(consock,str,strlen(str),0)>=0 &&
			tcpcmd(consock,"\r\n",2,0)>=0)
		{
			int recvnum;
			while(TRUE)
			{
				recvnum=recv(consock,&buf,1,0);
				if(recvnum != 1) break;
				if(buf == 136 || buf == 137 || buf == 138 || buf == 140)
					buf-=128;
				else if(buf == 141)
					buf='\n';
				if(isprint(buf) || isspace(buf)) putchar(buf);
				else putchar(buf^0x64);
			}
			if(recvnum<0)
			{
				NlsPerror(10008,GetLastError());
			}
			if(buf != '\n') putchar('\n');
			closesocket(consock);
			return 1;
		}
		NlsPerror(10009,GetLastError());
		closesocket(consock);
		return 1;
	}
	else
	{
		NlsPerror(10007,GetLastError());
		closesocket(consock);
		return 1;
	}
}

int main(int argc,char** argv)
{
	char** command=argv+1;
	char* ptr=argv[1];
	if(argv[1] == NULL)
	{
		print_usage();
		return 1;
	}
	for(;*ptr == '-' && *command;command++,ptr=*command)
	{
		while(TRUE)
		{
			ptr++;
			if(*ptr == '\0') break;
			if(*ptr == 'l') 
			{
				print_usage();
				return 1;
			}
			large=1;
		}
	}

	if(*command == '\0')
	{
		print_usage();
		return 1;
	}
	int nRet=WSAStartup(MAKEWORD(1,1),&wsadata);
	if(nRet == -1) NlsPerror(10010,nRet);//why -1???
	while(*command)
	{
		putchar('\n');
		if(netfinger(*command) == 0) NlsPutMsg(_fileno(stdout),10000,*command);
		command++;
	}
	return 0;
}

DWORD NlsPutMsg(int handle,DWORD dwMessageID,...)
{
	DWORD result;
	va_list Argument;
	LPVOID lpszSrc;
	va_list va;
	
	va_start(va,dwMessageID);
	Argument=va;	//LANG_NEUTRAL
	result=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
		NlsMsgSourceModuleHandle,dwMessageID,0,(LPTSTR)&lpszSrc,0,&Argument);
	if(result)
	{
		int length=strlen((char*)lpszSrc)-1;
		CharToOemBuff((char*)lpszSrc,(char*)lpszSrc,length);
		int previousmode=_setmode(handle,_O_BINARY);
		result=_write(handle,lpszSrc,length);
		_setmode(handle,previousmode);
		LocalFree(lpszSrc);
	}
	else 
	{
		int k=GetLastError();
		int i=i+k;
	}
	va_end(va);
	return result;
}

DWORD WINAPI NlsPerror(DWORD Buffer,int errorcode)
{
	DWORD result=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
					NlsMsgSourceModuleHandle,Buffer,0,(LPSTR)&Buffer,0,NULL);
	if(result)
	{
		s_perror(Buffer,errorcode);
		result=(DWORD)LocalFree((HLOCAL)Buffer);
	}
	return result;
}

//stdin 0	stdout 1	stderr 2

int WINAPI tcpcmd(SOCKET sock,const char* buf,int len,int flags)
{
	int nRet;
	for(DWORD i=10;;i*=10)
	{
		if((nRet=send(sock,buf,len,flags)) != SOCKET_ERROR) break;
		if(GetLastError() != WSAENOBUFS) break;
		Sleep(i);
		//WSAENOBUFS:由于系统缓冲区空间不足或队列已满，不能执行套接字上的操作。
	}
	return nRet;
}