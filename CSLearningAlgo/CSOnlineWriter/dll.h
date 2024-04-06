/****************************************************************************
  dll.h
  ---------------------
  begin     : 30 Aug 2005
  modified  : 30 Aug 2005
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

#include "resource.h"

class CCSOnlineWriterDll : public CWinApp
{
public:
  std::wstring m_dllName;         //!< full-path name of this module
  CCSOnlineWriterDll();
  virtual BOOL InitInstance();
  DECLARE_MESSAGE_MAP()
};

extern CCSOnlineWriterDll theDll;

