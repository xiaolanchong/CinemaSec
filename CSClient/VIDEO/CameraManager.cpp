//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Description is not available
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 17.02.2005
//                                                                                      //
//======================================================================================//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CameraManager.h"
#include "../../CSChair/interface.h"
#include "../../CSChair/public/NameParse.h"
#include "../DBFacet/CfgDB.h"

#include "../Log_mode/ClientLogManager.h"
#include <shlwapi.h>
//======================================================================================//
//                                 struct CameraManager                                 //
//======================================================================================//

static void DeleteGrabberFactory( IGrabberFactory* p)
{
	p->Release();
}

CameraManager::CameraManager()
{
	IGrabberFactory* pFactory;
	BOOL bUseStub = SHRegGetBoolUSValue( _T("Software\\ElVEES\\CinemaSec"), _T("VideoStub"), TRUE, FALSE );
	DWORD dwRes = CreateInterface( 
		bUseStub? STUB_GRABBER_FACTORY : CLIENT_GRABBER_FACTORY,
		(void**)&pFactory );
	if( dwRes || !pFactory)
	{
		return;
	}
	boost::shared_ptr<IGrabberFactory> Tmp( pFactory, DeleteGrabberFactory );
	IDebugOutput* pDbg = GetLogManager().CreateDebugOutput( L"Video" );
	HRESULT hr = pFactory->Initialize( pDbg, false );
	if( hr != S_OK )
	{
		return;
	}
	m_Factory = Tmp;
}

CameraManager::~CameraManager()
{
}

ICameraGrabber* CameraManager::CreateGrabber( int nCameraID )
{
	if( !m_Factory ) return 0;
	HRESULT hr;
	ICameraGrabber* p;
	hr = m_Factory->CreateCameraGrabber( &p, nCameraID );
	if( hr == S_OK )
		return p;
	else 
		return NULL;
}

IArchiveGrabber* CameraManager::CreateArchiveGrabber( int nCameraID, INT64 nStartTime )
{
	if( !m_Factory ) return 0;
	HRESULT hr;
	IArchiveGrabber* p;
	hr = m_Factory->CreateArchiveGrabber( &p, nCameraID, nStartTime );
	if( hr == S_OK )
		return p;
	else 
		return NULL;
}

IFileGrabber*	CameraManager::CreateFileGrabber( LPCWSTR szFileName )
{
	if( !m_Factory ) return 0;
	HRESULT hr;
	IFileGrabber* p;
	hr = m_Factory->CreateFileGrabber( &p, szFileName, 32, false, true );
	if( hr == S_OK )
		return p;
	else 
		return NULL;
}

void			CameraManager::StopRoomCamera( int nRoomID )
{
	UNREFERENCED_PARAMETER(nRoomID);
}

// FIXME when not use simulate, switch off file management

boost::shared_ptr<IBaseGrabber>	CameraManager::CreateGrabber( int nCameraID, LPCWSTR szSource, int nBits, bool bRealtime )
{
	if( !m_Factory ) return boost::shared_ptr< IBaseGrabber >();
	boost::shared_ptr< IBaseGrabber >		FrameGrabber;
	NameParse np( szSource);
//	IBaseGrabber* pGrabber = NULL;
//	HRESULT hr;
	std::wstring sSource = np.GetSource();
	switch( np.GetType() )
	{
	case vidt_file :
		{
			HRESULT hr;
			IFileGrabber* p;
			hr = m_Factory->CreateFileGrabber( &p, sSource.c_str() , nBits, !bRealtime, bRealtime );
			if( hr != S_OK ) break;
			return boost::shared_ptr< IBaseGrabber >( p, ReleaseInterface<IBaseGrabber>() ); 
		}
		break;
	case vidt_camera :
		{
			ICameraGrabber* pCameraGrab = CreateGrabber( nCameraID ) ;
			return boost::shared_ptr< IBaseGrabber >( pCameraGrab, ReleaseInterface<IBaseGrabber>() ); 
		}
		break;
	}
	return boost::shared_ptr< IBaseGrabber >();
}

boost::shared_ptr<IFileGrabber>	CameraManager::CreateFileGrabber( LPCWSTR szFileName, int nBits, bool bRealtime )
{
	if( !m_Factory ) return boost::shared_ptr<IFileGrabber>();
	HRESULT hr;
	IFileGrabber* p;
	hr = m_Factory->CreateFileGrabber( &p, szFileName, nBits, !bRealtime, bRealtime  );
	if( hr == S_OK )
		return boost::shared_ptr< IFileGrabber >( p, ReleaseInterface<IFileGrabber>() );
	else 
		return boost::shared_ptr<IFileGrabber>();
}

CameraManager& GetCameraManager()
{
	static CameraManager cm;
	return cm;
}

/////////////////////////////////////////// 

