#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "archive_mode_doc.h"
#include "archive_mode_view.h"
#include "VideoControlView.h"
#include "ArchiveStatView.h"
#include "../DBFacet/DBSerializer.h"
#include "../../CSEngine/DBFacet/CinemaOleDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CArchiveDoc, CArrangeDocument)

BEGIN_MESSAGE_MAP(CArchiveDoc, CArrangeDocument)
  //{{AFX_MSG_MAP(CArchiveDoc)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void	DumpTime( LPCTSTR szDesc, CTime t)
{
//#ifdef _DEBUG
	CString s = t.Format(_T("%H:%M:%S %d.%m.%Y"));
	TCHAR buf[255];
	StringCchPrintf( buf, 255, _T("%s - %s\n"), szDesc, (LPCTSTR)s );
	OutputDebugString( buf );
//#endif
}

class ArchiveHeaderText : public SimpleHeaderText
{
protected:
	CTime	m_time;
public:
	virtual CString Get( )
	{
		return SimpleHeaderText::Get(m_time, true);
	}
	// copy constructor is better
	ArchiveHeaderText( SimpleHeaderText* pHeaderText, CTime time ):
		SimpleHeaderText( *pHeaderText ), 
		m_time( time ) {}

	ArchiveHeaderText( CString sCinema, int nRoomNo, CTime time ) : SimpleHeaderText( sCinema, nRoomNo), m_time( time ){}
};

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CArchiveDoc::CArchiveDoc() : m_bDragged(false)
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CArchiveDoc::~CArchiveDoc()
{
}


//-------------------------------------------------------------------------------------------------
// CArchiveDoc diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CArchiveDoc::AssertValid() const
{
  CArrangeDocument::AssertValid();
}
void CArchiveDoc::Dump( CDumpContext & dc ) const
{
  CArrangeDocument::Dump(dc);
}
#endif //_DEBUG

CVideoControlView*	CArchiveDoc::GetVideoControl()
{
	return GetView<CVideoControlView>();
}


void CArchiveDoc::SeekArchive( int nFilmID)
try 
{
	CCinemaOleDB&	db = dynamic_cast< CCinemaOleDB& >( GetDB() );
	std::wstring sName;
	FilmTime ft;
	bool res = db.GetTableTimetableFacet().GetFilmFromId( nFilmID, ft, sName );
	ASSERT(res);

	GetVideoControl()->SeekVideo( ft.m_timeBegin);
}
catch(CommonException)
{
};

void	CArchiveDoc::UpdateArchive( CTime timeBegin, CTime timeEnd )
{
	if( GetArrangeView()->IsActionDisable() ) return;
	m_timeBegin			= timeBegin;
	m_timeLastFrame		= timeBegin;
	DumpTime( _T("CArchiveDoc::UpdateArchive::timeBegin"), timeBegin );
	DumpTime( _T("CArchiveDoc::UpdateArchive::timeEnd"), timeEnd );
	GetVideoControl()->EnableVideo( timeBegin, timeEnd );
	if( m_ArchiveGrabberMap.empty() )
	{
		for( size_t i = 0; i < m_CameraArr.size(); ++i )
		{
			IArchiveGrabber* pGrabber = GetCameraManager().CreateArchiveGrabber( m_CameraArr[i], timeBegin.GetTime() );
			if( pGrabber )
			{
				m_ArchiveGrabberMap.insert(std::make_pair
					( 
					m_CameraArr[i], 
					boost::shared_ptr<IArchiveGrabber>(pGrabber, ReleaseInterface<IArchiveGrabber>() )  
					) 
					);
			}
		}
		SeekVideoInternal( timeBegin, false, true );
	}
	else
	{
		SeekVideoInternal( timeBegin, true, true );
	}
}

void CArchiveDoc::LoadRoom( int nRoomID)
{
	
	CArrangeDocument::LoadRoom( nRoomID );
	CreateLocationHeader(nRoomID);
	GetVideoControl()->DisableVideo();
}

void	CArchiveDoc::SeekVideo( CTime timeSeek, bool bUpdateImage  )
{
	DumpTime( _T("SeekVideo"), timeSeek );
	SeekVideoInternal( timeSeek, true, bUpdateImage );
}

void	CArchiveDoc::SeekVideoInternal( CTime timeSeek, bool bVideo, bool bUpdateImage )
{
	DumpTime( _T("SeekVideoInternal"), timeSeek );
	SetCurrentTime( timeSeek );
	if( bVideo )
	{
		ArchiveGrabberMap_t::iterator it = m_ArchiveGrabberMap.begin();
		for( ; it != m_ArchiveGrabberMap.end(); ++it )
		{
			it->second->SeekTo( timeSeek.GetTime() );
		}
	}
	if( bUpdateImage )
		UpdateImage(true);
}

