// LayoutIDDialog.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "LayoutIDDialog.h"


// CLayoutIDDialog dialog

IMPLEMENT_DYNAMIC(CLayoutIDDialog, CLayerHelperSpinDialog)
CLayoutIDDialog::CLayoutIDDialog(CWnd* pParent /*=NULL*/)
	: CLayerHelperSpinDialog(CLayoutIDDialog::IDD, true, pParent),
		m_dwCount(1)
{
}

CLayoutIDDialog::~CLayoutIDDialog()
{
}

void CLayoutIDDialog::DoDataExchange(CDataExchange* pDX)
{
	CLayerHelperSpinDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLayoutIDDialog, CLayerHelperSpinDialog)
END_MESSAGE_MAP()


// CLayoutIDDialog message handlers

void	CLayoutIDDialog::DrawItem( Gdiplus::Graphics& gr, CRect rc, DWORD_PTR dwCookie )	
{
	CString s;
	s.Format( _T("%Id"), dwCookie );
	DrawHeader(gr, rc, s );
}

bool	CLayoutIDDialog::GetItemMenuContent( std::vector<CString> &  MenuItems, WORD x, WORD y, bool& bCanDeleted    )
{
	bCanDeleted = true;
	MenuItems.push_back( CString(_T("New id")) );
	return true;
}

std::pair<bool, DWORD_PTR>	CLayoutIDDialog::SelectMenuItem( size_t nIndex  )
{
	return std::make_pair( true, m_dwCount++ );
}

void	CLayoutIDDialog::DeleteItem( DWORD_PTR dwCookie )
{

}

void CLayoutIDDialog::OnOK()
{
	m_wndLayout.GetResultLayout( m_Coord );

	CLayerHelperSpinDialog::OnOK();
}

void CLayoutIDDialog::OnInitDialogOnce()
{
	CLayerHelperSpinDialog::OnInitDialogOnce();

	m_spinWidth.SetRange32( 1, 10 );
	m_spinHeight.SetRange32( 1, 10);

	const int nDim = 1;

	m_wndLayout.SetSize( nDim, nDim );
	m_spinWidth.SetPos32( nDim );
	m_spinHeight.SetPos32( nDim );
}