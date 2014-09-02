// njubrassDlg.cpp : implementation file
//

#include "stdafx.h"
#include "njubrass.h"
#include "njubrassDlg.h"
#include "MessageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BEGIN				1
#define END					2
#define WM_NOTIFYMESSAGE	WM_USER+909//自定义托盘图标消息
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNjubrassDlg dialog

CNjubrassDlg::CNjubrassDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNjubrassDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNjubrassDlg)
	m_MiPassWord = _T("");
	m_MingPassWord = _T("");
	m_MingUserName = _T("");
	m_MiUserName = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNjubrassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNjubrassDlg)
	DDX_Text(pDX, IDC_PassWord2, m_MiPassWord);
	DDX_Text(pDX, IDC_PassWord, m_MingPassWord);
	DDX_Text(pDX, IDC_UserName, m_MingUserName);
	DDX_Text(pDX, IDC_UserName2, m_MiUserName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNjubrassDlg, CDialog)
	//{{AFX_MSG_MAP(CNjubrassDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Encrypt, OnEncrypt)
	ON_BN_CLICKED(IDC_Decrypt, OnDecrypt)
	ON_BN_CLICKED(IDS_ABOUTBOX, OnAboutbox)
	ON_BN_CLICKED(IDC_Connect, OnConnect)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_Exit, &CNjubrassDlg::OnBnClickedExit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNjubrassDlg message handlers

BOOL CNjubrassDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	GetDlgItem(IDC_OriPassword)->SetFocus();
	::SetWindowPos(this->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE | SWP_NOMOVE);
	Reg_getvalue1();
	Reg_getvalue2();
	FUNC1();
	hrasconn=NULL;
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE  unless you set the focus to a control
}

DWORD CNjubrassDlg::Reg_getvalue1()
{
	HKEY hkey;
	char data[256];
	DWORD size=256;
	DWORD type=REG_NONE;
	OSVERSIONINFO OI;
	TOKEN_PRIVILEGES TP;
	CString username,password,sign;
	BOOL ischecked,issuccess;
	BYTE i=1;
	HANDLE hand;

	if(!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\njubras\\Project1",0,STANDARD_RIGHTS_READ | 
		KEY_NOTIFY | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,&hkey))
	{
		memset(data,0,256);
		size=256;
		if(!RegQueryValueEx(hkey,"MyValue1",0,&type,(PBYTE)data,&size))
		{
			username=Decrypt(data);
		}
		memset(data,0,256);
		size=256;
		if(!RegQueryValueEx(hkey,"MyValue2",0,&type,(PBYTE)data,&size))
		{
			password=Decrypt(data);
		}
		memset(data,0,256);
		size=256;
		if(!RegQueryValueEx(hkey,"MyValue3",0,&type,(PBYTE)data,&size))
		{
			sign=data;
		}
		memset(data,0,256);
		size=256;
		if(RegQueryValueEx(hkey,"RadioCheck",0,&type,(PBYTE)data,&size))
			ischecked=FALSE;
		else ischecked=data[0];
	}
	RegCloseKey(hkey);
	if(sign=="1")
		CheckDlgButton(IDC_CHECK_off,BST_CHECKED);
	if(ischecked==TRUE)
		CheckDlgButton(IDC_CHECK_save,BST_CHECKED);
	GetDlgItem(IDC_OriUsername)->SetWindowText(username);
	GetDlgItem(IDC_OriPassword)->SetWindowText(password);
	
	size=256;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\RasMan\\Parameters",
		0,STANDARD_RIGHTS_READ | KEY_NOTIFY | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,&hkey) || 
		RegQueryValueEx(hkey,"ProhibitIPSec",0,&type,(PBYTE)data,&size))
	{
		issuccess=TRUE;
	}
	else issuccess=data[0]!=1;
	RegCloseKey(hkey);
	if(issuccess==TRUE)
	{
		RegCreateKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\RasMan\\Parameters",&hkey);
		RegSetValueEx(hkey,"ProhibitIPSec",0,REG_DWORD,&i,4);
		RegCloseKey(hkey);
		if(IDYES==MessageBox("This is your first setup for nju bras cient.\nThe computer will restart to finish your setup.\n\
							 Do you want to restart your computer now?","提示消息",MB_YESNOCANCEL))
		{
			OI.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
			if(GetVersionEx(&OI) && OI.dwPlatformId!=2)
			{
				ExitWindowsEx(EWX_REBOOT  | EWX_FORCE,SHTDN_REASON_MAJOR_NONE);
			}
			else 
			{
				if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hand))
				{
					LookupPrivilegeValue(NULL,"SeShutdownPrivilege",&TP.Privileges[0].Luid);
					TP.PrivilegeCount=1;
					TP.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
					AdjustTokenPrivileges(hand,FALSE,&TP,0,NULL,NULL);
					ExitWindowsEx(EWX_REBOOT  | EWX_FORCE,SHTDN_REASON_MAJOR_NONE);
				}
			}
		}
	}
	return 0;
}

