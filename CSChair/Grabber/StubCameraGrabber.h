//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Отладочная заглушка для ICameraGrabber, создает тестовое изображение
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 05.07.2005
//                                                                                      //
//======================================================================================//
#ifndef _STUB_CAMERA_GRABBER_4996502097390510_
#define _STUB_CAMERA_GRABBER_4996502097390510_

#include "IGrabberFactory.h"
#include "FileGrabberFactory.h"
////////// stub stream
//#define TEST_EXCEPTION
#undef TEST_EXCEPTION

//! \brief реализация IArchiveGrabber, заглушка над архивом, поставляет кадры со временем
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class BaseStubCameraGrabber :  public IArchiveGrabber
{
protected:
	//! идентификатор камеры
	int					m_nID;
	//! буфер для заголовка
	BITMAPINFO			m_cacheHdr;	
	//! буфер для изображения
	std::vector<BYTE>	m_cacheImage;	
	//! контекс для прорисовки кадра
	CDC					m_cacheDC;
	//! битмап для прорисовки
	CBitmap				m_cacheBmp;

	//! создать заголовок
	void				GenerateImage( );
	//! вернуть надпись на кадре, позовляет переопределять для производных классов
	//! \return надпись
	virtual	CString		GenerateHeader(  ) = 0;
public:
	//! создать с параметрами
	//! \param nID идентификатор камеры
	//! \param dwWidth ширина изображения
	//! \param dwHeight высота изображения
	BaseStubCameraGrabber( int nID, DWORD dwWidth, DWORD dwHeight );
	virtual ~BaseStubCameraGrabber(){}

	virtual void	Release() { delete this;};

	virtual HRESULT LockFrame( const BITMAPINFO * &ppHeader, const BYTE * &ppImage, int   &nImgSize ) ;
	virtual HRESULT UnlockFrame()	;				
};

//! \brief заглушка для поставщика изображения с камеры для клиента
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class RealtimeStubCameraGrabber : public BaseStubCameraGrabber
{
public:
	//! создать с параметрами
	//! \param nID идентификатор камеры
	//! \param dwWidth ширина изображения
	//! \param dwHeight высота изображения
	RealtimeStubCameraGrabber( int nID, DWORD dwWidth, DWORD dwHeight ) : 
	  BaseStubCameraGrabber( nID, dwWidth, dwHeight )
	  {}

	  //! вернуть надпись на кадре, позовляет переопределять для производных классов
	  //! \return надпись
	CString		GenerateHeader(  ) ;
	virtual HRESULT SeekTo(INT64 nTime)			{ return E_NOTIMPL;}
	virtual HRESULT GetTime( INT64& nTime )		{ return E_NOTIMPL; }
};

//! \brief заглушка для поставщика изображения архива для клиента 
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class ArchiveStubCameraGrabber : public BaseStubCameraGrabber
{
	//! текущее время
	INT64	m_nTime;
	//! вермя с последнего вызова Seek
	INT64	m_nLastSeekTime;
	//! вывести время в Output
	//! \param szDesc комментарии
	//! \param t время для вывода
	void	DumpTime( LPCTSTR szDesc, CTime t)
	{
#ifdef _DEBUG
		CString s = t.Format(_T("%H:%M %d.%m.%Y"));
		TCHAR buf[255];
		StringCchPrintf( buf, 255, _T("%s - %s\n"), szDesc, (LPCTSTR)s );
		OutputDebugString( buf );
#endif
	}
public:
	//! создать с параметрами
	//! \param nID идентификатор камеры
	//! \param dwWidth ширина изображения
	//! \param dwHeight высота изображения
	//! \param nTime время начала работы архива 
	ArchiveStubCameraGrabber( int nID, DWORD dwWidth, DWORD dwHeight, INT64 nTime ) : 
		BaseStubCameraGrabber( nID, dwWidth, dwHeight  ), 
		m_nTime( nTime),
		m_nLastSeekTime(_time64(0))
		{}

	  CString		GenerateHeader(  ) ;
	  virtual HRESULT SeekTo(INT64 nTime) 
	  { 
		  m_nTime			= nTime; 
		  m_nLastSeekTime	= _time64(0);
		  DumpTime( _T("ArchiveStubCameraGrabber::SeekTo"), m_nTime);
		  return S_OK;
	  }
	  virtual HRESULT GetTime( INT64& nTime )		
	  { 
		  INT64 nDelta = (_time64(0) - m_nLastSeekTime) ;
		  nTime = m_nTime + nDelta;
		  DumpTime(_T("ArchiveStubCameraGrabber::GetTime"), m_nTime);
		  return S_OK; 
	  }
};

