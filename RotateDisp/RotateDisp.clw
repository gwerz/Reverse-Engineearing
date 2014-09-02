; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CRotateDispWnd
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "RotateDisp.h"

ClassCount=3
Class1=CRotateDispApp
Class2=CRotateDispDlg

ResourceCount=3
Resource1=IDR_MAINFRAME
Resource2=IDM_MAINMENU
Class3=CRotateDispWnd
Resource3=IDD_ROTATEDISP_DIALOG

[CLS:CRotateDispApp]
Type=0
HeaderFile=RotateDisp.h
ImplementationFile=RotateDisp.cpp
Filter=N

[CLS:CRotateDispDlg]
Type=0
HeaderFile=RotateDispDlg.h
ImplementationFile=RotateDispDlg.cpp
Filter=D
LastObject=ID_ROTATEDISP
BaseClass=CDialog
VirtualFilter=dWC

[DLG:IDD_ROTATEDISP_DIALOG]
Type=1
Class=CRotateDispDlg
ControlCount=6
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_WEBSITE,button,1342242816
Control6=IDC_STATIC,static,1342181889

[MNU:IDM_MAINMENU]
Type=1
Class=?
Command1=ID_ROTATEDISP
Command2=ID_VERSIONINFO
Command3=ID_EXIT
CommandCount=3

[CLS:CRotateDispWnd]
Type=0
HeaderFile=RotateDispWnd.h
ImplementationFile=RotateDispWnd.cpp
BaseClass=CWnd
Filter=W
VirtualFilter=dWC
LastObject=ID_VERSIONINFO

