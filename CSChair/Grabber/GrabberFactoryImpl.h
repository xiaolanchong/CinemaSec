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

//! класс загрузки chcsva.dll во время выполнения
struct	VideoLib
{
	//! вызовы валидны
	bool						m_Valid;
	//! дескриптор модуля
	HMODULE						m_hLibrary;

	//! указатель на InitStreamManager, инииализация менеджера видеопотоков
	CHCS::funcInitStreamManager		m_InitStreamManager;
	//! указатель на FreeStreamManager, освободить менеджер видеопотоков
	CHCS::funcFreeStreamManager		m_FreeStreamManager;
	//! указатель на GetStreamByID, создание потока по идентификатору камеры в БД
	CHCS::funcGetStreamByID			m_GetStreamByID;
	//! указатель на GetAvailableStreams, не используется
	CHCS::funcGetAvailableStreams	m_GetAvailableStreams;
	//! указатель на StartArchiveStream, создание архивного потока по идентификатору в БД
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

//! проверить валидность загрузки
//! \return true - загрузка успешна, false - ошибка
	bool IsValid() const { return m_Valid; }
};

/////////////////////////////////////////////////////////////////

//! \brief реализация архивного граббера для камеры
//! \version 1.0
//! \date 10-18-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//!
class CameraGrabberImpl : public IArchiveGrabber
{
	//! интерфейс архивного потока
	boost::shared_ptr<CHCS::IStream>	m_pStream;
	//! текущий кадр, уничтожится перед потоком
	boost::shared_ptr<CHCS::IFrame>		m_pFrame;
	//! кэш для заголовка
	std::vector<BYTE>					m_BmpHeader;
	
//! создать реализацию
//! \param pStream указатель на поток
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
	//! удалить интерфейс, для shared_ptr, use ReleaseInterface<>() instead
	//! \param pStream указатель на удаляемый интерфейс
	static void	DeleteVideoStream( CHCS::IStream* pStream ) 
	{ 
		pStream->Release();
	}
	//! удалить интерфейс, для shared_ptr, use ReleaseInterface<>() instead
	//! \param pFrame указатель на удаляемый интерфейс 
	static void	DeleteVideoFrame( CHCS::IFrame* pFrame ) 
	{ 
		pFrame->Release();
	}
public:
//! созадть реализацию
//! \param pStream поток, который оборачиваем
//! \return указатель на интерфейс
	static IArchiveGrabber* Create( CHCS::IStream* pStream )
	{
		return new CameraGrabberImpl(pStream);
	}

};
//=====================================================================================//
//                              class GrabberFactoryImpl                               //
//=====================================================================================//

//! \brief общая фабрика создания обертки над потоком
//! \version 1.0
//! \date 10-18-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//!
class GenericGrabberFactoryImpl : public FileGrabberFactory
{
protected:
	//! результат последней ф-и
	HRESULT		m_hrLastCode;
	//! серверная фабрика?
	bool		m_bServer;
	
//! создание
//! \param bServer true-сервер, false - клиент
	GenericGrabberFactoryImpl(bool bServer);
	virtual ~GenericGrabberFactoryImpl();

	//! обертка над библиотекой
	std::auto_ptr<VideoLib>			m_pVideoLib;
	
//! инициалиция
//! \param pDebugInt указатель на отладочный интерфейс
//! \param bLocalVideo может ли клиент захватывать локальные видеопотоки без сервер, не используется, для совместимости
//! \return S_OK - success, else error
	HRESULT		Initialize( IDebugOutput* pDebugInt, bool bLocalVideo );

	//! установить отладочный интерфейс, если он изменился
	//! \param pDebugInt указатель на отладочный интерфейс
	void		SetDebugOutput( IDebugOutput* pDebugInt ) 
	{ 
		m_Debug.Set( pDebugInt );  
	}

//! создать интерфейс потока камеры
//! \param ppFrameGrabber указатель на переменную выходного интерфейса
//! \param nRoomID идентификатор камеры
//! \return S_OK - success, else error
	HRESULT		CreateCameraGrabber( ICameraGrabber** ppFrameGrabber, int nRoomID );


//! 
//! \param ppFrameGrabber ppFrameGrabber указатель на переменную выходного интерфейса
//! \param nCameraID идентификатор камеры 
//! \param nStartTime начальное время архива, кол-во секунд _ftime()
//! \return S_OK - success, else error
	HRESULT		CreateArchiveGrabber( IArchiveGrabber** ppFrameGrabber, int nCameraID, INT64 nStartTime );

	//! ф-я отладки для chcsva.dll
	//! \param dwUserData пользовательские данные (указатель на IDebugOutput)
	//! \param nSeverity вид ошибки
	//! \param szRecord описание ошибки
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
