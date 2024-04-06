// Layout\RenumberDialog.cpp : implementation file
//

#pragma warning( disable :  4310 )	// truncate constant
#pragma warning( disable :  4244 )	// conversion
#pragma warning( disable :  4267 )	// conversion to DWORD while var has DWORD_PTR type
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "Layout\RenumberDialog.h"


// CRenumberDialog dialog

IMPLEMENT_DYNAMIC(CRenumberDialog, CLayerHelperDialog)

CRenumberDialog::CRenumberDialog(
								 const SourceArr_t&				SrcArr,
								 const std::vector<int>&		IdArr,
								 const SparceRectImageLayout&	Layout,
								 MyChair::ChairSet&				ChSet,
								 CWnd* pParent /*=NULL*/)
 : CLayerHelperDialog(CRenumberDialog::IDD, true, pParent),
	m_ChSet(ChSet),
	m_IdArr(IdArr),
	m_SrcArr(SrcArr),
	m_Layout(Layout)
{
	ASSERT( IdArr.size() == SrcArr.size() );
	ASSERT( IdArr.size() == Layout.GetCells().size() );
}

CRenumberDialog::~CRenumberDialog()
{
}

void CRenumberDialog::DoDataExchange(CDataExchange* pDX)
{
	CLayerHelperDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRenumberDialog, CDialog)
END_MESSAGE_MAP()

std::set<int>		CRenumberDialog::GetNonAttached()
{
	std::set<int> z( m_AllChairId ) ;
	for( size_t i =0; i < m_AttachedId.size() ; ++i )
	{
		if( m_AttachedId[i] != -1 )
		z.erase( m_AttachedId[i] );
	}
	return z;
}

void CRenumberDialog::OnInitDialogOnce()
{
	CLayerHelperDialog::OnInitDialogOnce();

	std::vector< CLayoutWnd::Coord > c;
	const SparceRectImageLayout::IndexArray_t& IndArr = m_Layout.GetCells();
	for( size_t i =0; i < IndArr.size() ; ++i )
	{
		c.push_back( CLayoutWnd::Coord( (WORD)IndArr[i].m_nX, (WORD)IndArr[i].m_nY, i ) );
	}
	m_wndLayout.SetResultLayout( c );

	m_AttachedId.resize( m_SrcArr.size(), -1 );
	MatchId();

	CRect rc;
	GetClientRect(&rc);
	OnSize( SIZE_RESTORED, rc.Width(), rc.Height() ); 
}

// CRenumberDialog message handlers
const COLORREF ANIMATED_CURVE_COLOR = RGB(0,128,255);

void		CRenumberDialog::DrawItem( Gdiplus::Graphics& gr, CRect rc, DWORD_PTR dwCookie )
{
	size_t nIndex = dwCookie;

	CDC* pDC = GetDC() ;
	CDC	 dc;
	CBitmap bmp;
	dc.CreateCompatibleDC( pDC );
	bmp.CreateCompatibleBitmap( pDC, rc.Width(), rc.Height() );
	ReleaseDC(pDC);
	CGdiObject* pOldBmp = dc.SelectObject( &bmp );

	CRect rcStub( CPoint(0,0), rc.Size() );
	MyDrawGrayImage( *m_SrcArr[nIndex], dc.GetSafeHdc(), rcStub , INVERT_AXIS_Y );
	Gdiplus::Bitmap* pOthBmp = Gdiplus::Bitmap::FromHBITMAP( (HBITMAP) bmp.GetSafeHandle(), 0 );
	gr.DrawImage( pOthBmp, CRectF(rc) );
	delete pOthBmp;

	dc.SelectObject( pOldBmp );

//	if( m_AttachedId[ nIndex ] != -1);
	int nCameraNo = m_AttachedId[ nIndex ];

	Gdiplus::Pen penWhite( (Gdiplus::ARGB)Gdiplus::Color::White );
	Gdiplus::Color clAnim ( GetRValue( ANIMATED_CURVE_COLOR ), GetGValue( ANIMATED_CURVE_COLOR ), GetBValue( ANIMATED_CURVE_COLOR ) );
	Gdiplus::Pen penOver( clAnim, 2);
	penOver.SetDashStyle( Gdiplus::DashStyleDash );
/*
	CPen penOverCh( PS_DASH, 1, ANIMATED_CURVE_COLOR);
	CPen penSelectCh( PS_DASH, 1, SELECTED_ANIMATED_CURVE_COLOR);
	CPen penWhiteCh( PS_SOLID, 1, RGB(255,255,255) );
*/
	MyChair::ChairSet::const_iterator crvIt ;
	int nOff = 0;
	int m_nAnimation = 0;
	for (crvIt = m_ChSet.begin(); crvIt != m_ChSet.end(); ++crvIt, ++nOff)
	{
		// FIXME only for zone 0
		if(crvIt->subFrameNo != nCameraNo ) continue;
		const Vec2fArr & curve = (crvIt)->curve;
		//int  k; 
		int nPoint = (int)(curve.size()); 
		for( int k=0; k < nPoint; ++k /*, ++iter*/ )
		{
			int newk  = (m_nAnimation + k)%nPoint;
			int nextk = (newk + 1)%nPoint;

			const Vec2f& v1 = curve[newk];
			const Vec2f& v2 = curve[nextk];
			ASSERT( m_SrcArr[ nIndex ] );
			PointF pt1 (	rc.left + double(rc.Width() * v1.x)/ m_SrcArr[ nIndex ]->width() ,
							rc.top + double(rc.Height() * v1.y)/ m_SrcArr[ nIndex ]->height()  );
			PointF pt2 (	rc.left + double(rc.Width() * v2.x)/ m_SrcArr[ nIndex ]->width() ,
							rc.top + double(rc.Height() * v2.y)/ m_SrcArr[ nIndex ]->height()  );
			bool bOver = ( ( k % 8)  < 4 );
			gr.DrawLine(  bOver ? &penOver : &penWhite, pt1, pt2  );
		}
	}
}

