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
#ifndef __GRABBER_FACTORY_IMPL_H_INCLUDED_0076786825683271__
#define __GRABBER_FACTORY_IMPL_H_INCLUDED_0076786825683271__

#include "IGrabberFactory.h"
#include "FileGrabberFactory.h"
#include "../../CSArchive/CHCSVA/chcsva.h"
#include <boost/shared_ptr.hpp>
#include "../COMMON/Exception.h"

MACRO_EXCEPTION( CameraGrabberException, CommonException );

//! ����� �������� chcsva.dll �� ����� ����������
struct	VideoLib
{
	//! ������ �������
	bool						m_Valid;
	//! ���������� ������
	HMODULE						m_hLibrary;

	//! ��������� �� InitStreamManager, ������������ ��������� ������������
	CHCS::funcInitStreamManager		m_InitStreamManager;
	//! ��������� �� FreeStreamManager, ���������� �������� ������������
	CHCS::funcFreeStreamManager		m_FreeStreamManager;
	//! ��������� �� GetStreamByID, �������� ������ �� �������������� ������ � ��
	CHCS::funcGetStreamByID			m_GetStreamByID;
	//! ��������� �� GetAvailableStreams, �� ������������
	CHCS::funcGetAvailableStreams	m_GetAvailableStreams;
	//! ��������� �� StartArchiveStream, �������� ��������� ������ �� �������������� � ��
	CHCS::funcStartArchiveStream	m_StartArchiveStream;

	VideoLib() : 
	m_Valid(false), 
		m_hLibrary( LoadLibrary(_T("chcsva.dll")) ),
		m_InitStreamManager( NULL ), 
		m_FreeStreamManager( NULL ),
		m_GetStreamByID( NULL),
		m_GetAvailableStreams( NULL)
	{
		if( m_hLibrary )
		{
			m_InitStreamManager		= (CHCS::funcInitStreamManager)GetProcAddress(		m_hLibrary, "InitStreamManager" );
			m_FreeStreamManager		= (CHCS::funcFreeStreamManager)GetProcAddress(		m_hLibrary, "FreeStreamManager" );
			m_GetStreamByID			= (CHCS::funcGetStreamByID)GetProcAddress(			m_hLibrary, "GetStreamByID" );
			m_GetAvailableStreams	= (CHCS::funcGetAvailableStreams)GetProcAddress(	m_hLibrary, "GetAvailableStreams" );
			m_StartArchiveStream	= (CHCS::funcStartArchiveStream)GetProcAddress(		m_hLibrary,	"StartArchiveStream");
			m_Valid	 =	!IsBadCodePtr( (FARPROC)m_InitStreamManager) && 
				!IsBadCodePtr((FARPROC)m_FreeStreamManager) && 
				!IsBadCodePtr((FARPROC)m_GetStreamByID) &&
				!IsBadCodePtr((FARPROC)m_GetAvailableStreams) &&
				!IsBadCodePtr((FARPROC)m_StartArchiveStream) ;
		}
	}

	~VideoLib()
	{
		if( m_hLibrary ) FreeLibrary( m_hLibrary );
	}

//! ��������� ���������� ��������
//! \return true - �������� �������, false - ������
	bool IsValid() const { return m_Valid; }
};

/////////////////////////////////////////////////////////////////

//! \brief ���������� ��������� �������� ��� ������
//! \version 1.0
//! \date 10-18-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//!
class CameraGrabberImpl : public IArchiveGrabber
{
	//! ��������� ��������� ������
	boost::shared_ptr<CHCS::IStream>	m_pStream;
	//! ������� ����, ����������� ����� �������
	boost::shared_ptr<CHCS::IFrame>		m_pFrame;
	//! ��� ��� ���������
	std::vector<BYTE>					m_BmpHeader;
	
//! ������� ����������
//! \param pStream ��������� �� �����
	CameraGrabberImpl( CHCS::IStream* pStream ) :
		m_pStream( pStream, DeleteVideoStream )
	{
	}

	virtual ~CameraGrabberImpl()
	{
	}

	virtual HRESULT LockFrame( const BITMAPINFO * & ppHeader, const BYTE * & ppImage, int &  nImgSize ) ;
	virtual HRESULT UnlockFrame()	;
	virtual HRESULT Start()			;
	virtual HRESULT Stop()			;
	virtual HRESULT SeekTo(INT64 nTime);
	virtual HRESULT GetTime(INT64& nTime);
	virtual void	Release()		
	{ 
		delete this;
	}
	//! ������� ���������, ��� shared_ptr, use ReleaseInterface<>() instead
	//! \param pStream ��������� �� ��������� ���������
	static void	DeleteVideoStream( CHCS::IStream* pStream ) 
	{ 
		pStream->Release();
	}
	//! ������� ���������, ��� shared_ptr, use ReleaseInterface<>() instead
	//! \param pFrame ��������� �� ��������� ��������� 
	static void	DeleteVideoFrame( CHCS::IFrame* pFrame ) 
	{ 
		pFrame->Release();
	}
public:
//! ������� ����������
//! \param pStream �����, ������� �����������
//! \return ��������� �� ���������
	static IArchiveGrabber* Create( CHCS::IStream* pStream )
	{
		return new CameraGrabberImpl(pStream);
	}

};
//=====================================================================================//
//                              class GrabberFactoryImpl                               //
//=====================================================================================//

