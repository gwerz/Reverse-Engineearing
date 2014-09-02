#if !defined(AFX_ROTATEDISPWND_H__42D9138D_874D_4E3F_A763_5D1BF3D8F6E1__INCLUDED_)
#define AFX_ROTATEDISPWND_H__42D9138D_874D_4E3F_A763_5D1BF3D8F6E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RotateDispWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRotateDispWnd dialog

class CRotateDispWnd : public CWnd
{
// Construction
public:
	CRotateDispWnd(CWnd* pParent = NULL);   // standard constructor
	void BeginWindowProc();
	
	bool IsAboutDlgShow;//64
	bool IsMouseDllOk;//65
	HWND hDlgWnd;//68
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRotateDispWnd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	int RotNum;//-1 0 1 2 3
	NOTIFYICONDATA IconData;
	
	void ChangeDispById(int angle);
	void ShowShellIcon();
	bool CanRotate();
	bool CheckCanRotate(LPCSTR lpSubKey);
	// Generated message map functions
	//{{AFX_MSG(CRotateDispWnd)
	afx_msg void OnExit();
	afx_msg void OnVersioninfo();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROTATEDISPWND_H__42D9138D_874D_4E3F_A763_5D1BF3D8F6E1__INCLUDED_)
