// njubrass.h : main header file for the NJUBRASS application
//

#if !defined(AFX_NJUBRASS_H__67BB834E_64E4_4DBA_8FA3_C906CA1E9FE3__INCLUDED_)
#define AFX_NJUBRASS_H__67BB834E_64E4_4DBA_8FA3_C906CA1E9FE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNjubrassApp:
// See njubrass.cpp for the implementation of this class
//

class CNjubrassApp : public CWinApp
{
public:
	CNjubrassApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNjubrassApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
	CString str;
// Implementation

	//{{AFX_MSG(CNjubrassApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NJUBRASS_H__67BB834E_64E4_4DBA_8FA3_C906CA1E9FE3__INCLUDED_)