DWORD CNjubrassDlg::Reg_getvalue2()
{
	HKEY hkey;
	char data[256];
	DWORD size=256;
	DWORD type=REG_NONE;
	CString username,password,sign;
	
	if(!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\njubras\\Project2",0,STANDARD_RIGHTS_READ | 
		KEY_NOTIFY | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,&hkey))
	{
		memset(data,0,256);
		if(!RegQueryValueEx(hkey,"MyValue1",0,&type,(PBYTE)data,&size))
		{
			username=Decrypt(data);
		}
		size=256;
		memset(data,0,256);
		if(!RegQueryValueEx(hkey,"MyValue2",0,&type,(PBYTE)data,&size))
		{
			password=Decrypt(data);
		}
		size=256;
		memset(data,0,256);
		if(!RegQueryValueEx(hkey,"MyValue3",0,&type,(PBYTE)data,&size))
		{
			sign=data;
		}
	}
	RegCloseKey(hkey);
	int num;
	if(username=="1")
	{
		num=atoi(sign);
		SetTimer(1002,60000*num,NULL);
	}
	All1="";
	return 0;
}

DWORD CNjubrassDlg::FUNC1()
{
	RASCONN rasconn[256];
	DWORD connections=0;
	DWORD i=0,size=256*sizeof(RASCONN);
	rasconn[0].dwSize=676;
	int result=RasEnumConnections(rasconn,&size,&connections);
	if(!result && connections>0)
	{
		char* temp=rasconn[0].szEntryName;
		while(1)
		{
			result=(DWORD)strstr(temp,"njubras");
			if(result) break;
			++i;
			temp+=sizeof(RASCONN);
			if(i>=256) return result;
		}
		hrasconn=rasconn[i].hrasconn;
		SetWindowText("Connecting succeed!");
		GetDlgItem(IDC_Connect)->SetWindowText("Disconnect");
		GetDlgItem(IDC_OriUsername)->EnableWindow(FALSE);
		GetDlgItem(IDC_OriPassword)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_off)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_save)->EnableWindow(FALSE);
		return 0;
	}
}

void CNjubrassDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNjubrassDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNjubrassDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

CString CNjubrassDlg::Encrypt(CString str)
{
	int length=str.GetLength();
	int after=0;
	CString result="";
	for(int pos=1;pos<=length;pos++)
	{
		char cursig=str[pos-1];
		int temp=pos/3;
		int temp1=pos%3;
		if(temp1 == 1)
		{
			result+=EncryptSingleChar(cursig>>2);
			after=16*(cursig & 3);
		}
		else if(temp1 == 2)
		{
			temp=after | (cursig>>4);
			result+=EncryptSingleChar(temp);
			after=4*(cursig & 0xF);
		}
		else//pos%3==0
		{
			result+=EncryptSingleChar(after | (cursig>>6));
			result+=EncryptSingleChar(cursig);
		}
	}
	if(length%3)
	{
		result+=EncryptSingleChar(after);
		if(length%3 == 1)
			result+="==";
		else
			result+="=";
	}

	return result;
}

void CNjubrassDlg::OnEncrypt() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_MiUserName=Encrypt(m_MingUserName);
	m_MiPassWord=Encrypt(m_MingPassWord);
	UpdateData(FALSE);
}

