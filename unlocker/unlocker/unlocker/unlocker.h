#include <windows.h>

//File operation code
#define	FILENONE	0
#define	FILEDELETE	1
#define	FILERENAME	2
#define	FILEMOVE	3

//Operating System ID
#define WINDOWS95		23
#define WINDOWSVISTA	25
#define WINDOWS2000		26
#define WINDOWSXPLATER	28


typedef NTSTATUS (WINAPI* ZWQUERYSYSTEMINFORMATION)(
							IN				SYSTEM_INFORMATION_CLASS	SystemInformationClass,
							IN	OUT			PVOID						SystemInformation,
							IN				ULONG						SystemInformationLength,
							OUT	OPTIONAL	PULONG						ReturnLength);

typedef NTSTATUS (WINAPI* ZWQUERYINFORMATIONPROCESS)(
							IN				HANDLE						ProcessHandle,
							IN				PROCESSINFOCLASS			ProcessInformationClass,
							OUT				PVOID						ProcessInformation,
							IN				ULONG						ProcessInformationLength,
							OUT	OPTIONAL	PULONG						ReturnLength);

typedef NTSTATUS (WINAPI* ZWQUERYOBJECT)(
							IN	OPTIONAL	HANDLE						Handle,
							IN				OBJECT_INFORMATION_CLASS	ObjectInformationClass,
							OUT	OPTIONAL	PVOID 						ObjectInformation,
							IN				ULONG						ObjectInformationLength,
							OUT	OPTIONAL	PULONG 						ReturnLength);

typedef NTSTATUS (WINAPI* ZWDELETEFILE)(IN	POBJECT_ATTRIBUTES 			ObjectAttributes);

typedef VOID (WINAPI* RTLINITUNICODESTRING)(
							OUT				PUNICODE_STRING				DestinationString,
							INT	OPTIONAL	PCWSTR						SourceString);

typedef NTSTATUS (WINAPI* RTLADJUSTPRIVILEGE)(
							IN				ULONG						Privilege,
							IN				BOOLEAN						Enable,
							IN				BOOLEAN						CurrentThread,
							OUT				PBOOLEAN					Enabled);	  

typedef NTSTATUS (WINAPI* NTLOADDRIVER)(IN	PUNICODE_STRING				DriverServiceName);

typedef NTSTATUS (WINAPI* NTUNLOADDRIVER)(IN	PUNICODE_STRING			DriverServiceName);

typedef HANDLE (WINAPI* MYCREATEFILE)(LPCWSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile);

typedef DWORD (WINAPI* MYSETFILEPOINTER)(HANDLE hFile,LONG IDistanceToMove,PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod);

typedef BOOL (WINAPI* MYREADFILE)(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead,LPOVERLAPPED lpOverlapped);

typedef BOOL (WINAPI* MYWRITEFILE)(HANDLE hFile,LPCVOID lpBuffer,DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,LPOVERLAPPED lpOverlapped);

typedef BOOL (WINAPI* MYCLOSEHANDLE)(HANDLE hObject);

typedef HGLOBAL (WINAPI* MYGLOBALALLOC)(UINT uFlags,DWORD dwBytes);

typedef HGLOBAL (WINAPI* MYGLOBALFREE)(HGLOBAL hMem);

typedef DWORD (WINAPI* MYGETFILESIZE)(HANDLE hFile,LPDWORD lpFileSizeHigh);

typedef VOID (WINAPI* MYSLEEP)(DWORD dwMilliseconds);

typedef BOOL (WINAPI* MYGETOVERLAPPEDRESULT)(HANDLE hFile,LPOVERLAPPED lpOverlapped,LPDWORD lpNumberOfBytesTransferred,BOOL bWait);

typedef DWORD (WINAPI* MYGETLASTERROR)(VOID);

typedef BOOL (WINAPI* MYFREELIBRARY)(HMODULE hModule);

typedef HMODULE (WINAPI* MYGETMODULEHANDLE)(LPCTSTR lpMoculeName);

typedef BOOL (WINAPI* MYCLOSEHANDLE)(HANDLE hObject);

typedef FARPROC (WINAPI* MYGETPROCADDRESS)(HMODULE hModule,LPCSTR lpProcName);

class OperateFileClass//sizeof=16
{
public:
	bool	IsFiletoUnlockUnicode;
	bool	IsListOfFiletoUnlock;
	bool	IsOutputLog;
	bool	IsNotShowGUItoUnlock;
	int		Operation;//Delete=1;Rename=2;Move=3
	LPWSTR	Command;
	LPWSTR	ObjectName;

public:
	OperateFileClass* Initial();
	int GetOperateFileParamFromCmdLine();
};

