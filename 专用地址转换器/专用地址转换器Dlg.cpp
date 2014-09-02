// 专用地址转换器Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "专用地址转换器.h"
#include "专用地址转换器Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyDlg dialog

CMyDlg::CMyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyDlg, CDialog)
	//{{AFX_MSG_MAP(CMyDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_NORMALADDR, OnChangeNormaladdr)
	ON_EN_CHANGE(IDC_THUNDERADDR, OnChangeThunderaddr)
	ON_EN_CHANGE(IDC_FLASHGETADDR, OnChangeFlashgetaddr)
	ON_EN_CHANGE(IDC_XUANFENGADDR, OnChangeXuanfengaddr)
	ON_EN_CHANGE(IDC_FEISUADDR, OnChangeFeisuaddr)
	ON_BN_CLICKED(IDC_POPUP, OnPopup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDlg message handlers

BOOL CMyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_NORMALADDR)->SetWindowText("http://www.shuax.com/");
	return TRUE;  // return TRUE  unless you set the focus to a control
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyDlg::OnPaint() 
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
HCURSOR CMyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL IsProcessing=FALSE;
char changer[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int __fastcall translate1(char* src,char* dst,int srcsize)
{
	_asm
	{
		mov		esi,edx
		mov		edi,ecx
		push	edi
		mov		ecx,srcsize
		mov		ebx,offset changer
processalign://此处到sign1:将字节对齐部分转码
		cmp		ecx,4h
		jb		sign1
		sub		ecx,3h
		lodsd
		dec		esi
		bswap	eax
		rol		eax,6h
		and		al,3Fh
		xlat
		stosb
		rol		eax,6h
		and		al,3Fh
		xlat
		stosb
		rol		eax,6h
		and		al,3Fh
		xlat
		stosb
		rol		eax,6h
		and		al,3Fh
		xlat
		stosb
		jmp		processalign

sign1://此处到sign3:将最后几个字节存储到eax中
		mov		edx,ecx
sign2:
		dec		ecx
		jl		sign3
		lodsb
		shl		eax,8h
		jmp		sign2
sign3:	
		mov		ecx,edx
		sub		ecx,3h
		neg		ecx
		mov		edx,ecx
sign4:
		dec		ecx
		jl		sign5
		shl		eax,8h
		jmp		sign4
sign5://将剩余部分转码
		rol		eax,6h
		and		al,3Fh
		xlat
		stosb
		rol		eax,6h
		and		al,3Fh
		xlat
		stosb
		rol		eax,6h
		and		al,3Fh
		xlat
		stosb
		rol		eax,6h
		and		al,3Fh
		xlat
		stosb
		sub		edi,edx
		mov		al,'='
		mov		ecx,edx
		repne	stosb
		mov		al,0
		stosb
		pop		eax
		sub		eax,edi
		neg		eax
		dec		eax
	}
}

int _fastcall translate2(char* src,char* dst,int srcsize)
{
	_asm
	{
		mov		esi,edx
		mov		edi,ecx
		mov		ebx,ecx
		mov		ecx,srcsize
		shr		ecx,2h
flag1:
		dec		ecx
		jl		flag10
		xor		edx,edx
		lodsd
flag2:
		cmp		al,'A'
		jb		flag4
		cmp		al,'Z'
		jg		flag3
		sub		al,'A'
		jmp		flag9
flag3:
		cmp		al,'a'
		jb		flag7
		cmp		al,'z'
		jg		flag7
		sub		al,'G'
		jmp		flag9
flag4:
		cmp		al,'0'
		jb		flag5
		cmp		al,'9'
		jg		flag7
		sub		al,0FCh
		jmp		flag9
flag5:
		cmp		al,'+'
		jnz		flag6
		mov		al,'>'
		jmp		flag9
flag6:
		cmp		al,'/'
		jnz		flag7
		mov		al,'?'
		jmp		flag9
flag7:
		xor		ecx,ecx
flag8:
		dec		ecx
		shl		edx,6h
		shr		eax,8h
		jnz		flag8
		shl		edx,2h
		mov		eax,edx
		bswap	eax
		stosd
		sub		ebx,edi
		neg		ebx
		lea		ebx,[ebx+ecx-1]
		jmp		flag11
flag9:
		or		dl,al
		shl		edx,6h
		shr		eax,8h
		jnz		flag2
		shl		edx,2h
		bswap	edx
		mov		eax,edx
		stosd
		dec		edi
		jmp		flag1
flag10:
		sub		ebx,edi
		neg		ebx
flag11:
		mov		eax,ebx
	}
}

void CMyDlg::OnChangeNormaladdr() 
{
	char datasolver1[1024];
	char datasolver2[1024];
	char datasolver3[1024];
	if(IsProcessing == FALSE)
	{
		IsProcessing=TRUE;
		int len=GetDlgItem(IDC_NORMALADDR)->GetWindowTextLength()+1;
		GetDlgItem(IDC_NORMALADDR)->GetWindowText(datasolver1,len);

		strcpy(datasolver2,"AA");
		strcat(datasolver2,datasolver1);
		strcat(datasolver2,"ZZ");
		datasolver3[translate1(datasolver3,datasolver2,strlen(datasolver2))]='\0';
		strcpy(datasolver2,"thunder://");
		strcat(datasolver2,datasolver3);
		GetDlgItem(IDC_THUNDERADDR)->SetWindowText(datasolver2);

		strcpy(datasolver2,"[FLASHGET]");
		strcat(datasolver2,datasolver1);
		strcat(datasolver2,"[FLASHGET]");
		datasolver3[translate1(datasolver3,datasolver2,strlen(datasolver2))]='\0';
		strcpy(datasolver2,"flashget://");
		strcat(datasolver2,datasolver3);
		GetDlgItem(IDC_FLASHGETADDR)->SetWindowText(datasolver2);

		datasolver3[translate1(datasolver3,datasolver1,strlen(datasolver1))]='\0';
		strcpy(datasolver2,"qqdl://");
		strcat(datasolver2,datasolver3);
		GetDlgItem(IDC_XUANFENGADDR)->SetWindowText(datasolver2);

		strrev(datasolver1);
		datasolver1[strlen(datasolver1)-7]='\0';
		strrev(datasolver1);
		datasolver3[translate1(datasolver3,datasolver1,strlen(datasolver1))]='\0';
		strcpy(datasolver2,"fs2you://");
		strcat(datasolver2,datasolver3);
		GetDlgItem(IDC_FEISUADDR)->SetWindowText(datasolver2);
		IsProcessing=FALSE;
	}
}

void CMyDlg::OnChangeThunderaddr() 
{
	char datasolver1[1024];
	char datasolver2[1024];
	char datasolver3[1024];
	if(IsProcessing == FALSE)
	{
		IsProcessing=TRUE;
		int len=GetDlgItem(IDC_THUNDERADDR)->GetWindowTextLength()+1;
		GetDlgItem(IDC_THUNDERADDR)->GetWindowText(datasolver2,len);
		strrev(datasolver2);
		datasolver2[strlen(datasolver2)-10]='\0';
		strrev(datasolver2);

		char* ptr=datasolver3+translate2(datasolver2,datasolver3,len);
		*(ptr-2)='\0';
		strrev(datasolver3);
		*(ptr-4)='\0';
		strrev(datasolver3);
		GetDlgItem(IDC_NORMALADDR)->SetWindowText(datasolver3);

		strcpy(datasolver2,datasolver3);
		strcpy(datasolver1,"[FLASHGET]");
		strcat(datasolver1,datasolver2);
		strcat(datasolver1,"[FLASHGET]");
		datasolver3[translate1(datasolver3,datasolver1,strlen(datasolver1))]='\0';
		strcpy(datasolver1,"flashget://");
		strcat(datasolver1,datasolver3);
		GetDlgItem(IDC_FLASHGETADDR)->SetWindowText(datasolver1);

		datasolver3[translate1(datasolver3,datasolver2,strlen(datasolver2))]='\0';
		strcpy(datasolver1,"qqdl://");
		strcat(datasolver1,datasolver3);
		GetDlgItem(IDC_XUANFENGADDR)->SetWindowText(datasolver1);

		strrev(datasolver2);
		datasolver2[strlen(datasolver2)-7]='\0';
		strrev(datasolver2);
		datasolver3[translate1(datasolver3,datasolver2,strlen(datasolver2))]='\0';
		strcpy(datasolver1,"fs2you://");
		strcat(datasolver1,datasolver3);
		GetDlgItem(IDC_FEISUADDR)->SetWindowText(datasolver1);
		IsProcessing=FALSE;
	}
}

void CMyDlg::OnChangeFlashgetaddr() 
{
	char datasolver1[1024];
	char datasolver2[1024];
	char datasolver3[1024];
	if(IsProcessing == FALSE)
	{
		IsProcessing=TRUE;
		int len=GetDlgItem(IDC_FLASHGETADDR)->GetWindowTextLength()+1;
		GetDlgItem(IDC_FLASHGETADDR)->GetWindowText(datasolver2,len);
		char* ptr=datasolver2;
		while(*ptr)
		{
			if(*ptr == '&')
				*ptr='\0';
			ptr++;
		}
		strrev(datasolver2);
		datasolver2[strlen(datasolver2)-11]='\0';
		strrev(datasolver2);

		ptr=datasolver3+translate2(datasolver2,datasolver3,len);
		*(ptr-10)='\0';
		strrev(datasolver3);
		*(ptr-20)='\0';
		strrev(datasolver3);
		GetDlgItem(IDC_NORMALADDR)->SetWindowText(datasolver3);
		
		strcpy(datasolver2,datasolver3);
		strcpy(datasolver1,"AA");
		strcat(datasolver1,datasolver2);
		strcat(datasolver1,"ZZ");
		datasolver3[translate1(datasolver3,datasolver1,strlen(datasolver1))]='\0';
		strcpy(datasolver1,"thunder://");
		strcat(datasolver1,datasolver3);
		GetDlgItem(IDC_THUNDERADDR)->SetWindowText(datasolver1);

		datasolver3[translate1(datasolver3,datasolver2,strlen(datasolver2))]='\0';
		strcpy(datasolver1,"qqdl://");
		strcat(datasolver1,datasolver3);
		GetDlgItem(IDC_XUANFENGADDR)->SetWindowText(datasolver1);

		strrev(datasolver2);
		datasolver2[strlen(datasolver2)-7]='\0';
		strrev(datasolver2);
		datasolver3[translate1(datasolver3,datasolver2,strlen(datasolver2))]='\0';
		strcpy(datasolver1,"fs2you://");
		strcat(datasolver1,datasolver3);
		GetDlgItem(IDC_FEISUADDR)->SetWindowText(datasolver1);
		IsProcessing=FALSE;
	}
}

void CMyDlg::OnChangeXuanfengaddr() 
{
	char datasolver1[1024];
	char datasolver2[1024];
	char datasolver3[1024];
	if(IsProcessing == FALSE)
	{
		IsProcessing=TRUE;
		int len=GetDlgItem(IDC_XUANFENGADDR)->GetWindowTextLength()+1;
		GetDlgItem(IDC_XUANFENGADDR)->GetWindowText(datasolver3,len);
		strrev(datasolver3);
		datasolver3[strlen(datasolver3)-7]='\0';
		strrev(datasolver3);

		translate2(datasolver3,datasolver1,len);
		GetDlgItem(IDC_NORMALADDR)->SetWindowText(datasolver1);

		strcpy(datasolver3,datasolver1);
		strcpy(datasolver2,"AA");
		strcat(datasolver2,datasolver3);
		strcat(datasolver2,"ZZ");
		datasolver1[translate1(datasolver1,datasolver2,strlen(datasolver2))]='\0';
		strcpy(datasolver2,"thunder://");
		strcat(datasolver2,datasolver1);
		GetDlgItem(IDC_THUNDERADDR)->SetWindowText(datasolver2);

		strcpy(datasolver2,"[FLASHGET]");
		strcat(datasolver2,datasolver3);
		strcat(datasolver2,"[FLASHGET]");
		datasolver1[translate1(datasolver1,datasolver2,strlen(datasolver2))]='\0';
		strcpy(datasolver2,"flashget://");
		strcat(datasolver2,datasolver1);
		GetDlgItem(IDC_FLASHGETADDR)->SetWindowText(datasolver2);

		strrev(datasolver3);
		datasolver3[strlen(datasolver3)-7]='\0';
		strrev(datasolver3);
		datasolver1[translate1(datasolver1,datasolver3,strlen(datasolver3))]='\0';
		strcpy(datasolver2,"fs2you://");
		strcat(datasolver2,datasolver1);
		GetDlgItem(IDC_FEISUADDR)->SetWindowText(datasolver2);
		IsProcessing=FALSE;
	}
}

void CMyDlg::OnChangeFeisuaddr() 
{	
	char datasolver1[1024];
	char datasolver2[1024];
	char datasolver3[1024];
	if(IsProcessing == FALSE)
	{
		IsProcessing=TRUE;
		int len=GetDlgItem(IDC_FEISUADDR)->GetWindowTextLength()+1;
		GetDlgItem(IDC_FEISUADDR)->GetWindowText(datasolver1,len);
		strrev(datasolver1);
		datasolver1[strlen(datasolver1)-9]='\0';
		strrev(datasolver1);

		translate2(datasolver1,datasolver3,len);
		strcpy(datasolver2,"http://");
		strcat(datasolver2,datasolver3);
		strcpy(datasolver3,datasolver2);
		char* ptr=datasolver3;
		while(*ptr)
		{
			if(*ptr == '|')
				*ptr='\0';
			ptr++;
		}
		GetDlgItem(IDC_NORMALADDR)->SetWindowText(datasolver3);

		strcpy(datasolver1,datasolver3);
		strcpy(datasolver2,"AA");
		strcat(datasolver2,datasolver1);
		strcat(datasolver2,"ZZ");
		datasolver3[translate1(datasolver3,datasolver2,strlen(datasolver2))]='\0';
		strcpy(datasolver2,"thunder://");
		strcpy(datasolver2,datasolver3);
		GetDlgItem(IDC_THUNDERADDR)->SetWindowText(datasolver2);

		strcpy(datasolver2,"[FLASHGET]");
		strcat(datasolver2,datasolver1);
		strcat(datasolver2,"[FLASHGET]");
		datasolver3[translate1(datasolver3,datasolver2,strlen(datasolver2))]='\0';
		strcpy(datasolver2,"flashget://");
		strcat(datasolver2,datasolver3);
		GetDlgItem(IDC_FLASHGETADDR)->SetWindowText(datasolver2);

		datasolver3[translate1(datasolver3,datasolver1,strlen(datasolver1))]='\0';
		strcpy(datasolver2,"qqdl://");
		strcat(datasolver2,datasolver3);
		GetDlgItem(IDC_XUANFENGADDR)->SetWindowText(datasolver2);
		IsProcessing=FALSE;
	}
}

void CMyDlg::OnPopup() 
{
	char content[1000];
	GetDlgItem(IDC_NORMALADDR)->GetWindowText(content,GetDlgItem(IDC_NORMALADDR)->GetWindowTextLength()+1);
	ShellExecute(NULL,NULL,content,NULL,NULL,SW_MAXIMIZE);
}

BOOL CMyDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYFIRST)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return 1;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
																									
																			          