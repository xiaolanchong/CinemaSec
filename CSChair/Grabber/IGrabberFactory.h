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
#ifndef __I_GRABBER_FACTORY_H_INCLUDED_2221272137662688__
#define __I_GRABBER_FACTORY_H_INCLUDED_2221272137662688__

//! GUID для клиентской фабрики видеопотоков
#define		CLIENT_GRABBER_FACTORY	L"{45EF49AC-BFBE-4E40-ACA5-54F9264331B6}"
//! GUID для серверной фабрики видеопотоков
#define		SERVER_GRABBER_FACTORY	L"{A163DB7C-9145-43e5-8052-6B374AACC157}"
//! для создания заглушки, for tests only : generate image with time
#define		STUB_GRABBER_FACTORY		L"{3A8B28C9-9655-4F87-8C8F-49C7C34B3A10}"
//! for test only : generate clear gray image
#define		STUB_SERVER_GRABBER_FACTORY	L"{62956F0E-FA72-4dc9-8A70-19461819050A}"


#include "../debug_int.h"
//=====================================================================================//
//                                grabber streams		                               //
//=====================================================================================//
//! интерфейс общего поставщика изображений
struct IBaseGrabber
{
	//! освободить интерфейс
	virtual void	Release() = 0;

	//! заблокировать кадр, после использования вызвать UnlockFrame
	//! типичный вариант использования
	//! \code
	//! HRESULT hr;
	//! const BITMAPINFO * ppHeader; const BYTE * ppImage; int   nImgSize ;
	//!	hr = pInt->LockFrame( ppHeader, ppImage, nImgSize );
	//! if( hr == S_OK)
	//! {
	//!		// do something with ppImage
	//!		MyProcessImage( ppHeader, ppImage );
	//!		// unlock frame, go to the next frame 
	//!		pInt->UnlockFrame();
	//! }
	//! \endcode
	//! \param	ppHeader	получает заголовок bmp, валиден до вызова UnlockFrame
	//! \param	ppImage		данные изображения, валиден до вызова UnlockFrame
	//! \param	nImgSize	размер изображения в байтах (для контроля)
	//! \return	S_OK - успех, иначе ошибка
	virtual HRESULT LockFrame( const BITMAPINFO * &ppHeader, const BYTE * &ppImage, int   &nImgSize ) = 0;

	//! разблокировать кадр после LockFrame
	//! \return	S_OK - успех, иначе ошибка
	virtual HRESULT UnlockFrame()	= 0;
};

//! поставщик изображений из видеофайла
struct IFileGrabber : IBaseGrabber
{
	//! взять размер изображения без LockFrame/UnlockFrame
	//! \param	sizeFrame	размер изображения в пикселях
	//! \return	S_OK - успех, иначе ошибка
	virtual HRESULT GetSize( SIZE& sizeFrame )			= 0;

	//! получить номер текущего кадра в видеофайле
	//! \param	nCurPos		номер кадра
	//! \return	S_OK - успех, иначе ошибка
	virtual HRESULT GetCurFrame( INT64 &nCurPos )		= 0;

	//! получить общее кол-во кадров в видеофайле
	//! \param	nTotal		общее кол-во кадров
	//! \return	S_OK - успех, иначе ошибка
	virtual HRESULT GetTotalFrames( INT64 &nTotal )		= 0;

	//! переместить на кадр
	//! \param	nCurPos		переместиться на кадр
	//! \return	S_OK - успех, иначе ошибка
	virtual HRESULT Seek( INT64 nCurPos )				= 0;

	//! получить fps из заголовка файла
	//! \param	fFPS		fps
	//! \return	S_OK - успех, иначе ошибка
	virtual HRESULT GetFPS(double& fFPS)				= 0;
};

//! поставщик изображений от камеры
struct ICameraGrabber : IBaseGrabber
{
};

//! поставщик изображений из архива
struct IArchiveGrabber : ICameraGrabber
{
	//! переместиться на время 
	//! \param nTime	number of seconds since Jan 1, 1970, см. _ftime()
	//! \return	S_OK - успех, иначе ошибка
	virtual HRESULT SeekTo(INT64	nTime)	= 0;	

	//! получить текущее время в потоке архива
	//! \param nTime	number of seconds since Jan 1, 1970, см. _ftime()
	//! \return	S_OK - успех, иначе ошибка
	virtual HRESULT	GetTime( INT64 &nTime)	= 0;
};

//=====================================================================================//
//                                struct IGrabberFactory                               //
//=====================================================================================//

//! интерфейс получения интерфейсов видеопотоков, Abstract Factory
struct IGrabberFactory
{
	//! освободить интерфейс
	virtual void		Release() = 0;

	//! инициализация интерфейса
	//! \param pDebugInt		интерфейс отладки
	//! \param bLocalVideo		разрешить брать клиенту видео без сервера (не используется)
	//! \return	S_OK - успех, иначе ошибка
	virtual HRESULT		Initialize( IDebugOutput* pDebugInt, bool bLocalVideo ) = 0;

	//! передать отладочный интерфейса
	//! \param pDebugInt		интерфейс отладки
	//! \return	S_OK - успех, иначе ошибка
	virtual void		SetDebugOutput( IDebugOutput* pDebugInt ) = 0;

	//! создание интерфейса камеры
	//! \param ppFrameGrabber	интерфейс камеры
	//! \param nCameraID		идентификатор камеры в БД
	//! \return	S_OK - успех, иначе ошибка
	virtual HRESULT		CreateCameraGrabber( ICameraGrabber** ppFrameGrabber, int nCameraID ) = 0;

	//! создание интерфейса файла
	//! \param ppFrameGrabber	интерфейс файла
	//! \param szFileName		имя видеофайла
	//! \param nBitsPerPixel	кол-во бит на пиксель если <= 32, иначе FOURCC кодека
	//! \param bPerFrame		покадровая передача (IBaseGrabber::LockFrame/IBaseGrabber::UnlockFrame сдвигается на 1 кадр вперед) 
	//! \param bLoopback		зацикливание (после последнего кадра идет 1й), для симулирования потоковго видео от камеры
	//! \return	S_OK - успех, иначе ошибка
	virtual HRESULT		CreateFileGrabber(	IFileGrabber** ppFrameGrabber, 
											LPCWSTR  szFileName,
											int nBitsPerPixel, 
											bool bPerFrame, 
											bool bLoopback ) = 0;

	//! создание интерфейса архива
	//! \param ppFrameGrabber	интерфейс архива
	//! \param nCameraID		идентификатор камеры в БД, для которой просматривается архив
	//! \param nStartTime		стартовое время архива, см. IArchiveGrabber::GetTime
	//! \return	S_OK - успех, иначе ошибка
	virtual HRESULT		CreateArchiveGrabber( IArchiveGrabber** ppFrameGrabber, int nCameraID, INT64 nStartTime ) = 0;

};

#endif //__I_GRABBER_FACTORY_H_INCLUDED_2221272137662688__