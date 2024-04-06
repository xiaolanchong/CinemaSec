#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"  // main symbols

class CCSAlgoDll : public CWinApp
{
public:
  std::wstring m_dllName;
  CCSAlgoDll();
  virtual BOOL InitInstance();
  DECLARE_MESSAGE_MAP()
};

extern CCSAlgoDll theDll;

