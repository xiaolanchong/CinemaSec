//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Simple video window array render class
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 17.06.2005
//                                                                                      //
//======================================================================================//
#ifndef _ARRAY_RENDER_8183614315531119_
#define _ARRAY_RENDER_8183614315531119_

#include "IVideoRender.h"
#include "../VideoImage.h"
#include "../../MultiImageLayout.h"

//======================================================================================//
//                                  class ArrayRender                                   //
//======================================================================================//

class ArrayRender : public IVideoRender
{
	std::vector< VideoImage >	m_VideoWnds;
	std::map<int, VideoImage* > m_WindowID;
	std::vector<int>			m_WindowArrayID;

	SparceRectImageLayout		m_Layout;
	RectImageLayout				m_SingleLayout;
	CString						m_sHeader;

	bool			IsSingleMode() const { return m_nActiveVideo != INVALID_ACTIVE_VIDEO;}

	virtual void	EnterToSingleMode(int nIndex);
	virtual	void	EnterToCommonMode();

	void			DrawVideoImage( Gdiplus::Graphics& gr );
	void			LButton( int nIndex );
	void	Clear()	;

	std::auto_ptr<Gdiplus::Font>	InitFont( );
	void			DrawHeader( Gdiplus::Font& font, Gdiplus::Graphics& gr, CRectF rc,CString sText);
	void			DrawID( Gdiplus::Font& font, Gdiplus::Graphics& gr, CRectF rc, int nID);

	DWORD			m_dwSizeX, m_dwSizeY; 
	bool			m_bShowID;
public:
	ArrayRender(CWnd* pWnd);
	virtual ~ArrayRender();

	virtual void	LoadRoom( int nRoomID, const WindowArray_t& wa )						;
	virtual void	UnloadRoom();
	virtual void	SetImage( int nID, const std::vector<BYTE>& ImageArr)	;
	virtual void	SetImage( int nID, const BITMAPINFO* pBmpInfo, const void* pBits) ;
	virtual void	SetCameraSource( int nID, /*const std::wstring& sSource*/int nCameraID) ;
	virtual void	SetVideoSource( int nID, const std::wstring& sSource ) ;
	virtual void	SetWindowHeader( const LOGFONTW& lf, CString sHeader ) ;
		;
	virtual void	Render( CDC* pDC)	;
	virtual void	Resize( int cx, int cy ) ; 
	virtual bool	CanTakeScreenShot()	;
	virtual boost::shared_ptr<Gdiplus::Image>	TakeScreenShot( );
	virtual void	OnLButtonDblClk( CPoint point );
	virtual void	ClearImage();
	virtual void	ShowID( bool bShow ) { m_bShowID = bShow; } ;

	bool	ForceToSingleMode( int x, int y );
};

#endif // _ARRAY_RENDER_8183614315531119_