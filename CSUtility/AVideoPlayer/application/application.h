/********************************************************************************
  application.h
  ---------------------
  begin     : May 2003
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  e-mail    : aaah@mail.ru, aaahaaah@hotmail.com
********************************************************************************/

#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

//=================================================================================================
/** \class CTrackApp.
    \brief CTrackApp. */
//=================================================================================================
class CTrackApp : public CWinApp
{
public:
  CTrackApp();
  virtual BOOL InitInstance();

public:
  afx_msg void OnAppAbout();
  DECLARE_MESSAGE_MAP()
  virtual BOOL OnIdle(LONG lCount);
};

extern CTrackApp theApp;


