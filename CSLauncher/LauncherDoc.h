// LauncherDoc.h : interface of the CLauncherDoc class
//
#pragma once

#define USE_SUPER_SERVER

#include <vector>
#include <afxmt.h>
#include <mmsystem.h>
using std::max;
using std::min;
#include "../CSUtility/csutility.h"
#include "../CSEngine/CSEngine.h"
#include "../CSEngine/ISuperServer.h"
#include "../CSEngine/syscfg_int.h"
#include <boost/shared_ptr.hpp>

#include "HtmlLog/CyclicLog.h"

class CAnalyzerView;
class CLauncherView;

enum ViewType
{
	View_MainServer,
	View_Camera
};

typedef std::map< int, std::vector<int > >	CameraMap_t;

class CLauncherDoc : public CDocument
{
protected: // create from serialization only
	CLauncherDoc();
	DECLARE_DYNCREATE(CLauncherDoc)


	typedef std::map< int, boost::shared_ptr<CyclicLog> >	DumpMap_t;
	boost::shared_ptr<CyclicLog>							m_VideoLog;
	boost::shared_ptr<CyclicLog>							m_MainLog;
	boost::shared_ptr<CyclicLog>							m_ExchangeLog;

	typedef boost::shared_ptr<ISuperServer>				ServerPtr_t;
	ServerPtr_t										m_ServerInt;

	CString											m_sInitialFileName;
	DumpMap_t										m_DumpMap;

	template <class T>	T*	GetView()
	{
		POSITION pos = GetFirstViewPosition();
		while( pos )
		{
			CView* pView = GetNextView( pos );
			if( pView->IsKindOf( RUNTIME_CLASS( T ) ) ) return (T*)pView;
		}
		return 0;
	}

	void	CreateLogFile(int nRoomID, bool bFirst);
	void	CreateVideoLogFile( );
	void	CreateMainLogFile( );
	void	CreateExchangeLogFile( );

	bool	InitServer();
	BOOL	CreateSuperServer(CCreateContext& cc, IDebugOutput* pDebugInt, bool bEmbedded);
	BOOL	CreateRoomServers(CCreateContext& cc, IDebugOutput* pDebugInt);
	BOOL	CreateExchange(CCreateContext& cc, IDebugOutput* pDebugInt);
	CString	GetCommonLogDirectory() const;
	
	bool	IsEmbedded();
public:
// Implementation
public:
	virtual ~CLauncherDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CLauncherView*	GetLauncherView();

	BOOL	CreateMainServer( CCreateContext& cc);
	void	LogMessage( int nRoomID, int nSeverity, LPCWSTR szMessage);

	void	LogVideoMessage(  int nSeverity, LPCWSTR szMessage);
	void	LogMainMessage(  int nSeverity, LPCWSTR szMessage);
	void	LogExchangeMessage(  int nSeverity, LPCWSTR szMessage);

	void	ShutdownServer( int nRoomID );
	void	ShutdownExchange();
	BOOL	InitServer( int nRoomID, IDebugOutput* pDbgInt );

	int		GetMaxLineNumber();
protected:

	virtual void OnCloseDocument();
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnReconnect();
	afx_msg void OnReport();
};


