// LayerHelperSpinDialog.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "LayerHelperSpinDialog.h"


// CLayerHelperSpinDialog dialog

IMPLEMENT_DYNAMIC(CLayerHelperSpinDialog, CDialog)
CLayerHelperSpinDialog::CLayerHelperSpinDialog(UINT nID, bool bAutoUpdate, CWnd* pParent /*=NULL*/)
	: CLayerHelperDialog( nID, bAutoUpdate, pParent)
{
}

CLayerHelperSpinDialog::~CLayerHelperSpinDialog()
{
}

void CLayerHelperSpinDialog::DoDataExchange(CDataExchange* pDX)
{
	CLayerHelperDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_spinWidth);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_spinHeight);
}

BEGIN_MESSAGE_MAP(CLayerHelperSpinDialog, CLayerHelperDialog)
	ON_NOTIFY( UDN_DELTAPOS, IDC_SPIN_WIDTH, OnGridWidthChange )
	ON_NOTIFY( UDN_DELTAPOS, IDC_SPIN_HEIGHT, OnGridHeightChange )
END_MESSAGE_MAP()


// CLayerHelperSpinDialog message handlers

void CLayerHelperSpinDialog::OnInitDialogOnce()
{
	CLayerHelperDialog::OnInitDialogOnce();

	m_spinWidth.SetBuddy( GetDlgItem( IDC_EDIT_WIDTH ) );
}

void	CLayerHelperSpinDialog::OnGridWidthChange( NMHDR* pHdr, LRESULT * pResult )
{
	NMUPDOWN* pNewHdr = (NMUPDOWN*)pHdr;
	int w = pNewHdr->iPos + pNewHdr->iDelta;
	int nLower, nUpper;
	m_spinWidth.GetRange32( nLower, nUpper );
	int h = m_spinHeight.GetPos32();
	if( nLower <= w && w <= nUpper )
	{
		m_wndLayout.SetSize( (WORD)w, (WORD)h );
		*pResult = 0;
	}
	else
	{
		*pResult = 1;
	}
}

void	CLayerHelperSpinDialog::OnGridHeightChange( NMHDR* pHdr, LRESULT * pResult )
{
	NMUPDOWN* pNewHdr = (NMUPDOWN*)pHdr;
	int h = pNewHdr->iPos + pNewHdr->iDelta;
	int nLower, nUpper;
	m_spinWidth.GetRange32( nLower, nUpper );
	int w = m_spinWidth.GetPos32();
	if( nLower <= h && h <= nUpper )
	{
		m_wndLayout.SetSize( (WORD)w, (WORD)h );
		*pResult = 0;
	}
	else
	{
		*pResult = 1;
	}
}