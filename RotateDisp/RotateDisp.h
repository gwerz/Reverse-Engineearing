// RotateDisp.h : main header file for the ROTATEDISP application
//

#if !defined(AFX_ROTATEDISP_H__450A4CEA_4E9E_4517_8E39_CC72A3AA03B5__INCLUDED_)
#define AFX_ROTATEDISP_H__450A4CEA_4E9E_4517_8E39_CC72A3AA03B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRotateDispApp:
// See RotateDisp.cpp for the implementation of this class
//

class CRotateDispApp : public CWinApp
{
public:
	CRotateDispApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRotateDispApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRotateDispApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROTATEDISP_H__450A4CEA_4E9E_4517_8E39_CC72A3AA03B5__INCLUDED_)
