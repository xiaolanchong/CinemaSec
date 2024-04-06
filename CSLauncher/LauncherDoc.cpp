// LauncherDoc.cpp : implementation of the CLauncherDoc class
//

#include "stdafx.h"
#include "CSLauncher.h"

#include <shlwapi.h>
#include <shlobj.h>
#include <winsock2.h>

#include "LauncherDoc.h"
#include "MainFrm.h"
#include <afxpriv.h>

#include "TabViews/LauncherView.h"
#include "TabViews/RoomView.h"
#include "TabViews/SettingsView.h"
#include "TabViews/VideoLogView.h"
#include "TabViews/AnalyzerView.h"
#include "TabViews/ExchangeLogView.h"

#include "Exchange\Exchange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLauncherDoc

IMPLEMENT_DYNCREATE(CLauncherDoc, CDocument)

BEGIN_MESSAGE_MAP(CLauncherDoc, CDocument)
	ON_COMMAND( ID_RECONNECT, OnReconnect )
	ON_COMMAND( ID_REPORT, OnReport )
END_MESSAGE_MAP()

// CLauncherDoc construction/destruction

CLauncherDoc::CLauncherDoc()
{
	// TODO: add one-time construction code here
}
CLauncherDoc::~CLauncherDoc()
{
}
// CLauncherDoc diagnostics

#ifdef _DEBUG
void CLauncherDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLauncherDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

#ifndef USE_SUPER_SERVER
void DestroyMainServer(IMainServer* p)
{
	p->Release();
}
#endif

bool	CLauncherDoc::IsEmbedded()
{
	CCommandLineInfo cmd;
	AfxGetApp()->ParseCommandLine(cmd);
#if 1 // 1 is for real
	return cmd.m_bRunEmbedded == TRUE;
#else
	return TRUE;
#endif
}

BOOL	CLauncherDoc::CreateMainServer( CCreateContext& cc )
{
	// NOTE : One CLauncherView is already created, attach info to him
	bool bEmbedded = IsEmbedded();
	if( !bEmbedded )
		CreateVideoLogFile();
	CreateMainLogFile( );
	CreateExchangeLogFile();

	POSITION pos = GetFirstViewPosition();
	ASSERT(pos);
	CView* pView = GetNextView(pos);
	CLauncherView * pLauncherView = STATIC_DOWNCAST( CLauncherView, pView );
	CMainFrame *	pFrame = STATIC_DOWNCAST( CMainFrame, pLauncherView->GetParentFrame() );

	cc.m_pCurrentFrame	= pFrame;
	cc.m_pCurrentDoc	= this;

	boost::shared_ptr<SelectionTag> Tag ( new LauncherTag);
	pFrame->AddTab( Tag );
	pLauncherView->SetTag( Tag );

	MyDebugOutputImpl DebugImpl;
	DebugImpl.Set( pLauncherView );

	BOOL bRes = CreateSuperServer( cc, pLauncherView, bEmbedded );
//	if( !bRes ) return FALSE;
	if( bRes && !bEmbedded )
	{
		CreateRoomServers(cc, pLauncherView);
	}
	return CreateExchange( cc, pLauncherView );
}

BOOL	CLauncherDoc::CreateSuperServer( CCreateContext& cc, IDebugOutput* pDebugInt, bool bEmbedded )
{
	MyDebugOutputImpl DebugImpl;
	DebugImpl.Set( pDebugInt );

	CMainFrame * pFrame = STATIC_DOWNCAST( CMainFrame,cc.m_pCurrentFrame );
	CVideoLogView* pVideoView = NULL;
	if( !bEmbedded )
	{
		cc.m_pNewViewClass	= RUNTIME_CLASS(CVideoLogView);
		pVideoView = (CVideoLogView*)pFrame->CreateView( &cc, AFX_IDW_PANE_FIRST + 1 );
		pVideoView->SendMessage( WM_INITIALUPDATE );	
		boost::shared_ptr<SelectionTag> VideoTagZ( new VideoTag );
		pVideoView->SetTag( VideoTagZ );
		pFrame->AddTab( VideoTagZ );
	}

	HRESULT hr;
	DWORD res;

	// first create superserver for video

	ISuperServer* pSuperServer;
	res = CreateEngineInterface( SUPER_SERVER_INTERFACE, (void**)&pSuperServer  );
	if ( res != 0 || !pSuperServer)	
	{
		DebugImpl.PrintW( IDebugOutput::mt_error, L"Failed to create Super server");
		m_ServerInt.reset();
		m_DumpMap.clear();
		return FALSE;
	}
	ServerPtr_t Tmp( pSuperServer, DestroySuperServer );

	hr = pSuperServer->Initialize(  pDebugInt, pVideoView, !bEmbedded );
	if( hr != S_OK  )
	{
		return FALSE;
	}
	pSuperServer->RegisterCallback( GetCallbackServerInt() );
	m_ServerInt = Tmp;
	return TRUE;
}

