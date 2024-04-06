// LayoutDialog.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "LayoutDialog.h"
#include ".\layoutdialog.h"

const UINT Timer_Update		= 10;
const UINT Period_Update	= 200;

// CLayoutDialog dialog

IMPLEMENT_DYNAMIC(CLayoutDialog, CLayerHelperSpinDialog)
CLayoutDialog::CLayoutDialog(	const  AvailableSourceArr_t& Arr, 
								bool bUpdate,
								CWnd* pParent /*=NULL*/)
	:	CLayerHelperSpinDialog(CLayoutDialog::IDD, true, pParent),
		m_Available(Arr),
		m_bUpdate(bUpdate)
{
	for( size_t i = 0; i < m_Available.size(); ++i )
	{
		m_NonAttached.insert( i );
	}
}

CLayoutDialog::~CLayoutDialog()
{
}


BEGIN_MESSAGE_MAP(CLayoutDialog, CLayerHelperSpinDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CLayoutDialog message handlers

void CLayoutDialog::OnInitDialogOnce()
{
	CLayerHelperSpinDialog::OnInitDialogOnce();

	int nMaxWidth = std::min<int> ( 10, m_Available.size() );
	int nMaxHeight = std::min<int> ( 10, m_Available.size() );
	m_spinWidth.SetRange32( 1, nMaxWidth );
	m_spinHeight.SetRange32( 1, nMaxHeight);

	size_t	nCount	= m_Available.size();
	WORD	nDim	= (WORD)ceil( sqrt( double(nCount) ) );

	m_wndLayout.SetSize( nDim, nDim );
	m_spinWidth.SetPos32( nDim );
	m_spinHeight.SetPos32( nDim );

	if( m_bUpdate ) SetTimer( Timer_Update, Period_Update, 0 );
}


void	CLayoutDialog::DrawItem( Gdiplus::Graphics& gr, CRect rc, DWORD_PTR dwCookie )
{
	int nIndex = m_Available[ dwCookie ].first;
	boost::shared_ptr<CBackground> m_Source = m_Available [ dwCookie ].second ;
	m_Source->Draw( gr, CRectF(rc) );

	CString s;
	// FIXME : resources
	s.Format( _T("Camera %d"), nIndex );
	DrawHeader( gr, rc, s );
}

bool	CLayoutDialog::GetItemMenuContent( std::vector<CString> &  MenuItems, WORD x, WORD y, bool& bCanDeleted   )
{
	bCanDeleted = true;
	UNREFERENCED_PARAMETER(x);
	UNREFERENCED_PARAMETER(y);
	MenuItems.clear();
	std::set< size_t >::const_iterator it = m_NonAttached.begin();
	for( ; it != m_NonAttached.end(); ++it )
	{
		size_t nOff = *it;
		int nId = m_Available[nOff].first;
		CString s;
		// FIXME : resources
		s.Format( _T("Camera %d"), nId );
		MenuItems.push_back(s);
	}
	return true;
}

std::pair<bool, DWORD_PTR>	CLayoutDialog::SelectMenuItem( size_t nIndex  )
{
	std::set< size_t >::iterator it = m_NonAttached.begin();
	std::advance( it, nIndex );
	size_t nOff = *it;
	m_NonAttached.erase( it );
	return std::make_pair( true, nOff );
}

void		CLayoutDialog::DeleteItem( DWORD_PTR dwCookie )
{
	m_NonAttached.insert( size_t(dwCookie) );
}

void CLayoutDialog::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if( Timer_Update )
	{
		m_wndLayout.Invalidate();
	}

	__super::OnTimer(nIDEvent);
}

void CLayoutDialog::OnOK()
{
	if( !m_NonAttached.empty() )
	{
		AfxMessageBox( IDS_ERROR_ITEM, MB_OK|MB_ICONERROR );
	}
	else
	{
		std::vector< CLayoutWnd::Coord > CellArr;
		m_wndLayout.GetResultLayout( CellArr );
		std::pair<WORD, WORD> p = m_wndLayout.GetSize();
		m_ResultLayout.SetArray( p.first, p.second );
		SparceRectImageLayout::IndexArray_t IndArr( m_Available.size(), SparceRectImageLayout::Cell() ) ;
		for( size_t i = 0; i < CellArr.size(); ++i )
		{
			size_t nIndex = CellArr[i].dwCookie;
			IndArr[ nIndex ].m_nX = CellArr[i].x;
			IndArr[ nIndex ].m_nY = CellArr[i].y;
		}
		m_ResultLayout.SetCells( IndArr );
		CLayerHelperSpinDialog::OnOK();
	}
}