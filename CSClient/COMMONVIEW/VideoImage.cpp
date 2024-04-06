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
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "VideoImage.h"

#include "..\..\CSChair\public\NameParse.h"
#include "..\..\CSChair\FrameGrabber.h"

#include "..\Background\EmptyBackground.h"
#include "..\Background\PicBackground.h"
#include "..\Background\VideoBackground.h"
#include "..\Background\ImageBackground.h"
#include "..\Background\FrameBackground.h"

#include "..\Video\CameraManager.h"

#include <boost/shared_ptr.hpp>
//=====================================================================================//
//                                  class VideoImage                                   //
//=====================================================================================//
VideoImage::VideoImage():
	m_pBackground( new CEmptyBackground )
{
}

VideoImage::~VideoImage()
{
}

void	VideoImage::Draw( Gdiplus::Graphics& gr, const CRect& rcDraw )
{
	Gdiplus::Color cl; 
	cl.SetFromCOLORREF( ::GetSysColor( COLOR_BTNSHADOW) );	
	Gdiplus::SolidBrush br( cl );
	gr.FillRectangle( &br, CRectF(rcDraw) );
	m_pBackground->Draw( gr, CRectF( rcDraw )  );
}

bool	VideoImage::SetCameraSource( /*LPCTSTR szSource*/int nCameraID)
{
	ICameraGrabber* pGrb = GetCameraManager().CreateGrabber( nCameraID );
	
	boost::shared_ptr<IBaseGrabber> Tmp( pGrb, ReleaseInterface<IBaseGrabber>() );

	boost::shared_ptr<CBackground> Temp( new CFrameBackground( Tmp) );
	m_pBackground = Temp;
	if( !pGrb ) return false; 
	return true;
}

bool	VideoImage::SetFileSource( LPCTSTR szSource)
{
	try
	{
		IBaseGrabber* pGrb = GetCameraManager().CreateFileGrabber( szSource );

		boost::shared_ptr<IBaseGrabber> Tmp( pGrb, ReleaseInterface<IBaseGrabber>() );

		boost::shared_ptr<CBackground> Temp( new CFrameBackground( Tmp) );
		m_pBackground = Temp;
		if( !pGrb ) return false; 
		return true;
	}
	catch(CommonException&)
	{
		return false;
	}
}

bool	VideoImage::SetBackground( const void* pBytes, size_t nSize )
{
	try
	{
		try
		{
			boost::shared_ptr<CBackground> Temp( new CImageBackground (pBytes, nSize) );
			m_pBackground = Temp;
			return true;
		}
		catch(ImageBgException&)
		{

			const BITMAPINFO* pBmp = (const BITMAPINFO*)pBytes;
			size_t nPalOffset =  pBmp->bmiHeader.biBitCount <= 8 ?  (1 << pBmp->bmiHeader.biBitCount) * sizeof(RGBQUAD) : 0 ;
#if 0
			const RGBQUAD* pPal = &pBmp->bmiColors[0];
#endif
			size_t nTotalOffset = sizeof(BITMAPINFOHEADER) + nPalOffset;
			const BYTE* pBits = (const BYTE*)pBmp + nTotalOffset;
#ifdef CONVERT_TO_TRUECOLOR
			BITMAPINFO	NewBi;
			memset(&NewBi, 0, sizeof(BITMAPINFO ));
			NewBi.bmiHeader.biBitCount	= 24;
			NewBi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
			NewBi.bmiHeader.biSizeImage	= pBmp->bmiHeader.biSizeImage * 3;
			NewBi.bmiHeader.biHeight	= pBmp->bmiHeader.biHeight;
			NewBi.bmiHeader.biWidth		= pBmp->bmiHeader.biWidth;
			NewBi.bmiHeader.biPlanes	= 1;

			std::vector<BYTE> NewImage( pBmp->bmiHeader.biSizeImage * 3 );
			for(size_t i = 0; i < pBmp->bmiHeader.biSizeImage; ++i)
			{
				BYTE * p = &NewImage[ i * 3 ];
				p[0] = p[1] = p[2] = pBits[ i ];
			}
#endif
			boost::shared_ptr<CBackground> Temp( new CBitmapBackground ((BITMAPINFO*)pBmp, (void*)pBits));
			m_pBackground = Temp;
		}

	}
	catch(CommonException&)
	{
		return false;
	}
	return true;
}

bool	VideoImage::SetBackground( const BITMAPINFO* pBmpInfo, const void* pBits )
try
{
	if( pBmpInfo == NULL || pBits == NULL)
	{
		boost::shared_ptr<CBackground> Temp( new CEmptyBackground );
		m_pBackground = Temp;
	}
	else
	{
#if 0
		const BITMAPINFO*	pResultBmpInfo;
		const void*			pResultBits;
		if( pBmpInfo->bmiHeader.biCompression != BI_RGB )
		{
			ImageDecompressor id( ;
		}
#endif
		boost::shared_ptr<CBackground> Temp( new CBitmapBackground (pBmpInfo, pBits));
		m_pBackground = Temp;
	}
	return true;
}
catch(CommonException&)
{
	return false;
}

void	VideoImage::ClearBackground()
{
	boost::shared_ptr<CBackground> Temp(new CEmptyBackground);
	m_pBackground = Temp;
}