CString CNjubrassDlg::Decrypt(CString str)
{
	int length=str.GetLength();
	int after=0;
	CString result="";
	char lastchar='\0';
	char changresult='\0';
	for(int pos=1;pos<=length;pos++)
	{
		char cursig=str[pos-1];
		if(cursig==61) break;
		changresult=DecryptSingleChar(cursig);
		int temp=pos%4;
		if(temp==1)
		{
			lastchar=4*changresult;
		}
		else if(temp==2)
		{
			result+=(char)(lastchar | (changresult>>4));
			lastchar=16*changresult;
		}
		else if(temp==3)
		{
			result+=(char)(lastchar | (changresult>>2));
			lastchar=changresult<<6;
		}
		else
		{
			result+=(char)(changresult | lastchar);
		}
	}

	return result;
}

void CNjubrassDlg::OnDecrypt() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_MingUserName=Decrypt(m_MiUserName);
	m_MingPassWord=Decrypt(m_MiPassWord);
	UpdateData(FALSE);
}

char CNjubrassDlg::EncryptSingleChar(char mychar)
{
	mychar=mychar & 0x3F;
	if(mychar>=0 && mychar<=25)
		return mychar+65;
	else if(mychar>=26 && mychar<=51)
		return mychar+71;
	else if(mychar>=52 && mychar<=61)
		return mychar-4;
	else if(mychar==62) return 43;
	else if(mychar==63) return 47;
	else return 0;
}

char CNjubrassDlg::DecryptSingleChar(char mychar)
{
	if(mychar>=65 && mychar<=90)
		return mychar-65;
	else if(mychar>=97 && mychar<=122)
		return mychar-71;
	else if(mychar>=48 && mychar<=57)
		return mychar+4;
	else if(mychar==43) return 62;
	else if(mychar==47) return 47;
	else return 0;
}

void CNjubrassDlg::OnAboutbox() 
{
	// TODO: Add your control notification handler code here
	CAboutDlg about;
	about.DoModal();
}

