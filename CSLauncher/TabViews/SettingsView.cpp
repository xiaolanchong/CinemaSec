// SettingsView.cpp : implementation file
//

#include "stdafx.h"
#include "../CSLauncher.h"
#include "SettingsView.h"


// CSettingsView

IMPLEMENT_DYNCREATE(CSettingsView, CFormView)

CSettingsView::CSettingsView()
	: CFormView(CSettingsView::IDD)
{
}

CSettingsView::~CSettingsView()
{
}

void CSettingsView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_LINE, m_spinLineNumber);
	DDX_Control(pDX, IDC_RICHEDIT_CS, m_edConnectionString);
#if 0
	DDX_Control(pDX, IDC_CHECK_DEBUG, m_chkDebug);
	DDX_Control(pDX, IDC_CHECK_INFO, m_chkInfo);
	DDX_Control(pDX, IDC_CHECK_WARNING, m_chkWarning);
	DDX_Control(pDX, IDC_CHECK_ERROR, m_chkError);
	DDX_Control(pDX, IDC_CHECK_UNKNOWN, m_chkUnknown);
#endif
}

BEGIN_MESSAGE_MAP(CSettingsView, CFormView)
END_MESSAGE_MAP()


// CSettingsView diagnostics

#ifdef _DEBUG
void CSettingsView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSettingsView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

// CSettingsView message handlers


void CSettingsView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
#if 0
	CheckDlgButton( IDC_CHECK_DEBUG,	BST_CHECKED );
	CheckDlgButton( IDC_CHECK_INFO,		BST_CHECKED );
	CheckDlgButton( IDC_CHECK_WARNING,	BST_CHECKED );
	CheckDlgButton( IDC_CHECK_ERROR,	BST_CHECKED );
	CheckDlgButton( IDC_CHECK_UNKNOWN,	BST_CHECKED );
#endif
	m_spinLineNumber.SetRange( 10, 5000 );
	m_spinLineNumber.SetPos( 100 );
}

void		CSettingsView::SetConnectionString ( LPCWSTR szCS )
{
	m_edConnectionString.SetTargetDevice(NULL, 0);
	SETTEXTEX st = { ST_DEFAULT, 1200 };
	m_edConnectionString.SendMessage( EM_SETTEXTEX, (WPARAM)&st, (LPARAM) szCS );

}

int		CSettingsView::GetMaxLineNumber()
{
	return m_spinLineNumber.GetPos(  );
}