// njubrassDlg.h : header file
//

#if !defined(AFX_NJUBRASSDLG_H__14318543_0999_415C_BD2E_CEB97B81B646__INCLUDED_)
#define AFX_NJUBRASSDLG_H__14318543_0999_415C_BD2E_CEB97B81B646__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNjubrassDlg dialog

class CNjubrassDlg : public CDialog
{
// Construction
public:
	CNjubrassDlg(CWnd* pParent = NULL);	// standard constructor

	CString		Encrypt(CString str);
	CString		Decrypt(CString str);
	CString		All1;
	char		EncryptSingleChar(char mychar);
	char		DecryptSingleChar(char mychar);
	DWORD		initialNet();
	DWORD		initialNet2();
	DWORD		initialOffCampusNet();
	DWORD		reg_setvalue();
	DWORD		MyRasDial(char*,char*,char*,char*,char*);
	DWORD		ErrorCause(DWORD dwResult);
	BOOL		ExecuteNotifyIcon(int nCode);
	DWORD		RasConnect(char*,char*,char*,char*,char*,char*);
	DWORD		MyGetAdaptersInfo(CString &info);
	DWORD		SuccessRet(CString str);
	DWORD		Initialcom(CString);
	DWORD		Reg_getvalue1();
	DWORD		Reg_getvalue2();
	DWORD		FUNC1();
// Dialog Data
	//{{AFX_DATA(CNjubrassDlg)
	enum { IDD = IDD_NJUBRASS_DIALOG };
	CString	m_MiPassWord;
	CString	m_MingPassWord;
	CString	m_MingUserName;
	CString	m_MiUserName;
	CString curusername;

	HRASCONN	hrasconn;

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNjubrassDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CNjubrassDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEncrypt();
	afx_msg void OnDecrypt();
	afx_msg void OnAboutbox();
	afx_msg void OnConnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExit();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NJUBRASSDLG_H__14318543_0999_415C_BD2E_CEB97B81B646__INCLUDED_)