void	CArchiveDoc::StepVideo( CTime timeStep )
{
	DumpTime( _T("StepVideo"), timeStep );
	SeekVideoInternal( timeStep, false, true );
}

void	CArchiveDoc::StepVideo()
{
	UpdateImage(true);
}

void	CArchiveDoc::SetCurrentTime( CTime time )
{
	LocationHeaderText* pHeaderText = dynamic_cast<LocationHeaderText*>( m_pHeaderText.get() );
	if( pHeaderText )
		m_pHeaderText = boost::shared_ptr<IHeaderText>( new ArchiveHeaderText( pHeaderText->GetCinema() , pHeaderText->GetRoom(), time ) );
}

void	CArchiveDoc::LoadImageRoomState	( int nStatID )
{
	CArrangeDocument::LoadImageRoomState( nStatID );
	try 
	{
		int		nRoomID, nSpectators;
		CTime	timeSeek;
		CCinemaOleDB&	db = dynamic_cast< CCinemaOleDB& >( GetDB() );
		db.GetTableStatFacet().GetStatInfo( nStatID, nRoomID, timeSeek, nSpectators);
		DumpTime( _T("CArchiveDoc::LoadImageRoomState"), timeSeek );
	//	SeekVideoInternal( timeSeek, false, false );
		m_timeLastFrame = timeSeek;
		GetVideoControl()->SeekForPhoto( timeSeek );
	}
	catch(DataBaseException)
	{

	}
}

void CArchiveDoc::OnActivateMode( bool bActivate)
{
	__super::OnActivateMode( bActivate );
	if( !bActivate )
	{
		m_ArchiveGrabberMap.clear();
		GetView<CArchiveView>()->UnloadRoom();
		GetView<CArchiveStatView>()->Clear();
	}
}

void	CArchiveDoc::LoadArchive( const std::vector<int>& CameraArr )
{
	m_ArchiveGrabberMap.clear();
	m_CameraArr = CameraArr;
}

void	CArchiveDoc::UpdateImage( bool bSetTime )
{
	CArrangeView* pView = GetArrangeView();//->SetImage( *it, &BmpInfo, &BmpBits[0] );
	ArchiveGrabberMap_t::iterator it = m_ArchiveGrabberMap.begin();
	bool bTimeWasSet = false;
	for( ; it != m_ArchiveGrabberMap.end(); ++it )
	{
		int nCameraID = it->first;
		IArchiveGrabber* pGrabber = it->second.get();
		ASSERT( pGrabber );
		const BITMAPINFO*	pbi		= NULL;
		const BYTE*			pBytes	= NULL;
		int					nImageSize;

		const BITMAPINFO*	pResultBmpInfo;
		const BYTE*			pResultBits;

		HRESULT hr = pGrabber->LockFrame( pbi, pBytes, nImageSize );
		if( hr == S_OK )
		{
			try
			{
				m_arc.Process( pbi, (const BYTE*)pBytes, pResultBmpInfo, pResultBits  );
			}
			catch (ArchiveDecompressorException) 
			{
				pResultBits		= NULL;
				pResultBmpInfo	= NULL;
			}
			if( bSetTime && !bTimeWasSet && !m_bDragged)
			{
				bTimeWasSet = true;
				INT64 nTime;
				pGrabber->GetTime( nTime );
				CTime time(nTime);
				SetCurrentTime( time );
				DumpTime( _T("CArchiveDoc::UpdateImage"), time );
				GetVideoControl()->SetExternalTime( time );
				m_timeLastFrame = time;
			}
			pView->SetImage( nCameraID, pResultBmpInfo, pResultBits  );
			pGrabber->UnlockFrame();
		}
		else
		{
			pView->SetImage( nCameraID, NULL, NULL );
		}
	}
	pView->Invalidate(FALSE);
}

void		CArchiveDoc::CreateLocationHeader( int nRoomID )
{
	CString sCinemaName;
	int nRoomNo;
	bool res = GetDB().GetTableRoomFacet().GetRoomInfo( nRoomID, sCinemaName, nRoomNo );
	if(!res)
	{
		CreateEmptyHeader();
	}
	else
		m_pHeaderText = boost::shared_ptr<IHeaderText>(new LocationHeaderText( sCinemaName, nRoomNo ) );
}

void	CArchiveDoc::StopVideo()
{
	m_timeLastFrame = m_timeBegin;
	SeekVideo( m_timeLastFrame );
}

void	CArchiveDoc::PauseVideo()
{
}

void	CArchiveDoc::PlayAfterPause()
{
	DumpTime( _T("PlayAfterPause"), m_timeLastFrame );
	SeekVideo( m_timeLastFrame );
}

void	CArchiveDoc::StartDrag()
{
	m_bDragged = true;
}

void	CArchiveDoc::StopDrag()
{
	m_bDragged = false;
}