void CNjubrassDlg::OnConnect() 
{
	// TODO: Add your control notification handler code here
	CString ConnectState;
	GetDlgItem(IDC_Connect)->GetWindowText(ConnectState);
	CString username;
	CString password;
	CString AdaperInfo;
	CString command;
	int nRet;

	if(ConnectState!="Disconnect")
	{
		initialNet();
		reg_setvalue();
		GetDlgItem(IDC_Connect)->SetWindowText("Connection...");
		GetDlgItem(IDC_OriUsername)->GetWindowText(username);
		GetDlgItem(IDC_OriPassword)->GetWindowText(password);
		CString temp;
		if(IsDlgButtonChecked(IDC_CHECK_off))
		{
			temp=Decrypt("MjE4Ljk0LjE0Mi4xMTQ=");//218.94.142.114
			initialOffCampusNet();
		}
		else
		{
			temp=Decrypt("MTcyLjIxLjEwMC4xMDA=");//172.21.100.100
		}
		MyRasDial("VPN","njubras",(LPTSTR)(LPCTSTR)username,(LPTSTR)(LPCTSTR)password,
			(LPTSTR)(LPCTSTR)temp);
		MyGetAdaptersInfo(AdaperInfo);
		nRet=RasConnect("njubras","","",(LPTSTR)(LPCTSTR)username,(LPTSTR)(LPCTSTR)password,"");
		if(nRet)
		{
			ErrorCause(nRet);
			hrasconn=0;
			SetWindowText("Connect error!");
			SetTimer(1001,1000,NULL);
			GetDlgItem(IDC_OriPassword)->SetFocus();
		}
		else
		{
			ExecuteNotifyIcon(1);
			if(IsDlgButtonChecked(IDC_CHECK_off)==FALSE)
			{
				initialNet2();
				command="rem\r\n echo off\r\n";
				if(username.GetLength()>0)
				{
					int pos=AdaperInfo.Find(";");
					while(pos>0)
					{
						CString temp=AdaperInfo.Left(pos);
						if(temp.Find("172.16.96.")<0)
						{
							if(temp.Find("0.0.0."<0)<0)
							{
								int pos1=username.Find(";");
								while(pos1>0)
								{
									CString temp1=username.Left(pos1);
									command+="route add ";
									command+=temp1;
									command+=" ";
									command+=temp;
									command+=" metric 20\r\n";
									CString temp2=AdaperInfo.Mid(pos1+1,AdaperInfo.GetLength()-pos1);
									pos1=temp2.Find(";");
								}
								AdaperInfo=AdaperInfo.Mid(pos1+1,AdaperInfo.GetLength()-pos);
							}
							else
							{
								AdaperInfo=AdaperInfo.Mid(pos+1,AdaperInfo.GetLength()-pos);
							}
						}
						else
						{
							AdaperInfo=AdaperInfo.Mid(pos+1,AdaperInfo.GetLength()-pos);
						}
						pos=AdaperInfo.Find(";");
					}
				}
				command+="echo on\r\n";
				CFile file("tmp.cmd",CFile::modeCreate | CFile::modeWrite);
				file.Write((LPTSTR)(LPCTSTR)command,command.GetLength());
				file.Close();
				ShellExecute(NULL,"open","tmp.cmd",0,0,0);
				Sleep(1000);
				DeleteFile("tmp.cmd");
			}
			SetWindowText("Connecting succeed!");
			GetDlgItem(IDC_Connect)->SetWindowText("Disconnect");
			Sleep(1000);
			GetDlgItem(IDC_interpretation)->SetWindowText("\r\n       Designed by njunic\
				Web: http://nic.nju.edu.cn                     Tel:  025-83594793           Email:bras@nju.edu.cn");
			CString content;
			GetDlgItem(IDC_OriUsername)->GetWindowText(content);
			ExecuteNotifyIcon(1);
			SuccessRet("bras,"+content);
		}
	}
	else if(hrasconn)
	{
		RasHangUp(hrasconn);
		SetWindowText("Connect to njunic");
		GetDlgItem(IDC_Connect)->SetWindowText("Connect(&C)");
		hrasconn=0;
		GetDlgItem(IDC_OriUsername)->EnableWindow(TRUE);
		GetDlgItem(IDC_OriPassword)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_off)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_save)->EnableWindow(TRUE);
	}
}

DWORD CNjubrassDlg::SuccessRet(CString str)
{
	CString str4="MjAyLjExOS4zMi41Ng==";//202.119.32.56
	str4=Decrypt(str4);
	CString str2=str+":8080/t.jsp?userId=";
	str2+=curusername;
	//func(str2)
	CString str3;
	//str3=data
	if(str3.GetLength()>=0)
	{
		if(CoInitialize(NULL)>=0)
		{
			Initialcom(str3);
		}
		CoUninitialize();
	}
	if(str3.GetLength())
	{
		//func
		CMessageDlg dlg;
		dlg.DoModal();
	}
	OnCancel();

	return true;
}

DWORD CNjubrassDlg::Initialcom(CString str)
{
	HRESULT				hr;
	IXMLDOMDocument*	pXMLDoc;
	IXMLDOMNode*		pXDN;
	if(CoCreateInstance(CLSID_DOMDocument,NULL,CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER,
		IID_IXMLDOMDocument,(LPVOID*)&pXMLDoc))
	{

	}
	return 0;
}

DWORD CNjubrassDlg::MyGetAdaptersInfo(CString& info)
{
	IP_ADAPTER_INFO IAI; 
	IP_ADAPTER_INFO* cur;
	DWORD size=sizeof(IAI);
	info="";
	memset(&IAI,0,sizeof(IP_ADAPTER_INFO));

	if(GetAdaptersInfo(&IAI,&size))//!=ERROR_SUCCESS
	{
		return 1;
	}
	else
	{
		for(cur=&IAI;cur!=NULL;cur=cur->Next)
		{
			CString str=cur->GatewayList.IpAddress.String;
			if(info.Find(str)==-1 && info.GetLength()>0)
			{
				info+=str;
				info+=";";
			}
		}
	}

	return 1;
}