bool		CRenumberDialog::GetItemMenuContent( std::vector<CString> &  MenuItems, WORD x, WORD y, bool& bCanDeleted    )
{
	bCanDeleted = false;
	MenuItems.push_back( CString(_T("No chairs")) );
	std::set<int>	NonAttachedId = GetNonAttached();
	std::set<int>::const_iterator it = NonAttachedId.begin();
	for(; it != NonAttachedId.end(); ++it )
	{
		CString s;
		s.Format( _T("Id = %d"), int(*it) );
		MenuItems.push_back( s );
	}

	m_nSelectedIndex = size_t(~0);
	const SparceRectImageLayout::IndexArray_t& IndArr = m_Layout.GetCells();
	for( size_t i =0; i <  IndArr.size(); ++i )
	{
		if( IndArr[i].m_nX == x  &&
			IndArr[i].m_nY == y )
		{
			m_nSelectedIndex = i;
			break;
		}
	}
	ASSERT( m_nSelectedIndex != ~0);

	return true;
}


std::pair<bool, DWORD_PTR>	CRenumberDialog::SelectMenuItem( size_t nIndex  )
{
	if( nIndex == 0 )
	{
		m_AttachedId[ m_nSelectedIndex ] = -1;
//		return std::make_pair( false, 0 );
	}
	else
	{
		std::set<int>	NonAttachedId = GetNonAttached();
		std::set<int>::iterator it = NonAttachedId.begin();
		std::advance ( it, nIndex - 1 );
		ASSERT( it != NonAttachedId.end() );
		int nCameraID = *it;
		m_AttachedId[ m_nSelectedIndex ] = nCameraID;
		NonAttachedId.erase( it );
	}
	return std::make_pair( true, m_nSelectedIndex );
}

void	CRenumberDialog::MatchId()
{
	MyChair::ChairSet::const_iterator it = m_ChSet.begin();
	for( ; it != m_ChSet.end(); ++it )
	{
		m_AllChairId.insert( it->subFrameNo );
//		m_NonAttachedId.insert( it->subFrameNo );
	}

	for( size_t i = 0; i < m_IdArr.size(); ++i )
	{
		if( m_AllChairId.find( m_IdArr[i] ) != m_AllChairId.end() )
		{
			m_AttachedId[i] = m_IdArr[i];
//			m_NonAttachedId.erase( m_IdArr[i] );
		}
	}
}

void	CRenumberDialog::RenumberAndErase()
{
	std::map<int, int>	Old2NewId;
	ASSERT( m_AttachedId.size() == m_IdArr.size() );
	for( size_t i = 0; i < m_AttachedId.size(); ++i)
	{
		if( m_AttachedId[i] != -1 )
		{
			Old2NewId.insert( std::make_pair( m_AttachedId[i], m_IdArr[i] ) );
		}
	}
	MyChair::ChairSet::iterator crvIt ;
	for (crvIt = m_ChSet.begin(); crvIt != m_ChSet.end(); )
	{
		std::map<int, int>::const_iterator it = Old2NewId.find( crvIt->subFrameNo );
		if( it == Old2NewId.end() )
		{
			m_ChSet.erase( crvIt++ );
		}
		else
		{
			crvIt->subFrameNo = it->second;
			++crvIt;
		}
	}
}

void CRenumberDialog::OnOK()
{
	RenumberAndErase();

	CLayerHelperDialog::OnOK();
}