// RotateDispDlg.h : header file
//

#if !defined(AFX_ROTATEDISPDLG_H__3FCEBA68_E765_42F2_9F81_D405E583D66A__INCLUDED_)
#define AFX_ROTATEDISPDLG_H__3FCEBA68_E765_42F2_9F81_D405E583D66A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRotateDispDlg dialog

class CRotateDispDlg : public CDialog
{
// Construction
public:
	CRotateDispDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRotateDispDlg)
	enum { IDD = IDD_ROTATEDISP_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRotateDispDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRotateDispDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnWebsite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROTATEDISPDLG_H__3FCEBA68_E765_42F2_9F81_D405E583D66A__INCLUDED_)
