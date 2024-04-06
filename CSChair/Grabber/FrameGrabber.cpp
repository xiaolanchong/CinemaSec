#include "stdafx.h"

#define USE_DSHOW

#include "../FrameGrabber.h"
#include "BaseFrameGrabberDS.h"
#include "BaseFrameGrabberVfw.h"

#include <BOOST/shared_ptr.hpp>
#include <map>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define ERR_GENERAL_ERROR	DWORD(-1)
#define ERR_OK				DWORD(0)

/////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::map< BaseFrameGrabber*, boost::shared_ptr<BaseFrameGrabber> > s_FileGrabbers;

#ifdef _cplusplus
extern "C"
{
#endif

DWORD	WINAPI CreateVideoFileGrabber( BaseFrameGrabber** ppFrameGrabber, int nBitsPerPixel, bool bPerFrame, bool bLoopback	)
{
	try
	{

	boost::shared_ptr<BaseFrameGrabber> Grb;
	if( !bPerFrame )
	{
		boost::shared_ptr<BaseFrameGrabber> Tmp ( new BaseFrameGrabberDS( nBitsPerPixel, bPerFrame, bLoopback ) );
		Grb = Tmp;
	}
	else
	{
		boost::shared_ptr<BaseFrameGrabber> Tmp ( new BaseFrameGrabberVfW( nBitsPerPixel, bPerFrame, bLoopback ) );
		Grb = Tmp;
	}
	s_FileGrabbers.insert( std::make_pair( Grb.get(), Grb ) );
	*ppFrameGrabber = Grb.get();
	return S_OK;
	}
	catch( CommonException& )
	{
		*ppFrameGrabber = NULL;
		return E_FAIL;
	}
}

DWORD	WINAPI DeleteVideoFileGrabber( BaseFrameGrabber* pFrameGrabber		)
{
	std::map< BaseFrameGrabber*, boost::shared_ptr<BaseFrameGrabber> >::iterator it = s_FileGrabbers.find( pFrameGrabber );
	if( it == s_FileGrabbers.end() )
	{
		return E_INVALIDARG;
	}
	else
	{
		s_FileGrabbers.erase( it );
		return S_OK;
	}
}

#ifdef _cplusplus
}
#endif