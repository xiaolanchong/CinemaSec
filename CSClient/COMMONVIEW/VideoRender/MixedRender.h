//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Õ‡‰ÒÚÓÈÍ‡ Ì‡‰ MergeRender & ArrayRender
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 27.06.2005
//                                                                                      //
//======================================================================================//
#ifndef _MIXED_RENDER_1423998396772036_
#define _MIXED_RENDER_1423998396772036_

#include "IVideoRender.h"
#include "..\..\..\CSChair\Grabber\IGrabberFactory.h"
#include "..\..\..\CSChair\Common\Exception.h"

#include "MergeRender.h"
#include "ArrayRender.h"
//======================================================================================//
//                                  class MixedRender                                   //
//======================================================================================//

class MixedRender : public IVideoRender
{
	boost::shared_ptr<IVideoRender>		m_pArrayRender;
	boost::shared_ptr<IVideoRender>		m_pMergeRender;
	boost::shared_ptr<IVideoRender>		m_pActiveRender;

	std::map< int, boost::shared_ptr<IBaseGrabber> >	m_GrabberMap;
public:
	MixedRender( CWnd* pWnd );
	virtual ~MixedRender();

	virtual void	LoadRoom( int nRoomID, const WindowArray_t& wa )						;
	virtual void	UnloadRoom();

	virtual void	SetImage( int nID, const std::vector<BYTE>& ImageArr)	;
	virtual void	SetImage( int nID, const BITMAPINFO* pBmpInfo, const void* pBits) ;
	virtual void	SetCameraSource( int nID, int nCameraID) ;
	virtual void	SetVideoSource( int nID, const std::wstring& sSource ) ;
	virtual void	SetWindowHeader( const LOGFONTW& lf, CString sHeader  ) ;
//	virtual void	Clear()		;
	virtual void	Render( CDC* pDC);
	virtual void	Resize( int x, int y ); 
	virtual bool	CanTakeScreenShot()	;
	virtual boost::shared_ptr<Gdiplus::Image>	TakeScreenShot( ) ;
	virtual void	OnLButtonDblClk( CPoint point ) ;

	virtual void	Destroy() ;
	virtual void	ClearImage();
	virtual void	ShowID( bool bShow);
};

#endif // _MIXED_RENDER_1423998396772036_