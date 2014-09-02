#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>

#pragma comment(lib,"lsasrv.lib")
#pragma comment(lib,"ntdll.lib")
#pragma comment(lib,"samsrv.lib")

#define DLL extern "C" __declspec(dllimport)

typedef enum _PROCESS_INFORMATION_CLASS 
{
//	ProcessBasicInformation,
	ProcessQuotaLimits=1,
	ProcessIoCounters,
	ProcessVmCounters,
	ProcessTimes,
	ProcessBasePriority,
	ProcessRaisePriority,
	ProcessDebugPort,
	ProcessExceptionPort,
	ProcessAccessToken,
	ProcessLdtInformation,
	ProcessLdtSize,
	ProcessDefaultHardErrorMode,
	ProcessIoPortHandlers,
	ProcessPooledUsageAndLimits,
	ProcessWorkingSetWatch,
	ProcessUserModeIOPL,
	ProcessEnableAlignmentFaultFixup,
	ProcessPriorityClass,
	ProcessWx86Information,
	ProcessHandleCount,
	ProcessAffinityMask,
	ProcessPriorityBoost,
	MaxProcessInfoClass
} PROCESS_INFORMATION_CLASS, *PPROCESS_INFORMATION_CLASS;

typedef enum _HARDERROR_RESPONSE_OPTION 
{
	OptionAbortRetryIgnore,
	OptionOk,
	OptionOkCancel,
	OptionRetryCancel,
	OptionYesNo,
	OptionYesNoCancel,
	OptionShutdownSystem
} HARDERROR_RESPONSE_OPTION, *PHARDERROR_RESPONSE_OPTION;

typedef enum _HARDERROR_RESPONSE 
{
	ResponseReturnToCaller,
	ResponseNotHandled,
	ResponseAbort,
	ResponseCancel,
	ResponseIgnore,
	ResponseNo,
	ResponseOk,
	ResponseRetry,
	ResponseYes
} HARDERROR_RESPONSE, *PHARDERROR_RESPONSE;

typedef enum _EVENT_TYPE 
{
	NotificationEvent,
	SynchronizationEvent
} EVENT_TYPE;

//samsrv.lib
DLL NTSTATUS WINAPI SamIInitialize();
DLL bool WINAPI SampUsingDsData();

//lsasrv.lib
DLL bool	 WINAPI LsaISetupWasRun();
DLL NTSTATUS WINAPI LsaDsDebugInitialize();
DLL NTSTATUS WINAPI LsapAuOpenSam(DWORD flag);
DLL NTSTATUS WINAPI LsapCheckBootMode();
DLL NTSTATUS WINAPI ServiceInit();
DLL NTSTATUS WINAPI LsapInitLsa();
DLL NTSTATUS WINAPI LsapDsInitializePromoteInterface();
DLL NTSTATUS WINAPI LsapDsInitializeDsStateInfo(NTSTATUS statu);

//ntdll.lib
DLL NTSTATUS WINAPI NtSetInformationProcess(IN HANDLE ProcessHandle,IN PROCESSINFOCLASS ProcessInformationClass,IN PVOID ProcessInformation,IN ULONG ProcessInformationLength);
DLL NTSTATUS WINAPI NtCreateEvent(OUT PHANDLE EventHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectAtributes,IN EVENT_TYPE EventType,IN BOOLEAN InitialState);
DLL NTSTATUS WINAPI NtOpenEvent(OUT PHANDLE EventHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectAttributes);
DLL NTSTATUS WINAPI NtSetEvent(IN HANDLE EventHandle,OUT PLONG PreviousState);
DLL NTSTATUS WINAPI NtRaiseHardError(IN NTSTATUS ErrorStatus,IN ULONG NumberOfParameters,IN PUNICODE_STRING UnicodeStringParameterMask,IN PVOID Parameters,IN HARDERROR_RESPONSE_OPTION ResponseOption,OUT PHARDERROR_RESPONSE);
DLL NTSTATUS WINAPI RtlAdjustPrivilege(IN ULONG Privilege,IN BOOLEAN Enable,IN BOOLEAN CurrentThread,OUT PBOOLEAN Enabled);
DLL NTSTATUS WINAPI NtShutdownSystem();
DLL NTSTATUS WINAPI RtlUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo);


