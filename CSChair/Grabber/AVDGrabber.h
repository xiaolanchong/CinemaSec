//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	обертка над проигрывателем формата AVD
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 15.08.2005
//                                                                                      //
//======================================================================================//
#ifndef _A_V_D_GRABBER_5629935164311137_
#define _A_V_D_GRABBER_5629935164311137_

#include "../../CSUtility/csutility.h"
#include "../../CSUtility/alib/video_interface.h"
#include "IGrabberFactory.h"
#include "../common/Exception.h"

MACRO_EXCEPTION( AVDGrabberException, CommonException ) 
//======================================================================================//
//                                   class AVDGrabber                                   //
//======================================================================================//

//! \brief обертка над проигрывателем формата AVD, см CSUtility
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class AVDGrabber : public IFileGrabber
{
	//! suicide machine
	virtual void	Release() { delete this; };

	virtual HRESULT LockFrame( const BITMAPINFO * &ppHeader, const BYTE * &ppImage, int   &nImgSize );
	virtual HRESULT UnlockFrame()	{ return S_OK; }

	virtual HRESULT GetSize( SIZE& sizeFrame )			;
	virtual HRESULT GetCurFrame( INT64 &nCurPos )		;
	virtual HRESULT GetTotalFrames( INT64 &nTotal )		;
	virtual HRESULT Seek( INT64 nCurPos )				;
	virtual HRESULT GetFPS(double& fFPS)				;

	//! AVD-граббер 
	std::auto_ptr<avideolib::IVideoReader>		m_pGrabber;
	//! бит/пиксель, игнорируется
	int											m_bpp;
	//! номер текущего кадра
	int											m_nCurFrame;
	//! буфер для кадра
	std::vector<BYTE>							m_LockCache;
	//! буфер для кадра
	Arr2ub										m_ByteCache;
	//! зациклить?
	bool										m_bLoop;
	//! буфер для заголовка
	BITMAPINFO									m_cacheHdr;
public:
	//! создать граббер с параметрами
	//! \param szFileName имя открывемого файла
	//! \param bpp бит/пиксель на выходе
	//! \param bLoop зациклить?
	//! \param pDbgInt интерфейс отладки
	AVDGrabber( LPCWSTR szFileName, int bpp, bool bLoop, IDebugOutput* pDbgInt );
	virtual ~AVDGrabber();
};

#endif // _A_V_D_GRABBER_5629935164311137_