#include "stdafx.h"
#include "output_write_dlg.h"

IMPLEMENT_DYNAMIC(COutputWriteDlg, CDialog)

BEGIN_MESSAGE_MAP(COutputWriteDlg, CDialog)
  ON_BN_CLICKED(IDC_BUTTON_DestinationFile, OnBnClickedButtonDestinationFile)
END_MESSAGE_MAP()


COutputWriteDlg::COutputWriteDlg( CWnd * pParent ) : CDialog( COutputWriteDlg::IDD, pParent )
{
  m_fname.Empty();
  m_fps = 1.0f;
  m_headFrame = 0;
  m_lastFrame = 0;
}


COutputWriteDlg::~COutputWriteDlg()
{
}


void COutputWriteDlg::DoDataExchange( CDataExchange * pDX )
{
  CDialog::DoDataExchange( pDX );
  DDX_Text( pDX, IDC_EDIT_FPS, m_fps );
  DDV_MinMaxFloat( pDX, m_fps, 1.0f, 50.0f );
  DDX_Text( pDX, IDC_EDIT_DestinationFile, m_fname );
  DDX_Text( pDX, IDC_EDIT_FirstFrame, m_headFrame );
  DDV_MinMaxInt( pDX, m_headFrame, 0, 1000000 );
  DDX_Text( pDX, IDC_EDIT_LastFrame, m_lastFrame );
  DDV_MinMaxInt( pDX, m_lastFrame, 0, 1000000 );
  if (m_headFrame > m_lastFrame)
    std::swap( m_headFrame, m_lastFrame );
}


void COutputWriteDlg::OnBnClickedButtonDestinationFile()
{
  CFileDialog dlg( TRUE, _T("avd"), 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
                   _T("Video Files (*.avd)|*.avd||"), this );
  if (dlg.DoModal() == IDOK)
    SetDlgItemText( IDC_EDIT_DestinationFile, (LPCTSTR)(dlg.GetPathName()) );
}