BOOL	CLauncherDoc::CreateRoomServers(CCreateContext& cc, IDebugOutput* pDebugInt)
{
	MyDebugOutputImpl DebugImpl;
	DebugImpl.Set( pDebugInt );

	cc.m_pCurrentDoc	= this;
	cc.m_pNewViewClass	= RUNTIME_CLASS(CRoomView);
	CMainFrame * pFrame = STATIC_DOWNCAST( CMainFrame,cc.m_pCurrentFrame );

	DWORD res;
	HRESULT hr;
	ISystemConfig* pCfgInt ;
	res = CreateEngineInterface( SYSTEM_DB_CONFIG_INTERFACE_0, (void**)&pCfgInt );
	if( res )
	{
		DebugImpl.PrintW( IDebugOutput::mt_error, L"Failed to create config interface" );
		return FALSE;
	}
	else
	{
		DebugImpl.PrintW( IDebugOutput::mt_info, L"Config interface was created" );
	}
	boost::shared_ptr< ISystemConfig > pSysCfgIntWrap( pCfgInt, DestroySystemConfigInt );
	pCfgInt->SetDebugInterface( pDebugInt );

	std::vector<int> RoomIDArr;
	hr = pCfgInt->GetRoomID(RoomIDArr);
	if ( hr != S_OK )	
	{
		DebugImpl.PrintW( IDebugOutput::mt_error, L"ISystemConfig::GetRoomID failed, reason=%d", hr);
		return FALSE;
	}

	hr = pCfgInt->LoadFromDB( true, false );
	if( hr )
	{
		DebugImpl.PrintW( IDebugOutput::mt_error, L"Failed to load config from db, reason=%x", hr );
		return E_FAIL;
	}	
	std::vector<LocalCameraConfig>	LocalCameras;
	for( size_t i = 0; i < RoomIDArr.size(); ++i )
	{
		int nRoomID = RoomIDArr[i];
		hr = pCfgInt->GetLocalCamera( nRoomID, LocalCameras );
		if( hr != S_OK )
		{
			DebugImpl.PrintW( IDebugOutput::mt_error, L"Failed to load local camera config, reason=%x", hr );
			return E_FAIL;
		}
	}

	CRoomView* pRoomView = 0;
	cc.m_pNewViewClass	= RUNTIME_CLASS(CRoomView);
	for( size_t i = 0; i < RoomIDArr.size(); ++i )
	{
		int nRoomID = RoomIDArr[i];
		pRoomView = (CRoomView*)pFrame->CreateView( &cc, AFX_IDW_PANE_FIRST + 1 );
		pRoomView->SetRoomID( nRoomID );
		pRoomView->SendMessage( WM_INITIALUPDATE );	
		CreateLogFile( nRoomID, true);
		boost::shared_ptr<SelectionTag> Tag ( new RoomTag(nRoomID) );
		pFrame->AddTab( Tag );
		pRoomView->SetTag( Tag );
	}
	// after all room views create their camera views
	cc.m_pNewViewClass	= RUNTIME_CLASS(CAnalyzerView);
	for( size_t i = 0; i < RoomIDArr.size(); ++i )
	{
		int nRoomID = RoomIDArr[i];
		std::vector<LocalCameraConfig> lc;
		pCfgInt->GetLocalCamera( nRoomID, lc );
		for( size_t j = 0; j < lc.size(); ++j )
		{
			int nCameraID = lc[j].m_nCameraNo;
			CAnalyzerView* pCamView = (CAnalyzerView*)pFrame->CreateView( &cc, AFX_IDW_PANE_FIRST + 1 );
			//pCamView->SetCameraID( nRoomID );
			pCamView->SendMessage( WM_INITIALUPDATE );	
			boost::shared_ptr<SelectionTag> Tag ( new CameraTag( nCameraID, nRoomID) );
			pFrame->AddTab( Tag );
			pCamView->SetTag( Tag );
			m_ServerInt->SetCameraWindow( nRoomID, nCameraID, pCamView->GetSafeHwnd() );
		}
	}

	cc.m_pNewViewClass	= RUNTIME_CLASS(CSettingsView);
	CSettingsView* pSetView = (CSettingsView*)pFrame->CreateView( &cc, AFX_IDW_PANE_FIRST + 1 );
	pSetView->SendMessage( WM_INITIALUPDATE );	
	boost::shared_ptr<SelectionTag> SettingTag( new SettingsTag );
	pSetView->SetTag( SettingTag );
	pFrame->AddTab( SettingTag );

	if( pCfgInt  )
	{
		std::wstring s;
		HRESULT hr = pCfgInt->GetConnectionString( s );
		pSetView->SetConnectionString( hr == S_OK ? s.c_str() :  L"<ERROR : Failed to get connection string>" );
	}
	return TRUE;
}

