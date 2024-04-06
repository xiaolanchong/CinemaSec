#include "stdafx.h"
#include "resource.h"
#include "about_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}


void CAboutDlg::DoDataExchange( CDataExchange * pDX )
{
  CDialog::DoDataExchange( pDX );
}

