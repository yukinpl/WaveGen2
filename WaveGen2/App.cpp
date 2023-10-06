﻿#include "pch.h"
#include "framework.h"
#include "App.h"
#include "CMainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




BEGIN_MESSAGE_MAP( App , CWinApp )
	ON_COMMAND( ID_HELP , &CWinApp::OnHelp )
END_MESSAGE_MAP()




App::App()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART ;
}



App theApp ;



BOOL App::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls ;
	InitCtrls.dwSize = sizeof( InitCtrls ) ;
	InitCtrls.dwICC  = ICC_WIN95_CLASSES ;
	InitCommonControlsEx( &InitCtrls ) ;

	CWinApp::InitInstance() ;


	AfxEnableControlContainer() ;

	CShellManager * pShellManager = new CShellManager ;

	CMFCVisualManager::SetDefaultManager( RUNTIME_CLASS( CMFCVisualManagerWindows ) ) ;

	CMainDlg dlg ;
	m_pMainWnd = & dlg ;
	INT_PTR nResponse = dlg.DoModal() ;
	if( nResponse == IDOK )
	{

	}
	else if( nResponse == IDCANCEL )
	{

	}
	else if( nResponse == -1 )
	{
		TRACE( traceAppMsg , 0 , 
			"경고: 대화 상자를 만들지 못했으므로 애플리케이션이 예기치 않게 종료됩니다.\n" ) ;

		TRACE( traceAppMsg , 0 , 
			"경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n" ) ;
	}


	if( pShellManager != nullptr )
	{
		delete pShellManager ;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp() ;
#endif


	return FALSE ;
}

