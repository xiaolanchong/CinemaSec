#pragma once

#include "..\resource.h"

//=================================================================================================
/** \struct COutputWriteDlg.
    \brief  COutputWriteDlg. */
//=================================================================================================
class COutputWriteDlg : public CDialog
{
  DECLARE_DYNAMIC(COutputWriteDlg)

public:
  COutputWriteDlg( CWnd * pParent = NULL );
  virtual ~COutputWriteDlg();

  enum { IDD = IDD_OUTPUT_WRITE_DLG };
  CString m_fname;
  float   m_fps;
  int     m_headFrame;
  int     m_lastFrame;

protected:
  virtual void DoDataExchange( CDataExchange * pDX );

protected:
  DECLARE_MESSAGE_MAP()
  afx_msg void OnBnClickedButtonDestinationFile();
};

