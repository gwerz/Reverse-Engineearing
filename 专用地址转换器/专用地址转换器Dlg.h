// 专用地址转换器Dlg.h : header file
//

#if !defined(AFX_DLG_H__93C440EC_2A67_404B_B1A3_0E707D6E4711__INCLUDED_)
#define AFX_DLG_H__93C440EC_2A67_404B_B1A3_0E707D6E4711__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMyDlg dialog

class CMyDlg : public CDialog
{
// Construction
public:
	CMyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMyDlg)
	enum { IDD = IDD_MY_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangeNormaladdr();
	afx_msg void OnChangeThunderaddr();
	afx_msg void OnChangeFlashgetaddr();
	afx_msg void OnChangeXuanfengaddr();
	afx_msg void OnChangeFeisuaddr();
	afx_msg void OnPopup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_H__93C440EC_2A67_404B_B1A3_0E707D6E4711__INCLUDED_)
