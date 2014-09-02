#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <iphlpapi.h>
#include <icmpapi.h>
//#pragma comment(lib,"MsWSock.Lib")
#pragma comment(lib,"WS2_32.Lib")
#pragma comment(lib,"Iphlpapi.lib")

struct ICMP_ECHO_REPLY_EX 
{
	union
	{
		ICMP_ECHO_REPLY	v4;
		IPV6_ADDRESS_EX v6;
	}IER;

	NTSTATUS		Status;
	ULONG			Information;
	BYTE			data[96];
};

const void* NlsMsgSourceModuleHandle;
WSADATA WsaData;
BYTE RequestData[64];
ICMP_ECHO_REPLY_EX ReplyBuffer;
DWORD ErrorTable[40]=
{ 
	IP_BUF_TOO_SMALL,10015,
	IP_DEST_NET_UNREACHABLE,10016,
	IP_DEST_HOST_UNREACHABLE,10017,
	IP_DEST_PROT_UNREACHABLE,10018,
	IP_DEST_PORT_UNREACHABLE,10019,
	IP_NO_RESOURCES,10020,
	IP_BAD_OPTION ,10021,
	IP_HW_ERROR ,10022,
	IP_PACKET_TOO_BIG,10023,
	IP_REQ_TIMED_OUT,10024,
	IP_BAD_REQ,10025,
	IP_BAD_ROUTE ,10026,
	IP_TTL_EXPIRED_TRANSIT,10027,
	IP_TTL_EXPIRED_REASSEM,10028,
	IP_PARAM_PROBLEM ,10029,
	IP_SOURCE_QUENCH,10030,
	IP_OPTION_TOO_BIG,10031,
	IP_BAD_DESTINATION,10032,
	IP_NEGOTIATING_IPSEC,10046,
	IP_GENERAL_FAILURE,10033
};


void NlsPutMsg(int handle,DWORD dwMessageID,...)
{
	DWORD nRet;
	va_list Argument;
	LPVOID lpszSrc;
	va_list va;
	
	va_start(va,dwMessageID);
	Argument=va;	//LANG_NEUTRAL
	nRet=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
		0,dwMessageID,0,(LPTSTR)&lpszSrc,0,&Argument);
	if(nRet)
	{
		int length=strlen((char*)lpszSrc);
		CharToOemBuff((char*)lpszSrc,(char*)lpszSrc,length);
		int previousmode=_setmode(handle,_O_BINARY);
		nRet=_write(handle,lpszSrc,length);
		_setmode(handle,previousmode);
		LocalFree(lpszSrc);
	}
	int i=GetLastError();

	va_end(va);
}

void WINAPI print_addr(const SOCKADDR *pSockaddr,socklen_t SockaddrLength,char locchar)
{
	bool IsSuccess=FALSE;
	char pNodeBuffer[1024];

	if(locchar && !getnameinfo(pSockaddr,SockaddrLength,pNodeBuffer,sizeof(pNodeBuffer)+1,NULL,0,NI_NAMEREQD))
	{//#define NI_NAMEREQD     0x04  /* Error if the host's name not in DNS */
		IsSuccess=TRUE;		
		NlsPutMsg(_fileno(stdout),10042,&pNodeBuffer);
	}
	if(getnameinfo(pSockaddr,SockaddrLength,pNodeBuffer,sizeof(pNodeBuffer)+1,NULL,0,NI_NUMERICHOST))
	{//#define NI_NUMERICHOST  0x02  /* Return numeric form of the host's address */
		NlsPutMsg(_fileno(stdout),10020);
		_exit(1);
	}
	if(IsSuccess) NlsPutMsg(_fileno(stdout),10000,pNodeBuffer);
	else NlsPutMsg(_fileno(stdout),10045,pNodeBuffer);	
} 

void WINAPI print_ip_addr(int address,char locchar)
{
	sockaddr_in Sockaddr;
	memset(&Sockaddr,0,sizeof(sockaddr_in));
	Sockaddr.sin_addr.s_addr=address;
	Sockaddr.sin_family=AF_INET;
	print_addr((SOCKADDR*)&Sockaddr,sizeof(sockaddr_in),locchar);
}

void WINAPI print_ipv6_addr(in6_addr* paddress,char locchar)
{
	sockaddr_in6 Sockaddr;
	memset(&Sockaddr,0,sizeof(Sockaddr));
	Sockaddr.sin6_family=AF_INET6;
	Sockaddr.sin6_addr=*paddress;
	print_addr((SOCKADDR*)&Sockaddr,sizeof(sockaddr_in6),locchar);
}

