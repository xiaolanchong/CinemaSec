/****************************************************************************
  CSAlgoCommonDll.h
  ---------------------
  begin     : Aug 2004
  modified  : 24 Aug 2005
  author(s) : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

//=================================================================================================
/** \class CCSAlgoCommonDll.
    \brief CCSAlgoCommonDll. */
//=================================================================================================
class CCSAlgoCommonDll : public CWinApp
{
public:
  CCSAlgoCommonDll();
  ~CCSAlgoCommonDll();
  virtual BOOL InitInstance();
  DECLARE_MESSAGE_MAP()
};

extern CCSAlgoCommonDll theDll;