template<class T>
class GeneralClass//sizeof=16
{
public:
	HGLOBAL			Handle;
	T*				Buf;
	SIZE_T			MaxSize;
	SIZE_T			CurPos;

public:
	GerernalClass();
	void Construct(T* data);
	void Destruct();
};

class CoreAPIArray//sizeof=32
{
public:
	ZWQUERYSYSTEMINFORMATION	ZwQuerySystemInformation;
	ZWQUERYINFORMATIONPROCESS	ZwQueryInformationProcess;
	ZWQUERYOBJECT				ZwQueryObject;
	ZWDELETEFILE				ZwDeleteFile;
	RTLINITUNICODESTRING		RtlInitUnicodeString;
	RTLADJUSTPRIVILEGE			RtlAdjustPrivilege;
	NTLOADDRIVER				NtLoadDriver;
	NTUNLOADDRIVER				NtUnloadDriver;

public:
	CoreAPIArray* CoreAPIArrayInitial();
	BOOL GetCoreAPIArray();	
};

typedef struct _UNICODE_STRING//sizeof=8
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct OBJECT_NAME_INFORMATION//sizeof=1024
{
	UNICODE_STRING	Name;
	WCHAR			NameBuffer[(1024-sizeof(UNICODE_STRING))/sizeof(WCHAR)];//508
}OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

typedef enum _OBJECT_INFORMATION_CLASS 
{
    ObjectBasicInformation,
    ObjectNameInformation,
    ObjectTypeInformation,
    ObjectAllInformation,
    ObjectDataInformation
} OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;

struct InjectStruct1//sizeof=3088
{
	MYFREELIBRARY MyFreeLibrary;
	MYGETMODULEHANDLE MyGetModuleHanle;
	MYCLOSEHANDLE MyCloseHandle;
	MYGETPROCADDRESS MyGetProcAddress;
	TCHAR AData[1024];//DLL名
	WCHAR UData[1024];//函数名
}

struct InjectStruct2//sizeof=2096
{
	MYCREATEFILE MyCreateFile;
	MYSETFILEPOINTER MySetFilePointer;
	MYREADFILE MyReadFile;
	MYWRITEFILE MyWriteFile;
	MYCLOSEHANDLE MyCloseHandle;
	MYGLOBALALLOC MyGlobalAlloc;
	MYGLOBALFREE MyGlobalFree;
	MYGETFILESIZE MyGetFileSize;
	MYSLEEP	MySleep;
	MYGETOVERLAPPEDRESULT MyGetOverlappedResult;
	MYGETLASTERROR MyGetLastError;
	HANDLE handle;
	WCHAR Data[1024];
}

struct ProcessInfo//sizeof=6160
{
	DWORD	Index;//+0000
	WCHAR	ProcessNameString[1024];//+0004
	DWORD	ProcessId;//+2052
	WORD	HandleId;//+2056
	WCHAR	PathLockedString[1024];//+2058
	WCHAR	ProcessPathString[1024];//+4106
	HMODULE	hmod;//+6156 对齐
}

struct ProcessSInfo//sizeof=2052
{
	ULONG ProcessID;
	WCHAR ProcessName[1024];
};

typedef struct _SYSTEM_HANDLE_INFORMATION//sizeof=16
{
	ULONG	ProcessID;
	UCHAR	ObjectTypeNumber;
	UCHAR	Flags;// 0x01 = PROTECT_FROM_CLOSE, 0x02 = INHERIT
	USHORT	Handle;
	PVOID	Object_Pointer;
	ACCESS_MASK	GrantedAccess;
}SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef struct _SYSTEM_HANDLE_INFORMATION_EX
{
	ULONG NumberOfHandles;
	SYSTEM_HANDLE_INFORMATION Information[1];
}SYSTEM_HANDLE_INFORMATION_EX, *PSYSTEM_HANDLE_INFORMATION_EX;

