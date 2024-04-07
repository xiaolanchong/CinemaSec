//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Íàäñòðîéêà íàä MergeRender & ArrayRender
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 27.06.2005
//                                                                                      //
//======================================================================================//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "MixedRender.h"
#include "../../../CSChair/debug_int.h"
#include "../../DBFacet/DBSerializer.h"
#include "../../Video/CameraManager.h"
//======================================================================================//
//                                  class MixedRender                                   //
//======================================================================================//
MixedRender::MixedRender(  CWnd* pWnd ):
	m_pArrayRender( new ArrayRender(pWnd) ),
	m_pMergeRender( new MergeRender(pWnd, pWnd) ),
	m_pActiveRender( m_pMergeRender ), 
	IVideoRender(pWnd)
{

}

MixedRender::~MixedRender()
{
}

void	MixedRender::LoadRoom( int nRoomID, const WindowArray_t& wa )
{
//	Clear();
	m_pArrayRender->LoadRoom( nRoomID, wa );
	m_pMergeRender->LoadRoom( nRoomID, wa );
	m_pActiveRender = m_pMergeRender;
}

void	MixedRender::SetImage( int nID, const std::vector<BYTE>& ImageArr)
{
	m_pArrayRender->SetImage( nID, ImageArr );
	m_pMergeRender->SetImage( nID, ImageArr );
}

void	MixedRender::SetImage( int nID, const BITMAPINFO* pBmpInfo, const void* pBits)
{
	m_pArrayRender->SetImage( nID, pBmpInfo, pBits );
	m_pMergeRender->SetImage( nID, pBmpInfo, pBits );
}

void	MixedRender::SetWindowHeader( const LOGFONTW& lf, CString sHeader  )
{
	m_pArrayRender->SetWindowHeader( lf, sHeader );
	m_pMergeRender->SetWindowHeader( lf, sHeader );
}

void	MixedRender::UnloadRoom()
{
	m_GrabberMap.clear();
	m_pArrayRender->UnloadRoom(  );
	m_pMergeRender->UnloadRoom(  );
}

void	MixedRender::Render( CDC* pDC)
{
	std::map< int, boost::shared_ptr<IBaseGrabber> >::iterator it = m_GrabberMap.begin();

	const BITMAPINFO*	pBi;
	const BYTE*			pByte;
	int					nImgSize;
	for( ; it != m_GrabberMap.end(); ++it )
	{
		IBaseGrabber* pGrabber = it->second.get();
		HRESULT hr = pGrabber->LockFrame(pBi, pByte, nImgSize);
		if( hr == S_OK )
		{
			SetImage( it->first, pBi, pByte );
			pGrabber->UnlockFrame();
		}
		else
		{
	// NOTE	may set null images or remain ones
			SetImage( it->first, NULL, NULL );
		}
	}

	m_pActiveRender->Render( pDC );
}

void	MixedRender::Resize( int x, int y )
{
	m_pArrayRender->Resize( x, y );
	m_pMergeRender->Resize( x, y );
}

bool	MixedRender::CanTakeScreenShot()
{
#if 0
	return m_pActiveRender->TakeScreenShot();
#else
	return m_pActiveRender->CanTakeScreenShot();
#endif
}

boost::shared_ptr<Gdiplus::Image>	MixedRender::TakeScreenShot( ) 
{
	return m_pActiveRender->TakeScreenShot();
}

void	MixedRender::OnLButtonDblClk( CPoint point )
{
	MergeRender* pMergeRender = dynamic_cast<MergeRender*> ( m_pActiveRender.get() );
	if( pMergeRender )
	{
		std::pair<int, int > CamPos = pMergeRender->GetCamera( point );
		ArrayRender* pArrayRender = dynamic_cast< ArrayRender* >( m_pArrayRender.get() );
		ASSERT( pArrayRender );
		if( pArrayRender->ForceToSingleMode( CamPos.first, CamPos.second ) )
		{
			m_pActiveRender = m_pArrayRender;
		}
	}
	else
	{
		m_pActiveRender = m_pMergeRender;
	}
	m_pWnd->Invalidate();
}

void	MixedRender::Destroy()
{
	m_pActiveRender.reset();
	m_pMergeRender->Destroy();
	m_pArrayRender->Destroy();
}

void	MixedRender::SetCameraSource( int nID, int nCameraID)
{
	IBaseGrabber* pGrb = GetCameraManager().CreateGrabber( nCameraID );
	if( pGrb )
	{
		boost::shared_ptr<IBaseGrabber> Tmp( pGrb, ReleaseInterface<IBaseGrabber>() );	
		m_GrabberMap.insert( std::make_pair( nID, Tmp ) );
	}
	else
	{
		m_GrabberMap.erase( nID );
	}
}

void	MixedRender::SetVideoSource( int nID, const std::wstring& sSource )
{
	IBaseGrabber* pGrb = GetCameraManager().CreateFileGrabber( sSource.c_str() );
	if( pGrb )
	{
		boost::shared_ptr<IBaseGrabber> Tmp( pGrb, ReleaseInterface<IBaseGrabber>() );	
		m_GrabberMap.insert( std::make_pair( nID, Tmp ) );
	}
	else
	{
		m_GrabberMap.erase( nID );
	}
}

void	MixedRender::ClearImage()
{
	if( m_pArrayRender.get() ) m_pArrayRender->ClearImage();
	if( m_pMergeRender.get() ) m_pMergeRender->ClearImage();
}

void	MixedRender::ShowID( bool bShow)
{
	if( m_pArrayRender.get() ) m_pArrayRender->ShowID( bShow );
	if( m_pMergeRender.get() ) m_pMergeRender->ShowID( bShow );
}