DWORD CNjubrassDlg::RasConnect(char* entryname,char* phonenumber,char* callbacknumber,
	char* username,char* password,char* domain)
{
	RASDIALPARAMS rasdialparams;
	HCURSOR cursor1,cursor2;
	DWORD nRet;
	memset(&rasdialparams,0,sizeof(RASDIALPARAMS));
	rasdialparams.dwSize=1060;
	strcpy(rasdialparams.szEntryName,entryname);
	strcpy(rasdialparams.szPhoneNumber,phonenumber);
	strcpy(rasdialparams.szCallbackNumber,callbacknumber);
	strcpy(rasdialparams.szUserName,username);
	strcpy(rasdialparams.szPassword,password);
	strcpy(rasdialparams.szDomain,domain);
	cursor1=GetCursor();
	cursor2=LoadCursor(NULL,IDC_WAIT);
	SetCursor(cursor2);
	nRet=RasDial(NULL,NULL,&rasdialparams,0,NULL,&hrasconn);
	RASCONNSTATUS rs;
	rs.dwSize=sizeof(RASCONNSTATUS);
	nRet=RasGetConnectStatus(hrasconn,&rs);
	nRet=GetLastError();
	SetCursor(cursor1);
	if(nRet!=NULL)
	{
		RasHangUp(hrasconn);
	}
	return nRet;
}

DWORD CNjubrassDlg::MyRasDial(char* type,char* name,char* username,char* password,char* DestIP)
{
	RASCREDENTIALS	rascredentials;
	RASENTRY		rasentry;
	int				lengthofip=strlen(DestIP);
	DWORD			dwBufferSize=0;
	RasGetEntryProperties(NULL,"",NULL,&dwBufferSize,NULL,NULL);
	if(dwBufferSize==0) return -1;

	memset(&rascredentials,0,sizeof(RASCREDENTIALS));
	memset(&rasentry,0,sizeof(RASENTRY));
	rascredentials.dwSize=sizeof(RASCREDENTIALS);
	rascredentials.dwMask=RASCM_UserName | RASCM_Password | RASCM_DefaultCreds;
	
	rasentry.dwSize=dwBufferSize;
//	ASSERT(dwBufferSize==1796);
	rasentry.dwCountryCode=86;
	rasentry.dwCountryID=86;
	rasentry.dwDialExtraSampleSeconds=120;
	rasentry.dwHangUpExtraSampleSeconds = 120;
	rasentry.dwDialExtraPercent=75;
	rasentry.dwDialMode=RASEDM_DialAll;
	rasentry.dwHangUpExtraPercent = 10;
	rasentry.dwfNetProtocols = RASNP_Ip;
	rasentry.dwfOptions=RASEO_RemoteDefaultGateway | RASEO_ModemLights | RASEO_SwCompression | 
		RASEO_SecureLocalFiles | RASEO_RequirePAP | RASEO_RequireSPAP | RASEO_PreviewUserPw |
		RASEO_ShowDialingProgress | RASEO_RequireCHAP | RASEO_RequireMsCHAP | RASEO_RequireMsCHAP2;
//	ASSERT(rasentry.dwfOptions==1024262928);
	rasentry.dwFramingProtocol=RASFP_Ppp;
//	ASSERT(rascredentials.dwSize==540);

	strcpy(rasentry.szLocalPhoneNumber,DestIP);
	strcpy(rasentry.szDeviceType,type);
	strcpy(rasentry.szDeviceName,"WAN   微型端口   (PPTP)");

	DWORD dwResult;
	dwResult=RasSetEntryProperties(NULL,name,&rasentry,sizeof(RASENTRY),NULL,0);
	strcpy(rascredentials.szUserName,username);
	strcpy(rascredentials.szPassword,password);

	if(dwResult || (dwResult=RasSetCredentials(NULL,name,&rascredentials,NULL))!=0)
	{
		ErrorCause(dwResult);
		return 0;
	}
	else return 1;
}

DWORD CNjubrassDlg::ErrorCause(DWORD dwResult)
{
	char szString[0x100];
	memset(szString,0,0x100);
	RasGetErrorString(dwResult,szString,0x100);
	CString error;
	error.Format("%d : ",dwResult);
	error+=szString;
	GetDlgItem(IDC_interpretation)->SetWindowText(error);
	RECT Rect;
	GetWindowRect(&Rect);
	Rect.bottom+=70;
	MoveWindow(&Rect,TRUE);
	return true;
}

