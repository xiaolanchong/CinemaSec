//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Менеджер отладочного интерфейса IDebugOutput для клиента
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 24.06.2005
//                                                                                      //
//======================================================================================//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "ClientLogManager.h"
#include "../../CSEngine/mainserver/ThreadError.h"
#include <atlconv.h>
#include "../../CSLauncher/HtmlLog/HtmlLog.h"
#include <shlwapi.h>

//======================================================================================//
//                                class ClientLogManager                                //
//======================================================================================//
class Stub_DebugOutputImpl;
class ClientLogManager;

class ClientLogManager : public IClientLogManager
{
	std::queue< Message_t >									m_MsgQueue;
	std::vector< boost::shared_ptr<Stub_DebugOutputImpl> >	m_DbgOutputs;
	SyncCriticalSection										m_Sync;
	boost::shared_ptr<HtmlLog>								m_pLogFile;
public:
	ClientLogManager();
	virtual ~ClientLogManager();

	void	LogMessage( LPCWSTR szSystem, CTime timeMsg, int messageType, LPCWSTR szMsg );
private:
	virtual IDebugOutput*						CreateDebugOutput( LPCWSTR szSystem ) ;
	virtual std::queue<Message_t>&				LockBuffer()	;
	virtual void								UnlockBuffer()	;
	virtual void								WriteMessage( const Message_t& msg) ;

	std::wstring		GetLogFileName() const;
	HtmlLog::Severity	ToLogSeverity(int nSeverity) const;
};

class Stub_DebugOutputImpl : public IDebugOutput
{
	ClientLogManager*	m_pMgr;
	std::wstring		m_sName;
	virtual void PrintW( __int32 messageType, LPCWSTR szMessage )
	{
		m_pMgr->LogMessage( m_sName.c_str(), CTime::GetCurrentTime(), messageType, szMessage );
	}
	virtual void PrintA( __int32 messageType, LPCSTR szMessage )
	{
		USES_CONVERSION;
		LPCWSTR szMessageU = A2CW( szMessage );
		Stub_DebugOutputImpl::PrintW( messageType, szMessageU );
	}
public:
	Stub_DebugOutputImpl( ClientLogManager* pMgr, LPCWSTR szSystem ): 
		m_pMgr( pMgr), 
		m_sName( szSystem ? szSystem : std::wstring() )
		{}
};


ClientLogManager::ClientLogManager()
{
	m_DbgOutputs.reserve( 128 );
}

ClientLogManager::~ClientLogManager()
{

}

IDebugOutput*		ClientLogManager::CreateDebugOutput( LPCWSTR szSystem )
{
	m_DbgOutputs.push_back( boost::shared_ptr<Stub_DebugOutputImpl>
		( new Stub_DebugOutputImpl( this, szSystem )) 
		);
	return m_DbgOutputs.back().get();
}

void ClientLogManager::LogMessage( LPCWSTR szSystem, CTime timeMsg, int messageType, LPCWSTR szMsg )
{
	const size_t c_MaxQueueSize = 10* 1000;
	std::queue<Message_t>& MsgQueue = LockBuffer();
	if( c_MaxQueueSize > MsgQueue.size() )
	{
#if 0
		OutputDebugStringW( szMsg );
		OutputDebugStringW( L"\n" );
#endif
		MsgQueue.push( Message_t( szSystem, timeMsg, messageType, szMsg? szMsg : std::wstring() ) );
	}
	UnlockBuffer();
}

std::queue<IClientLogManager::Message_t>&		ClientLogManager::LockBuffer()
{
	m_Sync.Lock();
	return m_MsgQueue;
}

void		ClientLogManager::UnlockBuffer()
{
	m_Sync.Unlock();
}

HtmlLog::Severity ClientLogManager::ToLogSeverity(int nSeverity) const
{
	switch(nSeverity)
	{
	case IDebugOutput::mt_debug_info :	return HtmlLog::sev_debug;	
	case IDebugOutput::mt_info :		return HtmlLog::sev_info;	
	case IDebugOutput::mt_warning :		return HtmlLog::sev_warning; 
	case IDebugOutput::mt_error :		return HtmlLog::sev_error;	
	default :							return HtmlLog::sev_unknown;	
	}
}


void		ClientLogManager::WriteMessage( const Message_t& msg)
{
	const size_t c_MaxLogSize = 1 * 1024 * 1024; // 1Mb
	if( !m_pLogFile || m_pLogFile->GetSize() > c_MaxLogSize )
	{
		std::wstring szName = GetLogFileName();
		m_pLogFile = boost::shared_ptr<HtmlLog>( new HtmlLog( szName.c_str(), L"CSClient" ));
	}
	if( m_pLogFile.get() )
	{
		m_pLogFile->AddRecord( ToLogSeverity( boost::get<2>( msg ) ), boost::get<1>(msg).GetTime(), boost::get<3>(msg).c_str() );
	}
}

std::wstring	ClientLogManager::GetLogFileName() const
{
	WCHAR szBuf[ 2* MAX_PATH ];
	WCHAR szLogName[ 2* MAX_PATH ];
	GetModuleFileNameW( ::GetModuleHandleW( 0 ), szBuf, 2 * MAX_PATH );
	PathRemoveFileSpecW( szBuf );
	PathCombineW( szLogName, szBuf, L"csclient_log.html" );
#if 1
	return std::wstring(szLogName);
#else
	return std::wstring( L"g:\\test.html" );
#endif
}
/////////////// stub

struct StubClientLogManager : IClientLogManager, IDebugOutput
{
//	std::wstring			m_sSystem;
	std::queue<Message_t>	m_msg;
	void PrintW( __int32 msg, LPCWSTR txt )
	{
		UNREFERENCED_PARAMETER(msg);
		UNREFERENCED_PARAMETER(txt);
	}
	void PrintA( __int32 msg, LPCSTR txt )
	{
		UNREFERENCED_PARAMETER(msg);
		UNREFERENCED_PARAMETER(txt);
	}

	MyDebugOutputImpl m_Debug;
	virtual IDebugOutput*						CreateDebugOutput( LPCWSTR szSystem )
	{
		UNREFERENCED_PARAMETER(szSystem);
		return this;
	}

	virtual std::queue<Message_t>&				LockBuffer()	{ return m_msg;}
	virtual void								UnlockBuffer()	{}
	virtual void								WriteMessage( const Message_t& msg) { UNREFERENCED_PARAMETER(msg); }
};

////////////////////

IClientLogManager& GetLogManager()
{
#if 1
	static ClientLogManager mng;
#else
	static StubClientLogManager mng;
#endif
	return mng;
}