void WINAPI print_time(ULONG str)
{
	if(str) NlsPutMsg(_fileno(stdout),10001,str);
	else NlsPutMsg(_fileno(stdout),10002); 
}

bool WINAPI param(DWORD* data,char** argv,int argc,int pos,DWORD smallnum,DWORD largenum)
{
	if(pos == argc-1)
	{
		NlsPutMsg(_fileno(stdout),10036,argv[pos]);
		return FALSE;
	}

	char** ptr=argv+pos;
	unsigned long returnnum=strtoul(ptr[1],(char**)&argv,0);
	// If base is 0, the initial characters of the string pointed to by nptr are used to determine the base. 
	//由此ptr[1][0]为进制数
	if(returnnum < smallnum || returnnum >largenum)
	{
		NlsPutMsg(_fileno(stdout),10037,ptr[0]);
		return FALSE;
	}
	*data = returnnum;
	return TRUE;
}

bool WINAPI ResolveTarget(int family,char* pNodeName,SOCKADDR* psockaddr,size_t* pbufferlen,char* pNodeBuffer,DWORD NodeBufferSize,bool flag)
{
	ADDRINFO pHints;
	PADDRINFOA info;

	*pNodeBuffer='\0';
	memset(&pHints,0,sizeof(pHints));
	pHints.ai_family=family;
	pHints.ai_flags=AI_NUMERICHOST;
	//#define AI_NUMERICHOST              0x00000004  // Nodename must be a numeric address string
	if(getaddrinfo(pNodeName,NULL,&pHints,&info) == 0)
	{//success
		*pbufferlen = info->ai_addrlen;
		memcpy(psockaddr,info->ai_addr,info->ai_addrlen);
		if(flag == FALSE)
		{
			getnameinfo(info->ai_addr,info->ai_addrlen,pNodeBuffer,NodeBufferSize,NULL,0,NI_NAMEREQD);
			//#define NI_NAMEREQD     0x04  /* Error if the host's name not in DNS */
		}
	}
	else
	{
		pHints.ai_flags=AI_CANONNAME;
		//#define AI_CANONNAME                0x00000002  // Return canonical name in first ai_canonname
		if(getaddrinfo(pNodeName,NULL,&pHints,&info)) return FALSE;
		*pbufferlen=info->ai_addrlen;
		memcpy(psockaddr,info->ai_addr,info->ai_addrlen);
		char* canonname=info->ai_canonname;
		if(canonname == NULL)
			canonname=pNodeName;
		char* ptr=pNodeBuffer;
		do 
		{
			*ptr++ = *canonname++;
		} 
		while (*(canonname-1));
	}
	freeaddrinfo(info);
	return TRUE;
}

int WINAPI GetSource(int ppResult,PCSTR pNodeName,void* newinfo)
{
	ADDRINFO pHints;
	memset(&pHints,0,sizeof(ADDRINFO));
	pHints.ai_family=ppResult;//????why
	pHints.ai_flags=AI_PASSIVE;//#define AI_PASSIVE   0x00000001  // Socket address will be used in bind() call
	
	PADDRINFOA adaptinfo;
	if(getaddrinfo(pNodeName,NULL,&pHints,&adaptinfo)) return 0;
	else
	{
		memcpy(newinfo,adaptinfo->ai_addr,adaptinfo->ai_addrlen);
		return 1;
	}
}

bool SetFamily(int* pfamily,int target,char* str)
{
	if(*pfamily && *pfamily != target)
	{
		NlsPutMsg(_fileno(stdout),10040,str);
		return FALSE;
	}
	*pfamily=target;
	return TRUE;
}

