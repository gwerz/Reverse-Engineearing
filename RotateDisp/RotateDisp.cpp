// RotateDisp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RotateDisp.h"
#include "RotateDispDlg.h"
#include "RotateDispWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRotateDispApp

BEGIN_MESSAGE_MAP(CRotateDispApp, CWinApp)
	//{{AFX_MSG_MAP(CRotateDispApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRotateDispApp construction

CRotateDispApp::CRotateDispApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRotateDispApp object

CRotateDispApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRotateDispApp initialization

BOOL CRotateDispApp::InitInstance()
{
	CreateMutex(NULL,FALSE,"RotateDisp");
	if(GetLastError())
		return FALSE;

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CRotateDispWnd* pwnd=new CRotateDispWnd;
	m_pMainWnd=pwnd;
	pwnd->BeginWindowProc();

	return TRUE;
}
