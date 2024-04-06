// VideoDocument.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "csclient.h"
#include "ArrangeDocument.h"
#include "ArrangeView.h"
#include "ArrangeStatView.h"
#include "../DBFacet/DBSerializer.h"
#include "../../CSEngine/DBFacet/CinemaOleDB.h"
#include "../Layout/LayoutValidate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class EmptyHeaderText : public IHeaderText
{
public:
	virtual CString Get( )	{ return CString();	}
};

class SpectatorHeaderText : public SimpleHeaderText
{
protected:
	int			m_nSpectators;
	CTime		m_timeStat;
public:
	virtual CString Get( )
	{
		CString sFmtHeader;
		CString sDateStat = GetLocalizedDateTime( m_timeStat, false );
		sFmtHeader.Format( IDS_FMT_ARCHIVE_HEADER, 
			(LPCTSTR)m_sCinema, m_nRoomNo, (LPCTSTR)sDateStat, m_nSpectators  );
		return sFmtHeader;
	}

	SpectatorHeaderText( CString sCinema, int nRoom, int nSpectators, CTime timeStat ): 
		SimpleHeaderText(sCinema, nRoom), m_nSpectators(nSpectators), m_timeStat(timeStat)  
		{}
};

/////////////////////////////////////////////////////////////////////////////
// CArrangeDocument

IMPLEMENT_DYNCREATE(CArrangeDocument, CDocument)

CArrangeDocument::CArrangeDocument() : 
	m_pHeaderText( new EmptyHeaderText )
{
}

CArrangeDocument::~CArrangeDocument()
{
}


BEGIN_MESSAGE_MAP(CArrangeDocument, CDocument)
	//{{AFX_MSG_MAP(CArrangeDocument)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArrangeDocument diagnostics

#ifdef _DEBUG
void CArrangeDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CArrangeDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CArrangeDocument commands

CArrangeView*		CArrangeDocument::GetArrangeView()
{
	return GetView<CArrangeView>();
}

CArrangeStatView*	CArrangeDocument::GetArrangeStatView()
{
	return GetView<CArrangeStatView>();
}

void	CArrangeDocument::LoadRoom			( int nRoomID )
try 
{
	std::set<int> SelectedCameras = GetDB().GetTableCameraFacet().SelectCamerasFromRoom( nRoomID );
	const std::set<CameraRow>& Cameras = GetDB().GetTableCameraFacet().GetCameras();
	std::set<int>::const_iterator itCam = SelectedCameras.begin();
	WindowArray_t wa;
	CArrangeView* pView = GetArrangeView();

	LayoutValidate vl;
	if( vl.CheckLayoutInRoom( GetDB(), nRoomID  ) )
		for( ; itCam != SelectedCameras.end(); ++itCam  )
		{
			std::set<CameraRow>::const_iterator it = Cameras.find( CameraRow(*itCam ) );
			ASSERT( it != Cameras.end() );
			VideoWindowInfo vwi;
			vwi.m_X			= it->m_Pos.first;
			vwi.m_Y			= it->m_Pos.second;
			vwi.m_nID		= it->m_nID;

			NameParse np(it->m_sGUID.c_str());
			wa.push_back( vwi );
		}
#if 1
	CreateEmptyHeader();
#else
	CreateVideoHeader(nRoomID);
#endif
	
	pView->LoadRoom( nRoomID, wa );
}
catch(CommonException)
{
};

void	CArrangeDocument::LoadImageRoomState	( int nStatID )
try 
{
	CCinemaOleDB&	db = dynamic_cast< CCinemaOleDB& >( GetDB() );
	PhotoMap_t PhMap = db.GetTableStatFacet().GetImages(nStatID);
	const std::set<CameraRow>& CamSet = db.GetTableCameraFacet().GetCameras();
	PhotoMap_t::const_iterator itPh = PhMap.begin();
	CArrangeView* pView = GetArrangeView();
	pView->ClearImage();
	for( ; itPh != PhMap.end(); ++itPh)
	{
		std::set<CameraRow>::const_iterator it = CamSet.find( itPh->first );
		ASSERT( it != CamSet.end() );
		GetArrangeView()->SetImage( itPh->first, *itPh->second );
	}
#if 0
	CString sFmtHeader = GetImageHeader( db, nStatID );
	pView->SetWindowHeader( sFmtHeader );
#else
	CreateImageHeader( db, nStatID );
#endif

}
catch(...)
{
	CArrangeView* pView = GetArrangeView();
	pView->ClearImage();
	CreateEmptyHeader();
};

