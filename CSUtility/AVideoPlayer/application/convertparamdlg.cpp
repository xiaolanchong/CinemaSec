#include "stdafx.h"
#include "view.h"

IMPLEMENT_DYNAMIC(CAppParamDlg, CDialog)

BEGIN_MESSAGE_MAP(CAppParamDlg, CDialog)
  ON_BN_CLICKED(IDC_BUTTON_DestinationFile, OnBnClickedButtonDestinationFile)
END_MESSAGE_MAP()


CAppParamDlg::CAppParamDlg( CWnd * pParent ) : CDialog( CAppParamDlg::IDD, pParent )
{
  m_fps = 25.0f;
  m_destinationFileName.Empty();
}


CAppParamDlg::~CAppParamDlg()
{
}


void CAppParamDlg::DoDataExchange( CDataExchange * pDX )
{
  CDialog::DoDataExchange( pDX );
  DDX_Text( pDX, IDC_EDIT_FPS, m_fps );
  DDV_MinMaxFloat( pDX, m_fps, 1.0f, 50.0f );
  DDX_Text( pDX, IDC_EDIT_DestinationFile, m_destinationFileName );
}


void CAppParamDlg::OnBnClickedButtonDestinationFile()
{
  CFileDialog dlg( TRUE, _T("avd"), 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
                   _T("Video Files (*.avd)|*.avd||"), this );
  if (dlg.DoModal() == IDOK)
    SetDlgItemText( IDC_EDIT_DestinationFile, (LPCTSTR)(dlg.GetPathName()) );
}

