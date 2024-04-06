//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   21.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#include "stdafx.h"
#include "GrabberFactoryImpl.h"
#include "../ChairError.h"
#include "../interfaceEx.h"


#include "..\public\memleak.h"

//=====================================================================================//
//                              class GrabberFactoryImpl                               //
//=====================================================================================//
GenericGrabberFactoryImpl::GenericGrabberFactoryImpl( bool bServer ) : m_bServer(bServer)
{

}

GenericGrabberFactoryImpl::~GenericGrabberFactoryImpl()
{
	if( m_pVideoLib.get() && m_pVideoLib->IsValid() )
	{
		m_pVideoLib->m_FreeStreamManager();
	}
}

HRESULT		GenericGrabberFactoryImpl::Initialize( IDebugOutput* pDebugInt, bool bLocalVideo )
{
	m_hrLastCode = E_FAIL;
	m_Debug.Set( pDebugInt );
	m_pVideoLib = std::auto_ptr<VideoLib>( new VideoLib );

	if( !m_pVideoLib.get() || !m_pVideoLib->IsValid() ) 
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"Can't locate video library chcsva.dll or library is invalid" );
		//return ERR_VIDEOLIB_NOT_FOUND;
	}
	else
	{
		DWORD dwClientFlag = CHCS_MODE_CLIENT 
#if 0
			| ( bLocalVideo ?  CHCS_MODE_DEVICE : 0)
#endif
			;
		CHCS::INITCHCS initCHCS;
		initCHCS.dwSize = sizeof(initCHCS);
		initCHCS.dwMode = m_bServer ?  CHCS_MODE_SERVER : dwClientFlag; 
		initCHCS.dwReserved = 0;
		initCHCS.dwUserData = (DWORD_PTR)this;
		initCHCS.procOutput = pDebugInt ? DebugOutputProc : NULL;	// Use OutputDebugString

		if(! m_pVideoLib->m_InitStreamManager(&initCHCS))
		{
			m_Debug.PrintW( IDebugOutput::mt_error, L"Can't initialize video library" );
			return ERR_VIDEOLIB_INIT_FAILED;
		}
	}
	m_hrLastCode = S_OK;
	return S_OK;
}

void	GenericGrabberFactoryImpl::DebugOutputProc( DWORD_PTR dwUserData, int nSeverity, LPCWSTR szRecord )
{
	if( !dwUserData ) return;
	GenericGrabberFactoryImpl * pThis = reinterpret_cast< GenericGrabberFactoryImpl * >(dwUserData);
	pThis->m_Debug.PrintW( nSeverity, szRecord );
	//OutputDebug
}

HRESULT	GenericGrabberFactoryImpl::CreateCameraGrabber( ICameraGrabber** ppFrameGrabber, int nCameraID )
{
	if( m_hrLastCode != S_OK ) return m_hrLastCode; 
	if( !m_pVideoLib.get() || !m_pVideoLib->IsValid() )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"chcsva.dll not found or not initialized" );
		return ERR_VIDEOLIB_INIT_FAILED;
	}
	*ppFrameGrabber = 0;
	try
	{
		CHCS::IStream* pStream;
		bool res = m_pVideoLib->m_GetStreamByID( &pStream, nCameraID );
		if( !res || !pStream )
		{
			throw CameraGrabberException("Err");
		}

		*ppFrameGrabber = CameraGrabberImpl::Create(pStream) ;
	}
	catch( CameraGrabberException )
	{
		*ppFrameGrabber = 0;
		return E_FAIL;
	}
	return S_OK;
}

HRESULT		GenericGrabberFactoryImpl::CreateArchiveGrabber( IArchiveGrabber** ppFrameGrabber, int nCameraID, INT64 nStartTime )
{
	if( m_hrLastCode != S_OK ) return m_hrLastCode; 
	if( !m_pVideoLib.get() || !m_pVideoLib->IsValid() )
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"chcsva.dll not found or not initialized" );
		return ERR_VIDEOLIB_INIT_FAILED;
	}
	*ppFrameGrabber = 0;
	try
	{
		CHCS::IStream* pStream;
		bool res = m_pVideoLib->m_StartArchiveStream( &pStream, nCameraID, nStartTime );
		if( !res || !pStream )
		{
			throw CameraGrabberException("Err");
		}

		*ppFrameGrabber = CameraGrabberImpl::Create(pStream) ;
	}
	catch( CameraGrabberException )
	{
		*ppFrameGrabber = 0;
		return E_FAIL;
	}
	return S_OK;
}

