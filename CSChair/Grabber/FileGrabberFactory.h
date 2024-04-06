//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Создание проигрывателя видеофайлов
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 19.08.2005
//                                                                                      //
//======================================================================================//
#ifndef _FILE_GRABBER_FACTORY_6400758542943037_
#define _FILE_GRABBER_FACTORY_6400758542943037_

#include "IGrabberFactory.h"

//======================================================================================//
//                               class FileGrabberFactory                               //
//======================================================================================//

//! \brief создает файловые грабберы
//! \version 1.0
//! \date 10-18-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//!
class FileGrabberFactory
{
protected:
	FileGrabberFactory();
	virtual ~FileGrabberFactory();

	//! обертка над отладочным интерфейсом
	MyDebugOutputImpl				m_Debug;

	//! созадет файловый граббер с помощью VFW
	//! \param ppFrameGrabber выходной граббер
	//! \param szFileName имя видео файла
	//! \param nBitsPerPixel бит/пиксель выходного изображения или FOURCC если < 32
	//! \param bPerFrame покадрово?
	//! \param bLoopback зациклить?
	//! \return S_OK - success, other - failing
	HRESULT		CreateNormalFileGrabber(	IFileGrabber** ppFrameGrabber, 
											LPCWSTR  szFileName,
											int nBitsPerPixel, 
											bool bPerFrame, 
											bool bLoopback ) ;

	//! созадет файловый граббер с помощью AVD проигрывателя из CSUtility
	//! \param szFileName имя видео файла
	//! \param nBitsPerPixel бит/пиксель выходного изображения, игнорируется, всегда 24
	//! \param bPerFrame покадрово?
	//! \param bLoopback зациклить?
	//! \return S_OK - success, other - failing
	HRESULT		CreateAVDFileGrabber(	IFileGrabber** ppFrameGrabber, 
										LPCWSTR  szFileName,
										int nBitsPerPixel, 
										bool bLoopback ) ;

	//! переключается по расширению на CreateNormalFileGrabber или CreateAVDFileGrabber
	//! \param ppFrameGrabber выходной граббер
	//! \param szFileName имя видео файла
	//! \param nBitsPerPixel бит/пиксель выходного изображения или FOURCC если < 32
	//! \param bPerFrame покадрово?
	//! \param bLoopback зациклить?
	//! \return S_OK - success, other - failing
	HRESULT		CreateFileGrabber(	IFileGrabber** ppFrameGrabber, 
									LPCWSTR  szFileName,
									int nBitsPerPixel, 
									bool bPerFrame, 
									bool bLoopback ) ;

};

#endif // _FILE_GRABBER_FACTORY_6400758542943037_