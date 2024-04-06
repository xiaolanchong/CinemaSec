#pragma once

//=================================================================================================
// CAboutDlg dialog used for App About.
//=================================================================================================
class CAboutDlg : public CDialog
{
public:
  CAboutDlg();
  enum { IDD = IDD_ABOUTBOX };
  virtual void DoDataExchange( CDataExchange * pDX );
  DECLARE_MESSAGE_MAP()
};