BOOL	CLauncherDoc::CreateExchange(CCreateContext& cc, IDebugOutput* pDebugInt)
{
	cc.m_pNewViewClass	= RUNTIME_CLASS(CExchangeLogView);
	CMainFrame * pFrame = STATIC_DOWNCAST( CMainFrame,cc.m_pCurrentFrame );
	CExchangeLogView* pExchangeView = (CExchangeLogView*)pFrame->CreateView( &cc, AFX_IDW_PANE_FIRST + 1 );
	pExchangeView->SendMessage( WM_INITIALUPDATE );	
	boost::shared_ptr<SelectionTag> ExchTag( new ExchangeTag );
	pExchangeView->SetTag( ExchTag );
	pFrame->AddTab( ExchTag );
	StartExchange( pExchangeView );
	return TRUE;
}

HtmlLog::Severity ToLogSeverity(int nSeverity)
{
	HtmlLog::Severity sev;
	switch(nSeverity)
	{
	case IDebugOutput::mt_debug_info :	sev = HtmlLog::sev_debug;	break;
	case IDebugOutput::mt_info :		sev = HtmlLog::sev_info;	break;
	case IDebugOutput::mt_warning :		sev = HtmlLog::sev_warning; break;
	case IDebugOutput::mt_error :		sev = HtmlLog::sev_error;	break;
	default :							sev = HtmlLog::sev_unknown;	break;
	}
	return sev;
}

void	CLauncherDoc::LogMessage( int nRoomID, int nSeverity, LPCWSTR szMessage)
{
	DumpMap_t::iterator it = m_DumpMap.find( nRoomID );
	ASSERT( it != m_DumpMap.end() );
	CyclicLog* pDump = it->second.get();
	{
		HtmlLog::Severity sev = ToLogSeverity( nSeverity );
		__time64_t timeNow = _time64(0);
		pDump->AddRecord( sev, timeNow, szMessage );
#if 0 // test log
		pDump->AddRecord( sev, timeNow, szMessage );
		pDump->AddRecord( sev, timeNow, szMessage );
		pDump->AddRecord( sev, timeNow, szMessage );
		pDump->AddRecord( sev, timeNow, szMessage );
		pDump->AddRecord( sev, timeNow, szMessage );
		pDump->AddRecord( sev, timeNow, szMessage );
#endif
	}
}

void	CLauncherDoc::LogVideoMessage( int nSeverity, LPCWSTR szMessage)
{
	if( !m_VideoLog ) return;
	HtmlLog::Severity sev = ToLogSeverity( nSeverity );
	__time64_t timeNow = _time64(0);
	m_VideoLog->AddRecord( sev, timeNow, szMessage );
}

void	CLauncherDoc::LogMainMessage( int nSeverity, LPCWSTR szMessage)
{
	if( !m_MainLog ) return;
	HtmlLog::Severity sev = ToLogSeverity( nSeverity );
	__time64_t timeNow = _time64(0);
	m_MainLog->AddRecord( sev, timeNow, szMessage );
}

