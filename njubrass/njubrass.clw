; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CNjubrassDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "njubrass.h"

ClassCount=3
Class1=CNjubrassApp
Class2=CNjubrassDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_MESSAGE
Resource4=IDD_NJUBRASS_DIALOG
Resource5=IDD_NJUMail

[CLS:CNjubrassApp]
Type=0
HeaderFile=njubrass.h
ImplementationFile=njubrass.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CNjubrassDlg]
Type=0
HeaderFile=njubrassDlg.h
ImplementationFile=njubrassDlg.cpp
Filter=D
LastObject=IDC_OriUsername
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=njubrassDlg.h
ImplementationFile=njubrassDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=6
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342177296
Control6=IDC_STATIC,static,1342308352

[DLG:IDD_NJUBRASS_DIALOG]
Type=1
Class=CNjubrassDlg
ControlCount=26
Control1=IDC_UserName,edit,1350631552
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_PassWord,edit,1350631552
Control5=IDC_STATIC,button,1342177287
Control6=IDC_UserName2,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_PassWord2,edit,1350631552
Control10=IDC_STATIC,button,1342177287
Control11=IDC_Encrypt,button,1342242816
Control12=IDC_Decrypt,button,1342242816
Control13=IDC_PIC,static,1342177294
Control14=IDC_LabelUsername,static,1342308352
Control15=IDC_LabelPassword,static,1342308352
Control16=IDC_OriUsername,edit,1350631552
Control17=IDC_OriPassword,edit,1350631584
Control18=IDC_CHECK_save,button,1342242819
Control19=IDC_CHECK_off,button,1342242819
Control20=IDC_Connect,button,1342242817
Control21=IDC_Exit,button,1342242816
Control22=IDC_Help,button,1342242816
Control23=IDS_ABOUTBOX,button,1342242816
Control24=IDC_STATIC,static,1342177296
Control25=IDC_STATIC,static,1342177296
Control26=IDC_interpretation,static,1342308352

[DLG:IDD_MESSAGE]
Type=1
Class=?
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[DLG:IDD_NJUMail]
Type=1
Class=?
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_AutoCheck,button,1342242819
Control4=IDC_Mail,static,1350696960
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_Minute,static,1350696960
Control8=IDC_STATIC,static,1342308352