DWORD CNjubrassDlg::reg_setvalue()
{
	HKEY hKey;
	CString username="";
	CString password="";
	if(!RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\njubras\\Project1",&hKey))
	{
		GetDlgItem(IDC_OriUsername)->GetWindowText(username);
		GetDlgItem(IDC_OriPassword)->GetWindowText(password);
		username=Encrypt(username);
		password=Encrypt(password);
		if(IsDlgButtonChecked(IDC_CHECK_save))
		{
			RegSetValueEx(hKey,"MyValue1",0,REG_SZ,(const BYTE*)(LPCTSTR)username,username.GetLength());
			RegSetValueEx(hKey,"MyValue2",0,REG_SZ,(const BYTE*)(LPCTSTR)password,password.GetLength());
		}
		else
		{
			RegSetValueEx(hKey,"MyValue1",0,REG_SZ,(const BYTE*)"",1);
			RegSetValueEx(hKey,"MyValue2",0,REG_SZ,(const BYTE*)"",1);
		}

		if(IsDlgButtonChecked(IDC_CHECK_off))
		{
			RegSetValueEx(hKey,"MyValue3",0,REG_SZ,(const BYTE*)"1",1);
		}
		else
		{
			RegSetValueEx(hKey,"MyValue3",0,REG_SZ,(const BYTE*)"0",1);
		}
		DWORD IsChecked = IsDlgButtonChecked(IDC_CHECK_save)!=0;
		RegSetValueEx(hKey,"RadioCheck",0,REG_DWORD,(const BYTE*)IsChecked,4);
		RegCloseKey(hKey);
	}
	return 0;
}

DWORD CNjubrassDlg::initialNet()
{
	SOCKET g_ClientSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(g_ClientSocket==INVALID_SOCKET) return -1;
	HOSTENT* host=gethostbyname("nic.nju.edu.cn");
	if(host==NULL) return -1;
	sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_addr.S_un.S_addr=inet_addr(*host->h_addr_list);
	addr.sin_port=htons(0x50);
	SOCKET g_ConnectSocket=connect(g_ClientSocket,(sockaddr*)&addr,sizeof(addr));
	if(g_ConnectSocket==-1) return -1;
	
	char buf[1024]="GET http://nic.nju.edu.cn/bras/error.txt HTTP/1.1\r\nAccept: */*\r\n\
		Referer: http://nic.nju.edu.cn/bras/error.txt Accept-Language: zh-cn\r\nAccept-Encoding: gzip, \
		deflate\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\nHost: \
		nic.nju.edu.cn\r\nConnection: Keep-Alive\r\n\r\n";
	send(g_ClientSocket,buf,strlen(buf),0);
	WSAGetLastError();

	char Str[1024];
	memset(Str,0,1024);
	if(recv(g_ClientSocket,Str,1024,0))
	{
		if(strstr(Str,"HTTP/1.1 200 OK"))
		{
			char* findpos=strstr(Str,"Content-Type: text/plain\r\n\r\n");
			if(findpos)
			{
				int length=strlen(Str);
				if(int(findpos-Str+28)<length)
				{
					char message[1024];
					memset(message,0,1024);
					strncpy(message,findpos+28,length-(findpos-Str)-28);
					MessageBox(message,"njubras message",MB_OK);
				}
			}
		}
	}
	return closesocket(g_ClientSocket);
}

DWORD CNjubrassDlg::initialNet2()
{
	SOCKET g_ClientSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(g_ClientSocket==INVALID_SOCKET) return -1;
	HOSTENT* host=gethostbyname("nic.nju.edu.cn");
	if(host==NULL) return -1;
	sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_addr.S_un.S_addr=inet_addr(*host->h_addr_list);
	addr.sin_port=htons(0x50);
	SOCKET g_ConnectSocket=connect(g_ClientSocket,(sockaddr*)&addr,sizeof(addr));
	if(g_ConnectSocket==-1) return -1;
	
	char buf[1024]="GET http://nic.nju.edu.cn/bras/ru.txt HTTP/1.1\r\nAccept: */*\r\n\
				   Referer: http://nic.nju.edu.cn/bras/ru.txt Accept-Language: zh-cn\r\n\
				   Accept-Encoding: gzip, deflate\r\nUser-Agent: Mozilla/4.0 \
				   (compatible; MSIE 5.01; Windows NT 5.0)\r\nHost: nic.nju.edu.cn\r\n\
				   Connection: Keep-Alive\r\n\r\n";
	send(g_ClientSocket,buf,strlen(buf),0);
	WSAGetLastError();

	char Str[1024];
	memset(Str,0,1024);
	if(recv(g_ClientSocket,Str,1024,0))
	{
		if(strstr(Str,"HTTP/1.1 200 OK"))
		{
			char* findpos=strstr(Str,"Content-Type: text/plain\r\n\r\n");
			if(findpos)
			{
				int length=strlen(Str);
				if(int(findpos-Str+28)<length)
				{
					char message[1024];
					memset(message,0,1024);
					strncpy(message,findpos+28,length-(findpos-Str)-28);
					MessageBox(message,"njubras message",MB_OK);
				}
			}
		}
	}
	return closesocket(g_ClientSocket);
}

DWORD CNjubrassDlg::initialOffCampusNet()
{
	SOCKET g_ClientSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(g_ClientSocket==INVALID_SOCKET) return -1;
	HOSTENT* host=gethostbyname("nic.nju.edu.cn");
	if(host==NULL) return -1;
	sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_addr.S_un.S_addr=inet_addr(*host->h_addr_list);
	addr.sin_port=htons(0x50);
	SOCKET g_ConnectSocket=connect(g_ClientSocket,(sockaddr*)&addr,sizeof(addr));
	if(g_ConnectSocket==-1) return -1;
	
	char buf[1024]="GET http://nic.nju.edu.cn/bras/offcampus.txt HTTP/1.1\r\n\
		Accept: */*\r\nReferer: http://nic.nju.edu.cn/bras/offcampus.txt \
		Accept-Language: zh-cn\r\nAccept-Encoding: gzip, deflate\r\nUser-Agent: \
		Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\nHost: nic.nju.edu.cn\r\n\
		Connection: Keep-Alive\r\n\r\n";
	send(g_ClientSocket,buf,strlen(buf),0);
	WSAGetLastError();

	char Str[1024];
	memset(Str,0,1024);
	if(recv(g_ClientSocket,Str,1024,0))
	{
		if(strstr(Str,"HTTP/1.1 200 OK"))
		{
			char* findpos=strstr(Str,"Content-Type: text/plain\r\n\r\n");
			if(findpos)
			{
				int length=strlen(Str);
				if(int(findpos-Str+28)<length)
				{
					char message[1024];
					memset(message,0,1024);
					strncpy(message,findpos+28,length-(findpos-Str)-28);
					MessageBox(message,"njubras message",MB_OK);
				}
			}
		}
	}
	return closesocket(g_ClientSocket);
}

void CNjubrassDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	ExecuteNotifyIcon(END);
	reg_setvalue();
	CNjubrassDlg::OnCancel();
}

BOOL CNjubrassDlg::ExecuteNotifyIcon(int nCode)
{
	NOTIFYICONDATA Data;
	BOOL result;
	char str[]="NJU Bras Client by NIC";
	memset(&Data,0,sizeof(Data));
	Data.cbSize=sizeof(NOTIFYICONDATA);
	Data.hWnd=m_hWnd;
	Data.uFlags=NIF_MESSAGE | NIF_ICON | NIF_TIP;
	Data.uID=IDR_MAINFRAME;
	Data.uCallbackMessage=WM_NOTIFYMESSAGE;
	Data.hIcon=LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy(Data.szTip,str);

	if(nCode==1)
	{
		Shell_NotifyIcon(NIM_ADD,&Data);
		result=ShowWindow(SW_HIDE);
	}
	else if(nCode==2)
	{
		result=Shell_NotifyIcon(NIM_DELETE,&Data);
	}

	return result;
}