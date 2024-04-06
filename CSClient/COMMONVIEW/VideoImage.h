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
//   Date:   26.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __VIDEO_IMAGE_H_INCLUDED_3284283781111726__
#define __VIDEO_IMAGE_H_INCLUDED_3284283781111726__

#include "..\Background\Background.h"
#include <boost/shared_ptr.hpp>

//=====================================================================================//
//                                  class VideoImage                                   //
//=====================================================================================//
class VideoImage
{
	boost::shared_ptr<CBackground>	m_pBackground;

public:
	//VideoImage( const  
	VideoImage();
	virtual ~VideoImage();

	bool	SetCameraSource( /*LPCTSTR szSource*/int nCameraID);
	bool	SetFileSource( LPCTSTR szSource);
	bool	SetBackground( const void* pBytes, size_t nSize );
	bool	SetBackground( const BITMAPINFO* pBmpInfo, const void* pBits );
	void	ClearBackground();

	void	Draw( Gdiplus::Graphics& gr, const CRect& rcDraw );

	CBackground* GetBackground() { return m_pBackground.get();}
	boost::shared_ptr<CBackground> GetBackgroundPtr() const { return m_pBackground; }
};

#endif //__VIDEO_IMAGE_H_INCLUDED_3284283781111726__