//! \brief  заглушка для поставщика изображения архива для сервера, передает монотонные кадры 
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class BaseServerStubCameraGrabber :  public ICameraGrabber
{
#ifdef TEST_EXCEPTION
	//! кадров передано
	DWORD				m_dwFrameNum;
#endif
protected:
	//! идентификатор камеры
	int					m_nID;
	//! буфер для заголовка
	BITMAPINFO			m_cacheHdr;	
	//! буфер для изобрадения
	std::vector<BYTE>	m_cacheImage;	

public:
	//! создать с параметрами
	//! \param nID идентификатор камеры
	//! \param dwWidth ширина изображения
	//! \param dwHeight высота изображения
	BaseServerStubCameraGrabber( int nID, DWORD dwWidth, DWORD dwHeight );
	virtual ~BaseServerStubCameraGrabber(){}

	virtual void	Release() { delete this;};

	virtual HRESULT LockFrame( const BITMAPINFO * &ppHeader, const BYTE * &ppImage, int   &nImgSize ) ;
	virtual HRESULT UnlockFrame()		{ return S_OK;		}
	virtual HRESULT SeekTo(INT64 nTime) { return E_NOTIMPL;	}
};

//======================================================================================//
//                               class StubGrabberFactory                                //
//======================================================================================//

//! \brief класс создания грабберов для клинета
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class StubGrabberFactory :	public		IGrabberFactory,
							protected	FileGrabberFactory
{
public:
	StubGrabberFactory();
	virtual ~StubGrabberFactory();

	virtual void		Release() 
	{
		delete this;
	}
	virtual HRESULT		Initialize( IDebugOutput* pDebugInt, bool bLocalVideo )
	{
		m_Debug.Set( pDebugInt );
		m_Debug.PrintW( IDebugOutput::mt_info, L"Stub grabber factory initialized. Use it only in tests" );
		return S_OK;
	}
	virtual void		SetDebugOutput( IDebugOutput* pDebugInt ) 
	{
		m_Debug.Set( pDebugInt );
	}
	virtual HRESULT		CreateCameraGrabber( ICameraGrabber** ppFrameGrabber, int nCameraID ) ;
	virtual HRESULT		CreateArchiveGrabber( IArchiveGrabber** ppFrameGrabber, int nCameraID, INT64 nStartTime ) ;

	virtual HRESULT		CreateFileGrabber(	IFileGrabber** ppFrameGrabber, 
											LPCWSTR  szFileName,
											int nBitsPerPixel, 
											bool bPerFrame, 
											bool bLoopback )
	{
		return FileGrabberFactory::CreateFileGrabber( ppFrameGrabber, szFileName, nBitsPerPixel, bPerFrame, bLoopback );
	}
};

//! \brief класс создания граббера для сервера
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class StubServerGrabberFactory : public IGrabberFactory
{
	MyDebugOutputImpl	m_Debug;
public:
	StubServerGrabberFactory() {};
	virtual ~StubServerGrabberFactory() {};

	virtual void		Release() 
	{
		delete this;
	}
	virtual HRESULT		Initialize( IDebugOutput* pDebugInt, bool bLocalVideo )
	{
		m_Debug.Set( pDebugInt );
		m_Debug.PrintW( IDebugOutput::mt_info, L"Stub server grabber factory initialized. Use it only in tests" );
		return S_OK;
	}
	virtual void		SetDebugOutput( IDebugOutput* pDebugInt ) 
	{
		m_Debug.Set( pDebugInt );
	}
	virtual HRESULT		CreateCameraGrabber( ICameraGrabber** ppFrameGrabber, int nCameraID ) 
	{
		m_Debug.PrintW( IDebugOutput::mt_info, L"Create ServerStubCameraGrabber for id=%d", nCameraID );
		*ppFrameGrabber = new BaseServerStubCameraGrabber (nCameraID, 720, 576) ;
		return S_OK;
	}
	virtual HRESULT		CreateArchiveGrabber( IArchiveGrabber** ppFrameGrabber, int nCameraID, INT64 nStartTime ) 
	{
		if( !ppFrameGrabber ) return E_POINTER;
		*ppFrameGrabber = NULL;
		return E_NOTIMPL;
	}

	virtual HRESULT		CreateFileGrabber(	IFileGrabber** ppFrameGrabber, 
		LPCWSTR  szFileName,
		int nBitsPerPixel, 
		bool bPerFrame, 
		bool bLoopback )
	{
		if( !ppFrameGrabber ) return E_POINTER;
		*ppFrameGrabber = NULL;
		return E_NOTIMPL;
	}
};

#endif // _STUB_CAMERA_GRABBER_4996502097390510_