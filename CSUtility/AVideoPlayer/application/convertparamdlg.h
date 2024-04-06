#pragma once

#include "..\resource.h"

class CAppParamDlg : public CDialog
{
  DECLARE_DYNAMIC(CAppParamDlg)

public:
  CAppParamDlg( CWnd * pParent = NULL );
  virtual ~CAppParamDlg();

  enum { IDD = IDD_APP_PARAM_DLG };
  float   m_fps;
  CString m_destinationFileName;

protected:
  virtual void DoDataExchange( CDataExchange * pDX );

protected:
  DECLARE_MESSAGE_MAP()
  afx_msg void OnBnClickedButtonDestinationFile();
};

