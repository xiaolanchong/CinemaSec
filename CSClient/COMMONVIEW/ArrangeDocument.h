#pragma once

#include "wv_message.h"

class CArrangeView;
class CArrangeStatView;
class CCinemaOleDB;

typedef std::vector< std::pair< CTime, int > >	MeasuredFilm_t;
MACRO_EXCEPTION( SpectatorException, CommonException );

struct IHeaderText
{
	virtual CString Get( ) = 0;
};

class CArrangeDocument : public CDocument
{
	WindowArray_t	m_Windows;
protected:
	CArrangeDocument();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CArrangeDocument)

	template <class T>	T*	GetView()
	{
		POSITION pos = GetFirstViewPosition();
		while( pos )
		{
			CView* pView = GetNextView( pos );
			if( pView->IsKindOf( RUNTIME_CLASS( T ) ) ) return (T*)pView;
		}
		return 0;
	}

	CArrangeView*		GetArrangeView();
	CArrangeStatView*	GetArrangeStatView();

	void				CreateVideoHeader( int nRoomID );
	void				CreateImageHeader( CCinemaOleDB& db, int nStatID );
	void				CreateEmptyHeader();

	boost::shared_ptr<IHeaderText>	m_pHeaderText;
// Attributes
public:
	CString			GetHeaderText() { return m_pHeaderText->Get( ); };
	virtual void	LoadRoom			( int nRoomID );

	virtual void	LoadImageRoomState	( int nStatID );
	virtual void	LoadVideoRoomState	( int nRoomID );

	int			GetFilmSpectatorNumber( const  MeasuredFilm_t& mf) const; // throw SpectatorException

	static	CString		GetLocalizedDate( CTime time) ;

	virtual void	UpdateRoom();
	
	void			DisableAction( bool bDisableAction );
	virtual void	OnActivateMode( bool bActive );

	std::vector< std::pair<int, CString> > GetRoomCamera( int nRoomID );
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArrangeDocument)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CArrangeDocument();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CArrangeDocument)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class LocationHeaderText : public IHeaderText
{
protected:
	CString		m_sCinema;
	int			m_nRoomNo;

	CString Get()
	{
		CString sFmtHeader;
		sFmtHeader.Format( IDS_FMT_LOCATION, (LPCTSTR)m_sCinema, m_nRoomNo  );
		return sFmtHeader;
	}
public:
	CString	GetCinema() const	{ return m_sCinema; }
	int		GetRoom() const		{ return m_nRoomNo; }
	LocationHeaderText( CString sCinema, int nRoom ): m_sCinema(sCinema), m_nRoomNo(nRoom){}
	LocationHeaderText(const  LocationHeaderText& sh) : 
		m_sCinema( sh.m_sCinema ),
		m_nRoomNo( sh.m_nRoomNo ){}
};

class SimpleHeaderText : public LocationHeaderText
{

protected:
	CString		GetLocalizedDateTime( CTime time, bool bSeconds )
	{
		CString sFmtHeader;
		CString sDateStat = CArrangeDocument::GetLocalizedDate( time );
		TCHAR	szBuf[255];
		SYSTEMTIME st;
		time.GetAsSystemTime(st);
		int res = 
				bSeconds? 
				GetTimeFormat( LOCALE_USER_DEFAULT,  TIME_FORCE24HOURFORMAT, &st, _T("HH:mm:ss"), szBuf, 255) :
				GetTimeFormat( LOCALE_USER_DEFAULT,  TIME_NOSECONDS|TIME_FORCE24HOURFORMAT, &st, _T("HH:mm"), szBuf, 255);
		ASSERT( res );
		sFmtHeader.Format( _T("%s %s"), (LPCTSTR)sDateStat, (LPCTSTR)szBuf );
		return sFmtHeader;
	}
	CString Get( CTime time, bool bSeconds)
	{
		CString sFmtHeader;
		CString sDateStat = GetLocalizedDateTime( time, bSeconds ) ;
		sFmtHeader.Format( IDS_FMT_VIDEO_HEADER, (LPCTSTR)m_sCinema, m_nRoomNo, (LPCTSTR)sDateStat  );
		return sFmtHeader;
	}
public:
	virtual CString Get( )
	{
		return Get( CTime::GetCurrentTime(), true );
	}

	SimpleHeaderText( CString sCinema, int nRoom ): LocationHeaderText(sCinema, nRoom){}
};