typedef enum _SYSTEM_INFORMATION_CLASS 
{
    SystemBasicInformation,
    SystemProcessorInformation,
    SystemPerformanceInformation,
    SystemTimeOfDayInformation,
    SystemPathInformation,
    SystemProcessInformation,
    SystemCallCountInformation,
    SystemDeviceInformation,
    SystemProcessorPerformanceInformation,
    SystemFlagsInformation,
    SystemCallTimeInformation,
    SystemModuleInformation,
    SystemLocksInformation,
    SystemStackTraceInformation,
    SystemPagedPoolInformation,
    SystemNonPagedPoolInformation,
    SystemHandleInformation,
    SystemObjectInformation,
    SystemPageFileInformation,
    SystemVdmInstemulInformation,
    SystemVdmBopInformation,
    SystemFileCacheInformation,
    SystemPoolTagInformation,
    SystemInterruptInformation,
    SystemDpcBehaviorInformation,
    SystemFullMemoryInformation,
    SystemLoadGdiDriverInformation,
    SystemUnloadGdiDriverInformation,
    SystemTimeAdjustmentInformation,
    SystemSummaryMemoryInformation,
    SystemNextEventIdInformation,
    SystemEventIdsInformation,
    SystemCrashDumpInformation,
    SystemExceptionInformation,
    SystemCrashDumpStateInformation,
    SystemKernelDebuggerInformation,
    SystemContextSwitchInformation,
    SystemRegistryQuotaInformation,
    SystemExtendServiceTableInformation,
    SystemPrioritySeperation,
    SystemPlugPlayBusInformation,
    SystemDockInformation,
    SystemPowerInformation,
    SystemProcessorSpeedInformation,
    SystemCurrentTimeZoneInformation,
    SystemLookasideInformation
} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;

typedef enum _SYSTEM_HANDLE_TYPE
 {
    OB_TYPE_UNKNOWN=0,// 0
    OB_TYPE_TYPE,     // 1,fixed
    OB_TYPE_DIRECTORY,// 2,fixed -> \\KnownDlls + \\Windows + \\BaseNamedObjects
    OB_TYPE_SYMBOLIC_LINK,// 3,fixed
    OB_TYPE_TOKEN,    // 4,fixed
    OB_TYPE_PROCESS,  // 5,fixed
    OB_TYPE_THREAD,   // 6,fixed
    OB_TYPE_JOB,      // 7,fixed -> \BaseNamedObjects\WmiProviderSubSystemHostJob
    OB_TYPE_DEBUG_OBJECT,// 8,fixed
    OB_TYPE_EVENT,    // 9,fixed -> \\BaseNamedObjects\\TpVcW32ListEvent + \\Security\\TRKWKS_EVENT
    OB_TYPE_EVENT_PAIR,//10,fixed 
	OB_TYPE_MUTANT,    //11,fixed -> \\BaseNamedObjects\\ShimCacheMutex 
	OB_TYPE_CALLBACK,  //12,fixed -> 
	OB_TYPE_SEMAPHORE, //13,fixed -> \\BaseNamedObjects\\shell.{A48F1A32-A340-11D1-BC6B-00A0C90312E1}
    OB_TYPE_TIMER,     //14,fixed -> \\BaseNamedObjects\\userenv: refresh timer for 624:1460 + \\BaseNamedObjects\\userenv: refresh timer for 624:1576
    OB_TYPE_PROFILE,   //15,fixed
    OB_TYPE_KEYED_EVENT,//16,fixed -> \\KernelObjects\\CritSecOutOfMemoryEvent
    OB_TYPE_WINDOWS_STATION,//17,fixed -> \\Windows\\WindowStations\WinSta0 + \\Windows\\WindowStations\\Service-0x0-3e7$
    OB_TYPE_DESKTOP,   //18,fixed -> \\Default + \\Disconnect + \\Winlogon
    OB_TYPE_SECTION,   //19,fixed -> \\BaseNamedObjects\\ShimSharedMemory
    OB_TYPE_KEY,       //20,fixed -> \\REGISTRY\\USER
    OB_TYPE_PORT,      //21,fixed -> \\Windows\\ApiPort + \\FusApiPort
    OB_TYPE_WAITABLE_PORT,//22,fixed -> \\\NLAPrivatePort + \\Security\\TRKWKS_PORT
    OB_TYPE_ADAPTER,   //23,fixed
    OB_TYPE_CONTROLLER,//24,fixed
    OB_TYPE_DEVICE,    //25,fixed
    OB_TYPE_DRIVER,    //26,fixed
    OB_TYPE_IOCOMPLETION,//27,fixed
    OB_TYPE_FILE,      //28,fixed -> \\Dfs  + \\Device\\KSENUM#00000001 + \\Device\\Tcp
    OB_TYPE_WMIGUID    //29,fixed
 }SYSTEM_HANDLE_TYPE;


typedef struct _LIST_ENTRY 
{
       struct _LIST_ENTRY *Flink; 
       struct _LIST_ENTRY *Blink; 
} LIST_ENTRY, *PLIST_ENTRY;

//PEB:Process Enviroment Block相关
typedef struct _LDR_MODULE 
{
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
	PVOID                   BaseAddress;
	PVOID                   EntryPoint;
	ULONG                   SizeOfImage;
	UNICODE_STRING          FullDllName;
	UNICODE_STRING          BaseDllName;
	ULONG                   Flags;
	SHORT                   LoadCount;
	SHORT                   TlsIndex;
	LIST_ENTRY              HashTableEntry;
	ULONG                   TimeDateStamp;
	
} LDR_MODULE, *PLDR_MODULE;

