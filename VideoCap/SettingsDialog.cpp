// SettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "VideoCap.h"
#include "SettingsDialog.h"
#include ".\settingsdialog.h"


// CSettingsDialog dialog

IMPLEMENT_DYNAMIC(CSettingsDialog, CDialog)
CSettingsDialog::CSettingsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDialog::IDD, pParent)
{
}

CSettingsDialog::~CSettingsDialog()
{
}

void CSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_FPS, m_spinFPS);
	DDX_Control(pDX, IDC_COMBO_ENCODER, m_wndEncoder);
}


BEGIN_MESSAGE_MAP(CSettingsDialog, CDialog)
END_MESSAGE_MAP()


// CSettingsDialog message handlers

void CSettingsDialog::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	GetParam();
	CDialog::OnCancel();
}

void CSettingsDialog::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	GetParam();
	CDialog::OnOK();
}

BOOL CSettingsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_spinFPS.SetRange32( 1, 25 );
	m_spinFPS.SetPos32( GetDefaultFPS() );

	m_Encoders.push_back( L"Y800" );

	for( size_t i = 0; i < m_Encoders.size(); ++i )
	{
		m_wndEncoder.AddString( m_Encoders[i].c_str() );
	}
	m_wndEncoder.SetCurSel( 0 );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void	CSettingsDialog::GetParam()
{
	m_nFPS		= m_spinFPS.GetPos32();
	int nIndex = m_wndEncoder.GetCurSel();
	if( nIndex != CB_ERR )
		m_sEncoder	= m_Encoders[nIndex];
	else
		m_sEncoder.clear();
}