int main(int argc,char** argv)
{

	DWORD MaxTtl=30,Timeout=4000;

	bool flag;
	size_t bufferlen;
	IP_OPTION_INFORMATION RequestOptions;
	bool isnoparam;//是否命令行后面直接跟着地址
	int family;
	SOCKADDR_IN vOutBuffer[8],vInBuffer[8];
	unsigned long addr;
	char** temp;
	BYTE optionsize;
	char **temp2;

	UCHAR pNodeBuffer[1025];
	UCHAR optiondata[40];
	UCHAR pNodeBufferother[65];

	optionsize=0;
	family=AF_UNSPEC;
	flag=TRUE;
	isnoparam=FALSE;

	SetThreadUILanguage(0);
	if(WSAStartup(MAKEWORD(2,0),&WsaData))
	{
		NlsPutMsg(_fileno(stdout),10034,GetLastError());
		return 1;
	}
	memset(vOutBuffer,0,sizeof(vOutBuffer));
	RequestOptions.Tos=NULL;
	RequestOptions.Flags=0;
	RequestOptions.OptionsSize=0;
	RequestOptions.Ttl=1;
	RequestOptions.OptionsData=optiondata;

	if(argc<2)
	{
		NlsPutMsg(_fileno(stdout),10003,argv[0]);
		WSACleanup();
		return 1;
	}
	int i=1;
	do
	{
		char* curpos=argv[i];
		temp=argv+i;
		if(curpos[0] != '-' && curpos[0] != '/')
		{
			isnoparam=TRUE;
			if(FALSE == ResolveTarget(family,curpos,(SOCKADDR*)vInBuffer,&bufferlen,(char*)pNodeBuffer,1025,flag))
			{
				NlsPutMsg(_fileno(stdout),10004,curpos);
				WSACleanup();
				return 1;
			}
			continue;
		}

		switch(curpos[1])
		{
			case 'd'://不将地址解析成主机名
				{
					flag=FALSE;
					continue;
				}

			case 'h'://搜索目标的最大跃点数
				{
					if(param(&MaxTtl,argv,argc,i++,1,255)) continue;
					WSACleanup();
					return 1;
				}

			case 'j'://与主机列表一起的松散源路由(仅适用于 IPv4)
				{
					if(FALSE == SetFamily(&family,AF_INET,curpos)) 
					{
						WSACleanup();
						return 1;
					}
					optionsize=RequestOptions.OptionsSize;
					if(RequestOptions.OptionsSize +7 >40)
					{
						NlsPutMsg(_fileno(stdout),10039);
						WSACleanup();
						return 1;
					}

					RequestOptions.OptionsData[RequestOptions.OptionsSize]=IP_OPT_LSRR;
					//#define IP_OPT_LSRR     0x83       // Loose source route 松散路由
					RequestOptions.OptionsData[RequestOptions.OptionsSize+1]=3;//+0 +1 +2
					RequestOptions.OptionsData[RequestOptions.OptionsSize+2]=4;//addr size
					RequestOptions.OptionsSize+=3;

					if(i<argc-2)
					{
						temp2=temp;
						while(i++ < argc-2)
						{
							if(temp[1][0] == '-') break;
							if(RequestOptions.OptionsSize+7 >40)
							{
								NlsPutMsg(_fileno(stdout),10039);
								WSACleanup();
								return 1;
							}
							temp2++;
							addr=inet_addr(temp2[0]);
							if(addr == INADDR_NONE)
							{
								NlsPutMsg(_fileno(stdout),10038,temp2[0]);
								WSACleanup();
								return 1;
							}
							*(DWORD*)(RequestOptions.OptionsData+RequestOptions.OptionsSize)=addr;
							RequestOptions.OptionsSize+=4;
						}
					}
					RequestOptions.OptionsSize+=4;
					optionsize=RequestOptions.OptionsSize;
					continue;
				}

			case 'w'://等待每个回复的超时时间(以毫秒为单位)
				{
					if(param(&Timeout,argv,argc,i++,1,INFINITE)) continue;
					else
					{
						WSACleanup();
						return 1;
					}
				}

			case 'S'://要使用的源地址(仅适用于 IPv6)
				{
					if(FALSE == SetFamily(&family,AF_INET6,curpos))
					{
						WSACleanup();
						return 1;
					}
					i++;
					if(0 == GetSource(family,argv[1],vOutBuffer))//failure
					{
						NlsPutMsg(_fileno(stdout),10038,curpos[1]);
						WSACleanup();
						return 1;
					}
					continue;
				}

			case '4'://强制使用 IPv4
				{
					if(FALSE == SetFamily(&family,AF_INET6,curpos))
					{
						WSACleanup();
						return 1;
					}
					else continue;
				}

			case '6'://强制使用 IPv6
				{
					if(FALSE == SetFamily(&family,AF_INET6,curpos))
					{
						WSACleanup();
						return 1;
					}
					else continue;
				}

			case '?':
				{
					NlsPutMsg(_fileno(stdout),10003,argv[0]);
					WSACleanup();
					return 1;
				}

			case 'R'://跟踪往返行程路径(仅适用于 IPv6)
				{
					if(FALSE == SetFamily(&family,AF_INET6,curpos))
					{
						WSACleanup();
						return 1;
					}
					RequestOptions.Flags |=IP_FLAG_REVERSE;
					continue;
				}

			default:
				{
					NlsPutMsg(_fileno(stdout),10040,argv[i]);
					NlsPutMsg(_fileno(stdout),10003);
					WSACleanup();
					return 1;
				}
		}
	}
	while(++i<argc);

	if(isnoparam == FALSE)
	{
		NlsPutMsg(1,10041);
		NlsPutMsg(1,10003);
		WSACleanup();
		return 1;
	}

	HANDLE icmpfile;
	DWORD serv;
	IPAddr addrv4;
	USHORT addrv6[8];

	if(vInBuffer[0].sin_family	 == AF_INET)
	{
		if(optionsize)
			*(DWORD*)(RequestOptions.OptionsData+optionsize)=vInBuffer[0].sin_addr.s_addr;
		icmpfile=IcmpCreateFile();
		serv=0;
	}
	else
	{
		serv=0;
		if(vOutBuffer[0].sin_family == AF_UNSPEC)
		{
			SOCKET sock=socket(vInBuffer[0].sin_family,0,IPPROTO_IP);
			if(sock == INVALID_SOCKET)
			{
				NlsPutMsg(_fileno(stdout),10034,WSAGetLastError());
				_exit(1);
			}
			DWORD chBytesReturned;
			WSAIoctl(sock,SIO_ROUTING_INTERFACE_QUERY,vInBuffer,sizeof(vInBuffer),vOutBuffer,sizeof(vOutBuffer),
				&chBytesReturned,NULL,NULL);

		}
		icmpfile=Icmp6CreateFile();
	}
	if(icmpfile == INVALID_HANDLE_VALUE)
	{
		NlsPutMsg(_fileno(stdout),10005,GetLastError());
		WSACleanup();
		return 1;
	}
	getnameinfo((SOCKADDR*)vInBuffer,bufferlen,(PCHAR)pNodeBufferother,65,(PCHAR)serv,serv,NI_NUMERICHOST);
	if(pNodeBuffer[0])
		NlsPutMsg(_fileno(stdout),10006,pNodeBuffer,pNodeBufferother,MaxTtl);
	else
		NlsPutMsg(_fileno(stdout),10035,pNodeBufferother,MaxTtl);
	if(RequestOptions.Ttl > MaxTtl || RequestOptions.Ttl == 0)
	{
		NlsPutMsg(_fileno(stdout),10011);
		IcmpCloseHandle(icmpfile);
		WSACleanup();
		return 0;
	}
	NlsPutMsg(1,10007,RequestOptions.Ttl);
	i=0;
	optionsize=0;
	DWORD nRet;
	
	while(RequestOptions.Ttl)
	{
		if(vInBuffer[0].sin_family == AF_INET)
		{
			if(0 == IcmpSendEcho2(icmpfile,NULL,NULL,NULL,vInBuffer[0].sin_addr.s_addr,
				RequestData,64,&RequestOptions,(LPVOID)&ReplyBuffer,sizeof(ReplyBuffer),Timeout))//Timeout:timeout
			{
				nRet=GetLastError();
				if(nRet == IP_REQ_TIMED_OUT)
				{
					NlsPutMsg(_fileno(stdout),10043);
					if(i == 2)
					{
						if(optionsize)
						{
							print_ip_addr(addrv4,flag);
							NlsPutMsg(_fileno(stdout),10044);
						}
						else
						{
							NlsPutMsg(_fileno(stdout),10024);
						}
					}
				}
				else if(nRet >= IP_STATUS_BASE)
				{
					DWORD k;
					for(k=0;;k+=2)
					{
						if(ErrorTable[k] != nRet && ErrorTable[k] != IP_GENERAL_FAILURE) 
							break;
					}
					NlsPutMsg(_fileno(stdout),ErrorTable[k+1]);
					break;
				}
				else
				{
					NlsPutMsg(_fileno(stdout),nRet);
					break;
				}
			}
			else
			{
				if(ReplyBuffer.IER.v4.Status == IP_SUCCESS || ReplyBuffer.IER.v4.Status == IP_TTL_EXPIRED_TRANSIT)
				{
					print_time(ReplyBuffer.IER.v4.RoundTripTime);
					if(i != 2)
					{
						addrv4=ReplyBuffer.IER.v4.Address;
						optionsize=1;
					}
					else
					{
						print_ip_addr(ReplyBuffer.IER.v4.Address,flag);/////////////
						NlsPutMsg(_fileno(stdout),10044);
						if(ReplyBuffer.IER.v4.Status == IP_SUCCESS)
						{
							break;
						}
						else
						{
							if(ReplyBuffer.IER.v4.RoundTripTime < 1000)
								Sleep(1000-ReplyBuffer.IER.v4.RoundTripTime);
						}
					}
				}
				else if(ReplyBuffer.IER.v4.Status >= IP_STATUS_BASE)
				{
					print_ipv6_addr((IN6_ADDR*)ReplyBuffer.IER.v6.sin6_addr,flag);//////////
					NlsPutMsg(_fileno(stdout),10009);
					DWORD k;
					for(k=0;;k+=2)
					{
						if(ErrorTable[k] != nRet && ErrorTable[k] != IP_GENERAL_FAILURE) 
							break;
					}
					NlsPutMsg(_fileno(stdout),ErrorTable[k+1]);
					break;
				}
				else
				{
					NlsPutMsg(_fileno(stdout),10010,ReplyBuffer.IER.v4.Status);
					break;
				}
			}
		}
		
		else if(0 == Icmp6SendEcho2(icmpfile,NULL,NULL,NULL,(sockaddr_in6*)vOutBuffer,(sockaddr_in6*)vInBuffer,
			RequestData,64,&RequestOptions,(void*)&ReplyBuffer,sizeof(ReplyBuffer),Timeout))
		{
			nRet=GetLastError();
			if(nRet == IP_REQ_TIMED_OUT)
			{
				NlsPutMsg(_fileno(stdout),10043);
				if(i == 2)
				{
					if(optionsize)
					{
						print_ipv6_addr((IN6_ADDR*)ReplyBuffer.IER.v6.sin6_addr,flag);
						NlsPutMsg(_fileno(stdout),10044);
					}
					else
						NlsPutMsg(_fileno(stdout),10024);
				}
			}
			else if(nRet < IP_STATUS_BASE)
			{
				NlsPutMsg(_fileno(stdout),10010,nRet);
				break;
			}
			else
			{
				DWORD k;
				for(k=0;;k+=2)
				{
					if(ErrorTable[k] != nRet && ErrorTable[k] != IP_GENERAL_FAILURE) 
						break;
				}
				NlsPutMsg(_fileno(stdout),ErrorTable[k+1]);
				break;
			}
		}
		else
		{
			nRet=ReplyBuffer.Status;
			if(nRet == IP_SUCCESS || nRet == IP_TTL_EXPIRED_TRANSIT)
			{
				print_time(ReplyBuffer.Information);
				if(i != 2)
				{
					memcpy(addrv6,ReplyBuffer.IER.v6.sin6_addr,16);
					optionsize=1;
				}
				else
				{
					print_ipv6_addr((IN6_ADDR*)ReplyBuffer.IER.v6.sin6_addr,flag);
					NlsPutMsg(_fileno(stdout),10044);
					if(nRet == IP_SUCCESS)
					{
						break;
					}
					else
					{
						if(ReplyBuffer.IER.v4.RoundTripTime < 1000)
							Sleep(1000-ReplyBuffer.IER.v4.RoundTripTime);
					}
				}
			}
			else if(nRet < IP_STATUS_BASE)
			{
				NlsPutMsg(_fileno(stdout),10010,nRet);
				break;
			}
			else
			{
				print_ipv6_addr((IN6_ADDR*)ReplyBuffer.IER.v6.sin6_addr,flag);
				NlsPutMsg(_fileno(stdout),10009);
				DWORD k;
				for(k=0;;k+=2)
				{
					if(ErrorTable[k] != nRet && ErrorTable[k] != IP_GENERAL_FAILURE) 
						break;
				}
				NlsPutMsg(_fileno(stdout),ErrorTable[k+1]);
				break;
			}
		}

		i++;
		if(i >= 3)
		{
			RequestOptions.Ttl++;
			if(RequestOptions.Ttl <= MaxTtl)
			{
				NlsPutMsg(_fileno(stdout),10007,RequestOptions.Ttl);
				i=0;
				optionsize=0;
			}
			else
				break;
		}
	}

	NlsPutMsg(_fileno(stdout),10011);
	IcmpCloseHandle(icmpfile);
	WSACleanup();
	return 0;
}