//! \brief ����� ������� �������� ������� ��� �������
//! \version 1.0
//! \date 10-18-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//!
class GenericGrabberFactoryImpl : public FileGrabberFactory
{
protected:
	//! ��������� ��������� �-�
	HRESULT		m_hrLastCode;
	//! ��������� �������?
	bool		m_bServer;
	
//! ��������
//! \param bServer true-������, false - ������
	GenericGrabberFactoryImpl(bool bServer);
	virtual ~GenericGrabberFactoryImpl();

	//! ������� ��� �����������
	std::auto_ptr<VideoLib>			m_pVideoLib;
	
//! �����������
//! \param pDebugInt ��������� �� ���������� ���������
//! \param bLocalVideo ����� �� ������ ����������� ��������� ����������� ��� ������, �� ������������, ��� �������������
//! \return S_OK - success, else error
	HRESULT		Initialize( IDebugOutput* pDebugInt, bool bLocalVideo );

	//! ���������� ���������� ���������, ���� �� ���������
	//! \param pDebugInt ��������� �� ���������� ���������
	void		SetDebugOutput( IDebugOutput* pDebugInt ) 
	{ 
		m_Debug.Set( pDebugInt );  
	}

//! ������� ��������� ������ ������
//! \param ppFrameGrabber ��������� �� ���������� ��������� ����������
//! \param nRoomID ������������� ������
//! \return S_OK - success, else error
	HRESULT		CreateCameraGrabber( ICameraGrabber** ppFrameGrabber, int nRoomID );


//! 
//! \param ppFrameGrabber ppFrameGrabber ��������� �� ���������� ��������� ����������
//! \param nCameraID ������������� ������ 
//! \param nStartTime ��������� ����� ������, ���-�� ������ _ftime()
//! \return S_OK - success, else error
	HRESULT		CreateArchiveGrabber( IArchiveGrabber** ppFrameGrabber, int nCameraID, INT64 nStartTime );

	//! �-� ������� ��� chcsva.dll
	//! \param dwUserData ���������������� ������ (��������� �� IDebugOutput)
	//! \param nSeverity ��� ������
	//! \param szRecord �������� ������
	static void	 _cdecl	DebugOutputProc( DWORD_PTR dwUserData, int nSeverity, LPCWSTR szRecord );
};

class ServerGrabberFactoryImpl :	public		IGrabberFactory, 
									protected	GenericGrabberFactoryImpl
{
public:
	ServerGrabberFactoryImpl() : GenericGrabberFactoryImpl(true)
	{
	}
private:
	//! suicide
	virtual void		Release() 
	{ 
		delete this;
	};

	virtual void		SetDebugOutput( IDebugOutput* pDebugInt )
	{
		GenericGrabberFactoryImpl::SetDebugOutput( pDebugInt );
	}

	virtual HRESULT		CreateCameraGrabber( ICameraGrabber** ppFrameGrabber, int nRoomID )
	{
		return GenericGrabberFactoryImpl::CreateCameraGrabber( ppFrameGrabber, nRoomID );
	}

	HRESULT		CreateFileGrabber(	IFileGrabber** ppFrameGrabber, 
									LPCWSTR  szFileName,
									int nBitsPerPixel, 
									bool bPerFrame, 
									bool bLoopback ) 
	{
		return GenericGrabberFactoryImpl::CreateFileGrabber(ppFrameGrabber, szFileName, nBitsPerPixel,  bPerFrame, bLoopback );
	}

	HRESULT		CreateArchiveGrabber( IArchiveGrabber** ppFrameGrabber, int nCameraID, INT64 nStartTime ) 
	{
		// can't create archive on server
		*ppFrameGrabber = NULL;
		return E_ACCESSDENIED;
	}

	virtual HRESULT		Initialize( IDebugOutput* pDebugInt, bool bLocalVideo )
	{
		return GenericGrabberFactoryImpl::Initialize( pDebugInt, bLocalVideo );
	}
};

class ClientGrabberFactoryImpl :	public		IGrabberFactory, 
									protected	GenericGrabberFactoryImpl
{
public:
	ClientGrabberFactoryImpl() : GenericGrabberFactoryImpl(false){}
private:
	// global, nothing
	virtual void		Release() 
	{ 
		delete this;
	};
	virtual HRESULT		Initialize( IDebugOutput* pDebugInt, bool bLocalVideo )
	{
		return GenericGrabberFactoryImpl::Initialize( pDebugInt, bLocalVideo );
	}

	virtual void		SetDebugOutput( IDebugOutput* pDebugInt )
	{
		GenericGrabberFactoryImpl::SetDebugOutput( pDebugInt );
	}

	virtual HRESULT		CreateCameraGrabber( ICameraGrabber** ppFrameGrabber, int nCameraID )
	{
		return GenericGrabberFactoryImpl::CreateCameraGrabber( ppFrameGrabber, nCameraID );
	}
	HRESULT		CreateFileGrabber(	IFileGrabber** ppFrameGrabber, 
									LPCWSTR  szFileName,
									int nBitsPerPixel, 
									bool bPerFrame, 
									bool bLoopback ) 
	{
		return GenericGrabberFactoryImpl::CreateFileGrabber(ppFrameGrabber, szFileName, nBitsPerPixel, bPerFrame, bLoopback );
	}

	HRESULT		CreateArchiveGrabber( IArchiveGrabber** ppFrameGrabber, int nCameraID, INT64 nStartTime ) 
	{
		return GenericGrabberFactoryImpl::CreateArchiveGrabber( ppFrameGrabber, nCameraID, nStartTime );
	}
};

#endif //__GRABBER_FACTORY_IMPL_H_INCLUDED_0076786825683271__