void	CArrangeDocument::LoadVideoRoomState	( int nRoomID )
try
{
	
	std::set<int> SelectedCameras = GetDB().GetTableCameraFacet().SelectCamerasFromRoom( nRoomID );
	const std::set<CameraRow>& Cameras = GetDB().GetTableCameraFacet().GetCameras();
	std::set<int>::const_iterator itCam = SelectedCameras.begin();
#if 0
	CArrangeView* pView = GetArrangeView();
	CString sFmtHeader = GetVideoHeader(nRoomID);
	pView->SetWindowHeader( sFmtHeader );
#else
	CreateVideoHeader( nRoomID );
#endif
	for( ; itCam != SelectedCameras.end(); ++itCam  )
	{
		std::set<CameraRow>::const_iterator it = Cameras.find( CameraRow(*itCam ) );
		ASSERT( it != Cameras.end() );
		VideoWindowInfo vwi;
		vwi.m_X			= it->m_Pos.first;
		vwi.m_Y			= it->m_Pos.second;
		vwi.m_nID		= it->m_nID;

		NameParse np(it->m_sGUID.c_str());
		int nRoomID = it->m_nID;
		switch( np.GetType() )
		{
			case vidt_camera :
				GetArrangeView()->SetCameraSource( nRoomID, /*np.GetSource() */ nRoomID);
				break;
			case vidt_file:
				GetArrangeView()->SetVideoSource( nRoomID, np.GetSource() );
				break;
		}
		
	}
}

catch (CommonException) 
{
	CArrangeView* pView = GetArrangeView();
	pView->ClearImage();
	CreateEmptyHeader();
};

void		CArrangeDocument::CreateEmptyHeader()
{
	m_pHeaderText = boost::shared_ptr<IHeaderText>(new EmptyHeaderText );
}

void		CArrangeDocument::CreateVideoHeader( int nRoomID )
{
	CString sCinemaName;
	int nRoomNo;
	bool res = GetDB().GetTableRoomFacet().GetRoomInfo( nRoomID, sCinemaName, nRoomNo );
	if(!res)
	{
		CreateEmptyHeader();
	}
	else
		m_pHeaderText = boost::shared_ptr<IHeaderText>(new SimpleHeaderText( sCinemaName, nRoomNo ) );
}

void		CArrangeDocument::CreateImageHeader( CCinemaOleDB& db, int nStatID )
try
{
	int nRoomID, nSpectators, nRoomNo;
	CTime	timeStat;
	CString sCinemaName;
	db.GetTableStatFacet().GetStatInfo( nStatID, nRoomID, timeStat, nSpectators );
	db.GetTableRoomFacet().GetRoomInfo( nRoomID, sCinemaName, nRoomNo );
	m_pHeaderText = boost::shared_ptr<IHeaderText>(new SpectatorHeaderText
		( sCinemaName, nRoomNo, nSpectators, timeStat ));
}
catch( DataBaseException )
{
	CreateEmptyHeader();
};

struct Compare2nd
{
	bool operator() ( const std::pair<CTime, int> & _1st, const std::pair<CTime, int> & _2nd  ) const 
	{
		return _1st.second < _2nd.second;
	}
};

int		CArrangeDocument::GetFilmSpectatorNumber( const  MeasuredFilm_t& mf) const
{
	if( mf.empty() ) throw SpectatorException("No spectator count");
#if 0
	return mf.back().second;
#else
	MeasuredFilm_t::const_iterator it = std::max_element( mf.begin(), mf.end(), Compare2nd() );
	return it->second;
#endif
}

CString		CArrangeDocument::GetLocalizedDate( CTime time)
{
	TCHAR	szBuf[255];
	SYSTEMTIME st;
	time.GetAsSystemTime(st);
//	const bool bUseCustom = true;
#if 0
	int res = GetDateFormat( LOCALE_USER_DEFAULT,  DATE_SHORTDATE, &st, NULL, szBuf, 255);
#else
	int res = GetDateFormat( LOCALE_USER_DEFAULT,  0, &st, _T("dd MMM yy"), szBuf, 255);
#endif
	ASSERT( res );
	return CString (szBuf); 
}

void	CArrangeDocument::UpdateRoom()
{
	CArrangeStatView* pView = GetView< CArrangeStatView > ();
	LoadRoom( pView->GetCurRoom() );
}

void	CArrangeDocument::DisableAction( bool bDisableAction )
{
	CArrangeView* pView = GetView< CArrangeView > ();
	pView->DisableAction( bDisableAction );
}

void	CArrangeDocument::OnActivateMode( bool bActive )
{
	if( bActive )
	{
		CArrangeStatView* pView = GetView< CArrangeStatView > ();
		LoadRoom( pView->GetCurRoom() );
	}
	else
	{
	//	UnloadRoom(  );
	}
}

std::vector< std::pair<int, CString> > CArrangeDocument::GetRoomCamera( int nRoomID )
{
	std::set<int> SelectedCameras = GetDB().GetTableCameraFacet().SelectCamerasFromRoom( nRoomID );
	const std::set<CameraRow>& Cameras = GetDB().GetTableCameraFacet().GetCameras();
	std::set<int>::const_iterator itCam = SelectedCameras.begin();

	std::vector< std::pair<int, CString> > CamMap;
	for( ; itCam != SelectedCameras.end(); ++itCam  )
	{
		std::set<CameraRow>::const_iterator it = Cameras.find( CameraRow(*itCam ) );
		ASSERT( it != Cameras.end() );
		CamMap.push_back( std::make_pair( it->m_nID,  CString(it->m_sIP.c_str()) ) );
	}
	return CamMap;
}