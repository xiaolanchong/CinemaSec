//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Description is not available
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 12.08.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "IInputMessage.h"
#include "../CSEngine/mainserver/ThreadError.h"
//======================================================================================//
//                                 struct IInputMessage                                 //
//======================================================================================//

class CInputBufferImpl : public IInputBuffer
{
public:
	CInputBufferImpl();
private:
	virtual		InputBuffer_t&	Lock()		;
	virtual		void			Unlock()	;
	virtual		void			RegisterWnd( HWND hWnd, UINT msg) ;
	virtual		bool			PutIncomingMessage( const std::wstring& sText) ;

	SyncCriticalSection			m_Sync;
	std::pair<HWND, UINT>		m_RegisterWnd;
	InputBuffer_t				m_Buffer;
};

CInputBufferImpl::CInputBufferImpl():
	m_RegisterWnd( 0, 0 )
{
}

CInputBufferImpl::InputBuffer_t&	CInputBufferImpl::Lock()
{
	m_Sync.Lock();
	return m_Buffer;
}

void			CInputBufferImpl::Unlock()
{
	m_Sync.Unlock();
}

void			CInputBufferImpl::RegisterWnd( HWND hWnd, UINT msg)
{
	m_RegisterWnd = std::make_pair( hWnd, msg );
}

bool			CInputBufferImpl::PutIncomingMessage( const std::wstring& sText)
{
	m_Sync.Lock(); 
	if( m_Buffer.size() > 100 )
	{
		m_Sync.Unlock();
		return false;
	}
	m_Buffer.push( sText );
	m_Sync.Unlock();
	if( ::IsWindow(m_RegisterWnd.first) )
	{
		::PostMessage( m_RegisterWnd.first, m_RegisterWnd.second, 0, 0 );
	}
	return true;
}

IInputBuffer&	GetInputBuffer()
{
	static CInputBufferImpl bf;
	return bf;
}