typedef struct _PEB_LDR_DATA 
{
	ULONG                   Length;
	BOOLEAN                 Initialized;
	PVOID                   SsHandle;
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef PVOID* PPVOID;
typedef void (*PPEBLOCKROUTINE)(PVOID PebLock); 

typedef struct _RTL_DRIVE_LETTER_CURDIR 
{
	USHORT                  Flags;
	USHORT                  Length;
	ULONG                   TimeStamp;
	UNICODE_STRING          DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;


typedef struct _RTL_USER_PROCESS_PARAMETERS 
{
	ULONG                   MaximumLength;
	ULONG                   Length;
	ULONG                   Flags;
	ULONG                   DebugFlags;
	PVOID                   ConsoleHandle;
	ULONG                   ConsoleFlags;
	HANDLE                  StdInputHandle;
	HANDLE                  StdOutputHandle;
	HANDLE                  StdErrorHandle;
	UNICODE_STRING          CurrentDirectoryPath;
	HANDLE                  CurrentDirectoryHandle;
	UNICODE_STRING          DllPath;
	UNICODE_STRING          ImagePathName;
	UNICODE_STRING          CommandLine;
	PVOID                   Environment;
	ULONG                   StartingPositionLeft;
	ULONG                   StartingPositionTop;
	ULONG                   Width;
	ULONG                   Height;
	ULONG                   CharWidth;
	ULONG                   CharHeight;
	ULONG                   ConsoleTextAttributes;
	ULONG                   WindowFlags;
	ULONG                   ShowWindowFlags;
	UNICODE_STRING          WindowTitle;
	UNICODE_STRING          DesktopName;
	UNICODE_STRING          ShellInfo;
	UNICODE_STRING          RuntimeData;
	RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[0x20];	
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB_FREE_BLOCK 
{
	PEB_FREE_BLOCK          *Next;
	ULONG                   Size;
} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;

typedef struct _PEB 
{
	BOOLEAN                 InheritedAddressSpace;
	BOOLEAN                 ReadImageFileExecOptions;
	BOOLEAN                 BeingDebugged;
	BOOLEAN                 Spare;
	HANDLE                  Mutant;
	PVOID                   ImageBaseAddress;
	PPEB_LDR_DATA           LoaderData;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	PVOID                   SubSystemData;
	PVOID                   ProcessHeap;
	PVOID                   FastPebLock;
	PPEBLOCKROUTINE         FastPebLockRoutine;
	PPEBLOCKROUTINE         FastPebUnlockRoutine;
	ULONG                   EnvironmentUpdateCount;
	PPVOID                  KernelCallbackTable;
	PVOID                   EventLogSection;
	PVOID                   EventLog;
	PPEB_FREE_BLOCK         FreeList;
	ULONG                   TlsExpansionCounter;
	PVOID                   TlsBitmap;
	ULONG                   TlsBitmapBits[0x2];
	PVOID                   ReadOnlySharedMemoryBase;
	PVOID                   ReadOnlySharedMemoryHeap;
	PPVOID                  ReadOnlyStaticServerData;
	PVOID                   AnsiCodePageData;
	PVOID                   OemCodePageData;
	PVOID                   UnicodeCaseTableData;
	ULONG                   NumberOfProcessors;
	ULONG                   NtGlobalFlag;
	BYTE                    Spare2[0x4];
	LARGE_INTEGER           CriticalSectionTimeout;
	ULONG                   HeapSegmentReserve;
	ULONG                   HeapSegmentCommit;
	ULONG                   HeapDeCommitTotalFreeThreshold;
	ULONG                   HeapDeCommitFreeBlockThreshold;
	ULONG                   NumberOfHeaps;
	ULONG                   MaximumNumberOfHeaps;
	PPVOID                  *ProcessHeaps;
	PVOID                   GdiSharedHandleTable;
	PVOID                   ProcessStarterHelper;
	PVOID                   GdiDCAttributeList;
	PVOID                   LoaderLock;
	ULONG                   OSMajorVersion;
	ULONG                   OSMinorVersion;
	ULONG                   OSBuildNumber;
	ULONG                   OSPlatformId;
	ULONG                   ImageSubSystem;
	ULONG                   ImageSubSystemMajorVersion;
	ULONG                   ImageSubSystemMinorVersion;
	ULONG                   GdiHandleBuffer[0x22];
	ULONG                   PostProcessInitRoutine;
	ULONG                   TlsExpansionBitmap;
	BYTE                    TlsExpansionBitmapBits[0x80];
	ULONG                   SessionId;
} PEB, *PPEB;
