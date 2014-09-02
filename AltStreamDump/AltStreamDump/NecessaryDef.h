#include <windows.h>
#define MAX_PATHLEN 260

typedef long NTSTATUS;
typedef unsigned long ULONG_PTR;

typedef struct _IO_STATUS_BLOCK 
{
	union 
	{
		NTSTATUS Status;
		PVOID    Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef enum _FILE_INFORMATION_CLASS 
{ 
	FileDirectoryInformation                 = 1,
	FileFullDirectoryInformation,
	FileBothDirectoryInformation,
	FileBasicInformation,
	FileStandardInformation,
	FileInternalInformation,
	FileEaInformation,
	FileAccessInformation,
	FileNameInformation,
	FileRenameInformation,
	FileLinkInformation,
	FileNamesInformation,
	FileDispositionInformation,
	FilePositionInformation,
	FileFullEaInformation,
	FileModeInformation,
	FileAlignmentInformation,
	FileAllInformation,
	FileAllocationInformation,
	FileEndOfFileInformation,
	FileAlternateNameInformation,
	FileStreamInformation,
	FilePipeInformation,
	FilePipeLocalInformation,
	FilePipeRemoteInformation,
	FileMailslotQueryInformation,
	FileMailslotSetInformation,
	FileCompressionInformation,
	FileObjectIdInformation,
	FileCompletionInformation,
	FileMoveClusterInformation,
	FileQuotaInformation,
	FileReparsePointInformation,
	FileNetworkOpenInformation,
	FileAttributeTagInformation,
	FileTrackingInformation,
	FileIdBothDirectoryInformation,
	FileIdFullDirectoryInformation,
	FileValidDataLengthInformation,
	FileShortNameInformation,
	FileIoCompletionNotificationInformation,
	FileIoStatusBlockRangeInformation,
	FileIoPriorityHintInformation,
	FileSfioReserveInformation,
	FileSfioVolumeInformation,
	FileHardLinkInformation,
	FileProcessIdsUsingFileInformation,
	FileNormalizedNameInformation,
	FileNetworkPhysicalNameInformation,
	FileIdGlobalTxDirectoryInformation,
	FileIsRemoteDeviceInformation,
	FileAttributeCacheInformation,
	FileNumaNodeInformation,
	FileStandardLinkInformation,
	FileRemoteProtocolInformation,
	FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef struct _FILE_STREAM_INFORMATION 
{
	ULONG         NextEntryOffset;
	ULONG         StreamNameLength;
	LARGE_INTEGER StreamSize;
	LARGE_INTEGER StreamAllocationSize;
	WCHAR         StreamName[1];
} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;

typedef NTSTATUS (WINAPI* NTQUERYINFORMATIONFILE)(HANDLE FileHandle,PIO_STATUS_BLOCK IoStatusBlock,
												  PVOID FileInformation,ULONG Length,FILE_INFORMATION_CLASS FileInformationClass);
typedef BOOL (WINAPI* LOOKUPPRIVILEGEVALUE)(LPCWSTR lpSystemName,LPCWSTR lpName,PLUID lpLuid);
typedef BOOL (WINAPI* OPENPROCESSTOEKN)(HANDLE ProcessHandle,DWORD DesiredAccess,PHANDLE TokenHandle);
typedef BOOL (WINAPI* ADJUSTTOKENPRIVILEGES)(HANDLE TokenHandle,BOOL DisableAllPrivileges,PTOKEN_PRIVILEGES NewState,
		DWORD BufferLength,PTOKEN_PRIVILEGES PreviousState,PDWORD ReturnLength);

struct StreamStruct
{
	WCHAR		StreamName[261];
	WCHAR		FileName[261];
	LONGLONG	StreamSize;
	LONGLONG	StreamAllocationSize;
	BOOL		IsDirectory;//or path
};

class MainClass
{
public:
	MainClass();
	void SetParam(WCHAR* paramtype,WCHAR* paramdata);
	void GoSearch();
	void RecurseFind(LPWSTR path,int CurrentDepth);

private:
	void ShowStaticInfo();
	virtual void AdaptStatic(StreamStruct* mystream);//0
	virtual void ShowCurrentFileStream(LPWSTR FolderPath);//2
	void WINAPI GetStreamData(HANDLE hFileHandle,LPCWSTR FileName,BOOL IsDir);
	void WINAPI GetStream(LPCWSTR FileName);
	void WINAPI GetStream(LPCWSTR cFileName,LPCWSTR cAlterFileName);

private:
	NTQUERYINFORMATIONFILE	NtQueryInfo;
	BOOL					NotBeginSearch;
	WCHAR					FolderPath[261];
	BOOL					SearchSubFolders;
	int						SubfoldersDepth;
	LONGLONG				TotalStreamBytes;
	LONGLONG				TotalStreamNum;
	DWORD					mem9;
	DWORD					mem10;
	WCHAR					CurrentDirectory[261];
};

class FindFileClass
{
public:
	FindFileClass();
	void GenSearchString(LPCWSTR Path);
	BOOL EnumNextFile();
	bool IsDir();
	void ReleaseFile();

public:
	HANDLE				hFindFile;
	WCHAR				LastFileName[324];
	WIN32_FIND_DATAW	FindData;
	WCHAR				CurrentFileName[324];
	WCHAR				CurrentFolderPath[324];
};
