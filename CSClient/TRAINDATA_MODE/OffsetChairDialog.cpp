// OffsetChairDialog.cpp : implementation file
//

#include "stdafx.h"
#include "csclient.h"
#include "OffsetChairDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COffsetChairDialog dialog


COffsetChairDialog::COffsetChairDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COffsetChairDialog::IDD, pParent), m_pCallback(0)
{
	//{{AFX_DATA_INIT(COffsetChairDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void COffsetChairDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COffsetChairDialog)
	DDX_Control(pDX, IDC_EDIT_DOWN, m_edDown);
	DDX_Control(pDX, IDC_EDIT_LEFT, m_edLeft);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COffsetChairDialog, CDialog)
	//{{AFX_MSG_MAP(COffsetChairDialog)
	ON_BN_CLICKED(IDC_MOVE, OnMove)
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COffsetChairDialog message handlers

void COffsetChairDialog::OnMove() 
{
	// TODO: Add your control notification handler code here
	if( m_pCallback ) m_pCallback->Move( int(m_edLeft.GetValue()), int( m_edDown.GetValue() ) );
}

BOOL COffsetChairDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_edLeft.SetFractionalDigitCount(0);
	m_edDown.SetFractionalDigitCount(0);
	return TRUE;
}