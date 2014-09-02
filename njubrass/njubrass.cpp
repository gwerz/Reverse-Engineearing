// njubrass.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "njubrass.h"
#include "njubrassDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNjubrassApp

BEGIN_MESSAGE_MAP(CNjubrassApp, CWinApp)
	//{{AFX_MSG_MAP(CNjubrassApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNjubrassApp construction

CNjubrassApp::CNjubrassApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	str="njubras";
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNjubrassApp object

CNjubrassApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CNjubrassApp initialization

BOOL CNjubrassApp::InitInstance()
{
	AfxEnableControlContainer();

	WSADATA WSAData;
	CreateMutex(NULL,TRUE,(LPCTSTR)str);
	if(GetLastError()==183)
	{
		MessageBox(NULL,"Last njubras still run","njubras Messae",MB_OK);
		return -1;
	}
	
	memset(&WSAData,0,sizeof(WSADATA));
	if(WSAStartup(MAKEWORD(2,2),&WSAData)!=0)
	{
		MessageBox(NULL,"无法初始化网络!","ERROR",MB_OK);
		return -1;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CNjubrassDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CNjubrassApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	WSACleanup();
	return CWinApp::ExitInstance();
}
