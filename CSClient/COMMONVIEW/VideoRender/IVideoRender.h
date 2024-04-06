//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Interface for video & image render
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 17.06.2005
//                                                                                      //
//======================================================================================//
#ifndef _I_VIDEO_RENDER_6027872486496396_
#define _I_VIDEO_RENDER_6027872486496396_
//======================================================================================//
//                                 class IVideoRender                                  //
//======================================================================================//

#include "../wv_message.h"
#include <boost/shared_ptr.hpp>

class IVideoRender
{
protected:
	CWnd*			m_pWnd;
	int				m_nActiveVideo;
	LOGFONTW		m_lf;
	enum
	{
		INVALID_ACTIVE_VIDEO = -1
	};
public:
	IVideoRender( CWnd* pWnd ) : m_pWnd( pWnd ) , m_nActiveVideo(INVALID_ACTIVE_VIDEO) {}
	virtual ~IVideoRender() {};

	virtual void	LoadRoom( int nRoomID, const WindowArray_t& wa )						= 0;
	virtual void	UnloadRoom() = 0;
	virtual void	SetImage( int nID, const std::vector<BYTE>& ImageArr)	= 0;
	virtual void	SetImage( int nID, const BITMAPINFO* pBmpInfo, const void* pBits) = 0;
	virtual void	SetCameraSource( int nID, /*const std::wstring& sSource*/int nCameraID) = 0;
	virtual void	SetVideoSource( int nID, const std::wstring& sSource ) = 0;
	virtual void	SetWindowHeader( const LOGFONTW& lf, CString sHeader ) = 0;
//	virtual void	Clear()		= 0;
	virtual void	Render( CDC* pDC)	= 0;
	virtual void	Resize( int x, int y )	= 0; 
	virtual bool	CanTakeScreenShot()		= 0;
	virtual boost::shared_ptr<Gdiplus::Image>	TakeScreenShot( ) = 0;
	virtual void	OnLButtonDblClk( CPoint point ) = 0;

	virtual void	Destroy() {};
	virtual void	ClearImage() = 0;
	virtual void	ShowID( bool bShow ) = 0;
};

#endif // _I_VIDEO_RENDER_6027872486496396_