void	CLauncherDoc::LogExchangeMessage( int nSeverity, LPCWSTR szMessage)
{
	if( !m_MainLog ) return;
	HtmlLog::Severity sev = ToLogSeverity( nSeverity );
	__time64_t timeNow = _time64(0);
	m_ExchangeLog->AddRecord( sev, timeNow, szMessage );
}

void	CLauncherDoc::CreateLogFile( int nRoomID, bool bFirst )
{
	if( bFirst )
	{
		CString sCommonDir = GetCommonLogDirectory();
		CString s;
		s.Format( _T("Room %d"), nRoomID );
		boost::shared_ptr<CyclicLog> logfile( new CyclicLog(sCommonDir, s, 50, 256 * 1024 ) );
		boost::shared_ptr<CyclicLog>& lastfile = m_DumpMap[ nRoomID ];
		lastfile = logfile;
	}
}

void	CLauncherDoc::CreateVideoLogFile( )
{
	CString sCommonDir = GetCommonLogDirectory();
	m_VideoLog = boost::shared_ptr<CyclicLog> ( new CyclicLog(sCommonDir, L"Video", 50, 256 * 1024 ) );
}

void	CLauncherDoc::CreateMainLogFile( )
{
	CString sCommonDir = GetCommonLogDirectory();
	m_MainLog = boost::shared_ptr<CyclicLog> ( new CyclicLog(sCommonDir, L"Main", 50, 256 * 1024 ) );
}

void	CLauncherDoc::CreateExchangeLogFile( )
{
	CString sCommonDir = GetCommonLogDirectory();
	m_ExchangeLog = boost::shared_ptr<CyclicLog> ( new CyclicLog(sCommonDir, L"Exchange", 50, 256 * 1024 ) );
}

void	CLauncherDoc::ShutdownServer( int nRoomID )
{
	ASSERT(nRoomID >= 0);
	if( m_ServerInt.get() )
	{
		m_ServerInt->UnregisterCallback();
		m_ServerInt->StopServer( nRoomID );
	}
}

BOOL	CLauncherDoc::InitServer( int nRoomID, IDebugOutput* pDbgInt )
{
	ASSERT(nRoomID >= 0);
	HRESULT hr;
	hr = m_ServerInt->StartServer( nRoomID, pDbgInt, L"" );
	return hr == S_OK ? TRUE : FALSE;
}

int		CLauncherDoc::GetMaxLineNumber()
{
	CSettingsView* pView = GetView<CSettingsView>();
	return pView ? pView->GetMaxLineNumber() : 100;
}

CString CLauncherDoc::GetCommonLogDirectory() const
{
	TCHAR szBuf[MAX_PATH];
	GetModuleFileName( GetModuleHandle(NULL), szBuf, MAX_PATH );
	PathRemoveFileSpec( szBuf );
	PathAppend( szBuf, L"Launcher_Dump" );
	return CString(szBuf);
}

void	CLauncherDoc::ShutdownExchange()
{
//	DestroyExchange();
}

void CLauncherDoc::OnCloseDocument()
{
	DestroyExchange();
	m_ServerInt.reset();
//	int *p = 0;
//	(*p) = 0;
	CLauncherView* pView = GetView< CLauncherView >( );
	ASSERT(pView);
	m_MainLog->AddRecord( HtmlLog::sev_info, _time64(0), L"Application exited normally" );
	
	CDocument::OnCloseDocument();
}

void CLauncherDoc::OnReconnect()
{
	HRESULT hr = GetCallbackServerInt()->Reconnect();
	if( hr == S_FALSE )
	{
		AfxMessageBox( _T("Already connected"));
	}
	else if( hr != S_OK )
	{
		AfxMessageBox(_T("Failed"));
	}
}

void CLauncherDoc::OnReport()
{
	HRESULT hr = GetCallbackServerInt()->Report();
	if( hr == S_FALSE )
	{
		AfxMessageBox(_T("S_FALSE"));
	}
	else if( hr != S_OK )
	{
		AfxMessageBox(_T("Failed"));
	}
}