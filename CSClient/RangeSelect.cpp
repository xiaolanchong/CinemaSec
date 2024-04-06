// RangeSelect.cpp : implementation file
//

#include "stdafx.h"
#include "CSClient.h"
#include "RangeSelect.h"


// CRangeSelect

IMPLEMENT_DYNAMIC(CRangeSelect, CStatic)
CRangeSelect::CRangeSelect()
{
}

CRangeSelect::~CRangeSelect()
{
}


BEGIN_MESSAGE_MAP(CRangeSelect, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CRangeSelect message handlers

void	CRangeSelect::SetMinMax( CTime timeMin, CTime timeMax )
{
	ASSERT( timeMin < timeMax );
	m_timeMax = timeMax;
	m_timeMin = timeMin;
	m_timeStart = m_timeEnd = m_timeMin;
	Invalidate();
}

void	CRangeSelect::SetStart( CTime timeStart )
{
	m_timeStart = m_timeEnd = timeStart;
	Invalidate();
}

void	CRangeSelect::SetCurrent( CTime timeCur )
{
	m_timeEnd = timeCur;
	Invalidate();
}

void	CRangeSelect::Reset()
{
	m_timeStart = m_timeEnd = m_timeMin = m_timeMax;
	Invalidate();
}

void	CRangeSelect::SetEnd( CTime timeEnd )
{
	m_timeEnd = timeEnd;
	Invalidate();
}

std::pair<CTime, CTime> CRangeSelect::GetRange() const
{
	CTime timeStart = m_timeStart > m_timeEnd ? m_timeEnd	: m_timeStart;
	CTime timeEnd	= m_timeStart > m_timeEnd ? m_timeStart : m_timeEnd;
	return std::make_pair( timeStart, timeEnd );
}

void	CRangeSelect::OnPaint()
{
	CPaintDC dc(this);
	CRect rc;
	GetClientRect( &rc ); rc.InflateRect(0, 0, 1 , 1);
//	rc.DeflateRect(1, 1);
	CPen	penFrame( PS_SOLID, 1, GetSysColor( COLOR_BTNSHADOW ) );
	CBrush	brFrame( GetSysColor( COLOR_BTNFACE ) );

	CGdiObject* pOldBrush	= dc.SelectObject( &brFrame );
	CGdiObject* pOldPen		= dc.SelectObject( &penFrame );
#if 1
	dc.SelectStockObject( NULL_PEN );
	dc.Rectangle( &rc );
	dc.SelectObject( &penFrame );
#endif
	if( m_timeStart != m_timeEnd && m_timeMin != m_timeMax)
	{
		CTime timeStart = m_timeStart > m_timeEnd ? m_timeEnd	: m_timeStart;
		CTime timeEnd	= m_timeStart > m_timeEnd ? m_timeStart : m_timeEnd;

		CTimeSpan	tpWhole		= m_timeMax - m_timeMin;
		CTimeSpan	tpSelect	= timeEnd	- m_timeMin;
		CTimeSpan	tpOffset	= timeStart - m_timeMin;	

		rc.DeflateRect( 1, 2 );
		LONG nSec = tpWhole.GetTotalSeconds();
		CRect rcRange(	rc.left + rc.Width() * tpOffset.GetTotalSeconds()/nSec, rc.top,
						rc.left + rc.Width() * tpSelect.GetTotalSeconds()/nSec, rc.bottom);
		dc.FillSolidRect( &rcRange, GetSysColor( COLOR_BTNHIGHLIGHT ) );
	}

	dc.SelectObject( pOldBrush );
	dc.SelectObject( pOldPen );
}