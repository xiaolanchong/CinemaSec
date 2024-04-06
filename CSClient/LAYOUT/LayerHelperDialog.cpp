// LayerHelperDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CSClient.h"
#include "LayerHelperDialog.h"


// CLayerHelperDialog dialog

IMPLEMENT_DYNAMIC(CLayerHelperDialog, CDialog)
CLayerHelperDialog::CLayerHelperDialog(UINT nID, bool bAutoUpdate, CWnd* pParent /*=NULL*/)
	:	CDialog(nID, pParent), 
		m_bInit(false),
		m_wndLayout( this, bAutoUpdate)
{
}

CLayerHelperDialog::~CLayerHelperDialog()
{
}

void CLayerHelperDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLayerHelperDialog, CDialog)
END_MESSAGE_MAP()



// CLayerHelperDialog message handlers

BOOL CLayerHelperDialog::OnInitDialog()
{
	if( !CDialog::OnInitDialog() ) return FALSE;

	if( !m_bInit )
	{
		m_bInit = true;
		OnInitDialogOnce();
		
	}
	return TRUE;
}

void CLayerHelperDialog::OnInitDialogOnce()
{
	CWnd *pWnd = GetDlgItem( IDC_LAYOUT );
	ASSERT(pWnd);
	CRect rc;
	pWnd->GetWindowRect(&rc);
	ScreenToClient( &rc );
	pWnd->DestroyWindow();
	m_wndLayout.Create( 0, 0, WS_BORDER|WS_CHILD|WS_VISIBLE, rc, this, IDC_LAYOUT );

	m_LayoutManager.Attach( this );
	m_LayoutManager.AddAllChildren();

	m_LayoutManager.SetConstraint(IDC_LAYOUT, OX_LMS_RIGHT, OX_LMT_SAME, -2);
	m_LayoutManager.SetConstraint(IDC_LAYOUT, OX_LMS_BOTTOM, OX_LMT_SAME, -2);
}

void CLayerHelperDialog::DrawHeader( Gdiplus::Graphics& gr, CRectF rc,CString sText)
{
	Gdiplus::FontFamily  fontFamily(L"Verdana");
	Gdiplus::Font        font(&fontFamily, 10, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

	Gdiplus::StringFormat stringFormat;
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
	stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);;		

	SolidBrush brString( static_cast<DWORD>(Color::White) );
	gr.DrawString( sText, -1, &font, rc, &stringFormat, &brString);
}