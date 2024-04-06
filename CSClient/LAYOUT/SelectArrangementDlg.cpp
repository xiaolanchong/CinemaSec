// Layout\SelectArrangementDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CSClient.h"
#include "Layout\SelectArrangementDlg.h"
#include ".\selectarrangementdlg.h"


// CSelectArrangementDlg dialog

IMPLEMENT_DYNAMIC(CSelectArrangementDlg, CDialog)
CSelectArrangementDlg::CSelectArrangementDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectArrangementDlg::IDD, pParent)
{
}

CSelectArrangementDlg::~CSelectArrangementDlg()
{
}

void CSelectArrangementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSelectArrangementDlg, CDialog)
END_MESSAGE_MAP()


// CSelectArrangementDlg message handlers

static struct RadioLayout
{
	int		nID;
	int		x, y;
} 
s_PredefindedLayout[] = 
{
	{ IDC_RADIO1, 1, 1 },
	{ IDC_RADIO2, 2, 2 },
	{ IDC_RADIO3, 2, 3 }
};

void CSelectArrangementDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	
	//int MaxX = 0, MaxY = 0;
	for( size_t i = 0; i < sizeof(s_PredefindedLayout)/sizeof(RadioLayout); ++i )
	{
		if( IsDlgButtonChecked( s_PredefindedLayout[i].nID ) )
		{
			SparceRectImageLayout::IndexArray_t IndArr;
			const RadioLayout& rl = s_PredefindedLayout[i];
			for( int k = 0; k < rl.y; ++k  )
				for( int l = 0; l < rl.x; ++l  )
				{
					int nCameraID = rl.x * k + l + 1;
					m_Cameras.push_back( nCameraID );
					IndArr.push_back( SparceRectImageLayout::Cell( l, k ) );
				}
			m_Layout.SetArray( rl.x, rl.y );
			m_Layout.SetCells( IndArr );
			break;
		}
	}
	
	CDialog::OnOK();
}

BOOL CSelectArrangementDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CheckDlgButton( IDC_RADIO1, 1 );
	return TRUE;
}
