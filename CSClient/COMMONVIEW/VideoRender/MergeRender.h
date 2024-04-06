//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	CVMCad component render
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 20.06.2005
//                                                                                      //
//======================================================================================//
#ifndef _MERGE_RENDER_4784860685322462_
#define _MERGE_RENDER_4784860685322462_

#include "IVideoRender.h"
#include "..\..\..\CVMCad\CVMCadComponent\Sources\CVMCadComponent.hpp"
#include "..\..\..\CSChair\Grabber\IGrabberFactory.h"

#include "..\..\..\CSChair\Common\Exception.h"

MACRO_EXCEPTION( RenderException,				CommonException );
MACRO_EXCEPTION( RenderCreateException,			RenderException );
MACRO_EXCEPTION( RenderInvalidDeviceException,	RenderException );
MACRO_EXCEPTION( RenderLoadModelException,		RenderException );
//======================================================================================//
//                                  class MergeRender                                   //
//======================================================================================//

class MergeRender : public IVideoRender
{
	typedef std::map< int, std::pair<int, int> >	PosMap_t;
	PosMap_t									m_IdPositionMap;

	std::vector< std::pair< int, boost::shared_ptr<IBaseGrabber> > >	m_GrabberMap;
	// <-- static
	static boost::shared_ptr< IRoomRender >		m_pRender;	
	static LONG									m_nRenderRef;
	// <-- static
	std::vector<BYTE>							m_cacheArr;
	BITMAPINFO									m_cacheBi;

	std::pair<int, int>	GetPosFromID( int nID );
	void				SetDefaultState();
public:
	MergeRender( CWnd* pCreateWnd, CWnd* pWnd );
	virtual ~MergeRender();

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
	virtual void	ShowID( bool ) {}; // not used

	std::pair<int, int>	GetCamera( CPoint pt) { return m_pRender->GetCamera( pt ); }

};

#endif // _MERGE_RENDER_4784860685322462_