//HRESULT		GenericGrabberFactoryImpl::CreateFileGrabber(	IFileGrabber** ppFrameGrabber, 
//															LPCWSTR szFileName,
//															int nBitsPerPixel, 
//															bool bPerFrame, 
//															bool bLoopback ) 
//{
//	*ppFrameGrabber = NULL;
//	if( !ppFrameGrabber) return E_POINTER;
//
//	LPCWSTR szExt = PathFindExtensionW( szFileName );
//	if( lstrcmpiW(szExt, L".avd" ) == 0 )
//	{
//		return CreateAVDFileGrabber( ppFrameGrabber, szFileName, nBitsPerPixel, bLoopback );
//	}
//	else
//		return CreateNormalFileGrabber( ppFrameGrabber, szFileName, nBitsPerPixel, bPerFrame, bLoopback );
//}
//
//HRESULT		GenericGrabberFactoryImpl::CreateNormalFileGrabber(	IFileGrabber** ppFrameGrabber, 
//																LPCWSTR szFileName,
//																int nBitsPerPixel, 
//																bool bPerFrame, 
//																bool bLoopback ) 
//{
//	if( nBitsPerPixel != 32 && 
//		nBitsPerPixel != 24 && 
//		nBitsPerPixel != 16 && 
//		nBitsPerPixel != 8	&&
//		nBitsPerPixel != MY_BI_YUY2 &&
//		nBitsPerPixel != MY_BI_GRAY &&
//		nBitsPerPixel != MY_BI_Y800 ) 
//		return E_INVALIDARG;
//	IFileGrabber* pFileGrabber;
//	try
//	{
//		BaseFrameGrabber *pGrab = NULL;
//		if( bPerFrame) 
//		{
//			BaseFrameGrabberVfW *pGrabVfW	= new BaseFrameGrabberVfW( nBitsPerPixel, bPerFrame, bLoopback );
//			pFileGrabber					= pGrabVfW;
//			pGrab							= pGrabVfW;
//		}
//		else
//		{
//			BaseFrameGrabberDS *pGrabDS = new BaseFrameGrabberDS( nBitsPerPixel, bPerFrame, bLoopback );
//			pFileGrabber				= pGrabDS ;
//			pGrab						= pGrabDS;
//		}
//		FrameGrabberParameters parm;
//		bool res = pGrab->Start( szFileName, parm ); 
//		if( !res )
//		{
//			pFileGrabber->Release();
//			return E_FAIL;
//		}
//	}
//	catch(DirectShowException2 )
//	{
//		return E_FAIL;
//	}
//	catch( CommonException )
//	{
//		return E_FAIL;
//	}
//	*ppFrameGrabber = pFileGrabber;
//	return S_OK;
//}
//
//HRESULT	GenericGrabberFactoryImpl::CreateAVDFileGrabber(IFileGrabber** ppFrameGrabber, 
//														LPCWSTR  szFileName,
//														int nBitsPerPixel, 
//														bool bLoopback ) 
//try
//{
//	*ppFrameGrabber = new AVDGrabber( szFileName, nBitsPerPixel, bLoopback, m_Debug.Get() );
//	return S_OK;
//}
//catch ( AVDGrabberException ) 
//{
//	*ppFrameGrabber = NULL;
//	return E_FAIL;
//};

/////////////////////////// ICameraGrabber implementation ////////////////////////////

HRESULT CameraGrabberImpl::LockFrame( const BITMAPINFO * & ppHeader, const BYTE * & ppImage, int &  nImgSize )
{
	ppHeader	= NULL;
	ppImage	= NULL;
	nImgSize	= 0;
	if( !m_pStream ) 
	{
		return E_FAIL;
	}
	m_pFrame.reset();
	// bug in chsva.dll - GetNextFrame returns true when pFrame=NULL; 23.06.2005
	CHCS::IFrame *pFrame = NULL;
	bool res = m_pStream->GetNextFrame( &pFrame, 200 ); // ms
	if( !res || !pFrame) return E_FAIL;
	boost::shared_ptr<CHCS::IFrame> Tmp( pFrame, DeleteVideoFrame );
	m_pFrame = Tmp;
	ppImage = (const BYTE*)m_pFrame->GetBytes();
	long nSize = m_pFrame->GetFormat(0);
	if( !nSize ) return E_FAIL;
	// we expect BITMAPINFO but may have only BITMAPINFOHEADER
	nSize = nSize < sizeof(BITMAPINFO) ? sizeof(BITMAPINFO) : nSize ;
	m_BmpHeader.resize( nSize );
	BITMAPINFO* pbi = (BITMAPINFO*)&m_BmpHeader[0];
	m_pFrame->GetFormat( pbi );
	ppHeader	= pbi;
	nImgSize	= pbi->bmiHeader.biSizeImage;
	return S_OK;
}

HRESULT CameraGrabberImpl::UnlockFrame()
{
	if( !m_pStream || !m_pFrame ) return E_FAIL; 
	m_pFrame.reset();
	return S_OK;
}

HRESULT CameraGrabberImpl::Start()		
{
	if( !m_pStream ) return E_FAIL; 
	bool res = m_pStream->Start();
	return res? S_OK : E_FAIL ;
}

HRESULT CameraGrabberImpl::Stop()
{
	if( !m_pStream ) return E_FAIL;
	m_pStream->Stop();
	return S_OK;
}

HRESULT CameraGrabberImpl::SeekTo(INT64 nTime)
{
	if( !m_pStream ) return E_FAIL;
	long res = m_pStream->SeekTo( nTime );
	UNREFERENCED_PARAMETER(res);
	return S_OK;
}

HRESULT CameraGrabberImpl::GetTime(INT64& nTime)
{
	if( !m_pStream || !m_pFrame ) return E_FAIL;
	nTime = m_pFrame->GetTime( );
	return S_OK;
}

EXPOSE_INTERFACE( ClientGrabberFactoryImpl, IGrabberFactory, CLIENT_GRABBER_FACTORY );
EXPOSE_INTERFACE( ServerGrabberFactoryImpl, IGrabberFactory, SERVER_GRABBER_FACTORY );