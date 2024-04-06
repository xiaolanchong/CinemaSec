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
#include "stdafx.h"
#include "FileGrabberFactory.h"
#include "BaseFrameGrabberDS.h"
#include "BaseFrameGrabberVfW.h"

#include "AVDGrabber.h"
#include <Shlwapi.h>

#include "..\public\memleak.h"

//======================================================================================//
//                               class FileGrabberFactory                               //
//======================================================================================//
FileGrabberFactory::FileGrabberFactory()
{
}

FileGrabberFactory::~FileGrabberFactory()
{
}

HRESULT		FileGrabberFactory::CreateFileGrabber(	IFileGrabber** ppFrameGrabber, 
														 LPCWSTR szFileName,
														 int nBitsPerPixel, 
														 bool bPerFrame, 
														 bool bLoopback ) 
{
	*ppFrameGrabber = NULL;
	if( !ppFrameGrabber) return E_POINTER;

	LPCWSTR szExt = PathFindExtensionW( szFileName );
	if( lstrcmpiW(szExt, L".avd" ) == 0 )
	{
		return CreateAVDFileGrabber( ppFrameGrabber, szFileName, nBitsPerPixel, bLoopback );
	}
	else
		return CreateNormalFileGrabber( ppFrameGrabber, szFileName, nBitsPerPixel, bPerFrame, bLoopback );
}

HRESULT		FileGrabberFactory::CreateNormalFileGrabber(	IFileGrabber** ppFrameGrabber, 
															   LPCWSTR szFileName,
															   int nBitsPerPixel, 
															   bool bPerFrame, 
															   bool bLoopback ) 
{
	if( nBitsPerPixel != 32 && 
		nBitsPerPixel != 24 && 
		nBitsPerPixel != 16 && 
		nBitsPerPixel != 8	&&
		nBitsPerPixel != MY_BI_YUY2 &&
		nBitsPerPixel != MY_BI_GRAY &&
		nBitsPerPixel != MY_BI_Y800 ) 
		return E_INVALIDARG;
	IFileGrabber* pFileGrabber;
	try
	{
		BaseFrameGrabber *pGrab = NULL;
		if( bPerFrame) 
		{
			BaseFrameGrabberVfW *pGrabVfW	= new BaseFrameGrabberVfW( nBitsPerPixel, bPerFrame, bLoopback );
			pFileGrabber					= pGrabVfW;
			pGrab							= pGrabVfW;
		}
		else
		{
			BaseFrameGrabberDS *pGrabDS = new BaseFrameGrabberDS( nBitsPerPixel, bPerFrame, bLoopback );
			pFileGrabber				= pGrabDS ;
			pGrab						= pGrabDS;
		}
		FrameGrabberParameters parm;
		bool res = pGrab->Start( szFileName, parm ); 
		if( !res )
		{
			pFileGrabber->Release();
			return E_FAIL;
		}
	}
	catch(DirectShowException2 )
	{
		return E_FAIL;
	}
	catch( CommonException )
	{
		return E_FAIL;
	}
	*ppFrameGrabber = pFileGrabber;
	return S_OK;
}

HRESULT	FileGrabberFactory::CreateAVDFileGrabber(IFileGrabber** ppFrameGrabber, 
														LPCWSTR  szFileName,
														int nBitsPerPixel, 
														bool bLoopback ) 
														try
{
	*ppFrameGrabber = new AVDGrabber( szFileName, nBitsPerPixel, bLoopback, m_Debug.Get() );
	return S_OK;
}
catch ( AVDGrabberException ) 
{
	*ppFrameGrabber = NULL;
	return E_FAIL;
};