NTSTATUS WINAPI LsapNotifyInitializationFinish(NTSTATUS statu)
{
	NTSTATUS result;
	HANDLE phandle=NULL;
	BOOLEAN Enabled;
	OBJECT_ATTRIBUTES objattrib;
	UNICODE_STRING ustring;

	if(statu >= 0)
	{
		RtlInitUnicodeString(&ustring,L"\\SAM_SERVICE_STARTED");
		objattrib.ObjectName=&ustring;
		objattrib.Length=sizeof(OBJECT_ATTRIBUTES);
		objattrib.RootDirectory=NULL;
		objattrib.Attributes=0;
		objattrib.SecurityDescriptor=NULL;
		objattrib.SecurityQualityOfService=NULL;

		result=NtCreateEvent(&phandle,SYNCHRONIZE | EVENT_MODIFY_STATE,&objattrib,NotificationEvent,FALSE);
		if(!NT_SUCCESS(result))
		{
			if(result == STATUS_OBJECT_NAME_EXISTS || result == STATUS_OBJECT_NAME_COLLISION)
				result=NtOpenEvent(&phandle,SYNCHRONIZE | EVENT_MODIFY_STATE,&objattrib);
		}
		if(NT_SUCCESS(result))
		{
			result=NtSetEvent(phandle,NULL);
			if(!NT_SUCCESS(result)) result=NtClose(phandle);
		}
		return result;
	}
	if(ImpersonateSelf(SecurityImpersonation))
	{
		HANDLE hObject;
		if(OpenThreadToken(GetCurrentThread(),TOKEN_QUERY,TRUE,&hObject) == FALSE)
			RevertToSelf();
		else
		{
			Enabled=TRUE;
			PSID pSid=NULL;
			BOOL IsMember;
			SID_IDENTIFIER_AUTHORITY SIA={SECURITY_WORLD_RID,SECURITY_WORLD_RID,SECURITY_WORLD_RID,SECURITY_WORLD_RID,SECURITY_WORLD_RID,SECURITY_LOGON_IDS_RID};
			if(AllocateAndInitializeSid(&SIA,1,SECURITY_LOCAL_SYSTEM_RID,SECURITY_WORLD_RID,SECURITY_WORLD_RID,SECURITY_WORLD_RID,SECURITY_WORLD_RID,
				SECURITY_WORLD_RID,SECURITY_WORLD_RID,SECURITY_WORLD_RID,&pSid))
			{
				if(CheckTokenMembership(hObject,pSid,&IsMember))
					Enabled=IsMember;
				if(pSid)
					FreeSid(pSid);
			}
			CloseHandle(hObject);
			result=RevertToSelf();
			if(Enabled == FALSE)
				return result;
		}
	}

	HARDERROR_RESPONSE response;
	DWORD parameter=0x10010;//messagemap ID?
	NTSTATUS nRet=NtRaiseHardError(statu | STATE_SYSTEM_ALERT_HIGH,1,NULL,&parameter,OptionOk,&response);
	if(LsaISetupWasRun())
	{
		RtlInitUnicodeString(&ustring,L"\\SETUP_FAILED");
		objattrib.ObjectName=&ustring;
		objattrib.Length=sizeof(UNICODE_STRING);
		objattrib.RootDirectory=NULL;
		objattrib.Attributes=0;
		objattrib.SecurityDescriptor=NULL;
		objattrib.SecurityQualityOfService=NULL;
		result=NtOpenEvent(&phandle,SYNCHRONIZE | EVENT_MODIFY_STATE,&objattrib);
		if(NT_SUCCESS(statu))
			result=NtSetEvent(phandle,NULL);
	}
	else if(NT_SUCCESS(nRet))
	{
#define SE_SHUTDOWN_PRIVILEGE               (19L)
		RtlAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE,TRUE,FALSE,&Enabled);	
	}
	return result;
}

LONG WINAPI LsaTopLevelExceptionHandler(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	return RtlUnhandledExceptionFilter(ExceptionInfo);
}

void main()
{
	SetErrorMode(SEM_FAILCRITICALERRORS);
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)LsaTopLevelExceptionHandler);
#define FOREGROUND_BASE_PRIORITY	9
#define NtCurrentProcess()			((HANDLE)(LONG_PTR)-1)	
	DWORD flag=FOREGROUND_BASE_PRIORITY;
	NTSTATUS statu=NtSetInformationProcess(NtCurrentProcess(),(PROCESSINFOCLASS)ProcessBasePriority,&flag,sizeof(flag));
	if(NT_SUCCESS(statu))
	{
		statu=LsapCheckBootMode();
		if(NT_SUCCESS(statu))
		{
			statu=ServiceInit();
			if(NT_SUCCESS(statu))
			{
				statu=LsapInitLsa();
				if(NT_SUCCESS(statu))
				{
					statu=SamIInitialize();
					if(NT_SUCCESS(statu))
					{
						statu=LsapDsInitializePromoteInterface();
						if(NT_SUCCESS(statu))
						{
							statu=LsapAuOpenSam(1);
							if(NT_SUCCESS(statu))
							{
								statu=LsapDsInitializeDsStateInfo(SampUsingDsData()?2:1);
							}
						}
					}
				}
			}
		}
	}
	LsapNotifyInitializationFinish(statu);
	ExitThread(statu);
}