// 服务控制管理器是一个RPC 服务器，它显露了一组应用编程接口，程序员可以方便的编写程序来配置
// 服务和控制远程服务器中服务程序。
// 服务程序通常编写成控制台类型的应用程序，总的来说，一个遵守服务控制管理程序接口要求的程序
// 
// 包含下面三个函数：
// 1。服务程序主函数（main）：调用系统函数 StartServiceCtrlDispatcher 连接程序主线程到服务控制管理程序。
// 2。服务入口点函数（ServiceMain）：执行服务初始化任务，同时执行多个服务的服务进程有多个服务入口函数。
// 3。控制服务处理程序函数（Handler）：在服务程序收到控制请求时由控制分发线程引用。（此处是Service_Ctrl）。
// 另外在系统运行此服务之前需要安装登记服务程序：installService 函数。删除服务程序则需要先删除服务安装登记：removeService 函数。
#include <windows.h>
#include <winsvc.h>

SERVICE_STATUS_HANDLE BootVerificationStatusHandle=NULL;
SERVICE_STATUS BootVerificationStatus={0,0,0,0,0,0,0};
HANDLE BootVerificationDoneEvnet=NULL;

void WINAPI HandlerProc(DWORD dwControl)//控制服务处理程序函数
{
	if(dwControl == SERVICE_CONTROL_STOP)
	{
		BootVerificationStatus.dwWin32ExitCode=0;
		BootVerificationStatus.dwCurrentState=SERVICE_STOP_PENDING;//The service is stopping.
		SetEvent(BootVerificationDoneEvnet);//使事件产生信号
	}
	if(!SetServiceStatus(BootVerificationStatusHandle,&BootVerificationStatus))
		GetLastError();
}

void WINAPI ServiceProc(DWORD   dwNumServicesArgs,LPWSTR  *lpServiceArgVectors)//服务入口点函数
{
	SERVICE_STATUS ServiceStatus;

	BootVerificationDoneEvnet=CreateEvent(NULL,TRUE,FALSE,NULL);//初始化无信号事件
	BootVerificationStatus.dwServiceType=SERVICE_WIN32;
	BootVerificationStatus.dwCurrentState=SERVICE_RUNNING;
	BootVerificationStatus.dwControlsAccepted=SERVICE_ACCEPT_STOP;
	//The service can be stopped. This control code allows the service to receive SERVICE_CONTROL_STOP notifications.
	BootVerificationStatus.dwWin32ExitCode=0;
	BootVerificationStatus.dwServiceSpecificExitCode=0;
	BootVerificationStatus.dwCheckPoint=0;
	// This value is not valid and should be zero when the service does not have a start, stop, pause, or continue operation pending
	BootVerificationStatus.dwWaitHint=0;
	BootVerificationStatusHandle=RegisterServiceCtrlHandlerW(L"BootVerification",HandlerProc);//注册控制服务处理程序函数
	if(!SetServiceStatus(BootVerificationStatusHandle,&BootVerificationStatus))//设置服务状态
		GetLastError();
	NotifyBootConfigStatus(TRUE);//this function reports the boot status to the service control manager
	SC_HANDLE scm=OpenSCManagerW(NULL,NULL,SC_MANAGER_CONNECT);//Enables connecting to the service control manager.
	if(scm != NULL)
	{
		SC_HANDLE service=OpenServiceW(scm,L"BootVerification",SERVICE_STOP);
		if(service != NULL && ControlService(service,SERVICE_CONTROL_STOP,&ServiceStatus) != NULL/*sends a control code to a service.*/ )
		{
			WaitForSingleObject(BootVerificationDoneEvnet,INFINITE);//等待事件发出信号
			BootVerificationStatus.dwWin32ExitCode=0;
			BootVerificationStatus.dwCurrentState=SERVICE_STOPPED;
			if(!SetServiceStatus(BootVerificationStatusHandle,&BootVerificationStatus))
				GetLastError();
			ExitThread(0);
		}
	}

	BootVerificationStatus.dwWin32ExitCode=GetLastError();
	SetServiceStatus(BootVerificationStatusHandle,&BootVerificationStatus);
	ExitProcess(0);
}

void main()//服务程序主函数
{
	SERVICE_TABLE_ENTRYW ServiceStartTable[2];

	ServiceStartTable[1].lpServiceName=NULL;//表示TABLE_ENTRY结束
	ServiceStartTable[1].lpServiceProc=NULL;//表示TABLE_ENTRY结束
	ServiceStartTable[0].lpServiceName=L"BootVerification";//服务名称
	ServiceStartTable[0].lpServiceProc=ServiceProc;//服务入口函数
	int nret=StartServiceCtrlDispatcherW(ServiceStartTable);//向service control manager注册此服务
	nret=GetLastError();
	ExitProcess(0);
}
