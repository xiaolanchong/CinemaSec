// CSClient.h : main header file for the CSCLIENT application
//

#if !defined(AFX_CSCLIENT_H__C10D023E_0693_4EBA_9A06_7DB0209D9157__INCLUDED_)
#define AFX_CSCLIENT_H__C10D023E_0693_4EBA_9A06_7DB0209D9157__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER <= 1200
#pragma warning( disable : 4097 )
#endif

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "res/resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCSClientApp:
// See CSClient.cpp for the implementation of this class
//

class CCSClientApp : public CWinApp
{
	class GdiPlusInit
	{
		ULONG_PTR					gdiplusToken;
	public:
		GdiPlusInit()
		{
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			// Initialize GDI+.
			Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		}
		~GdiPlusInit()
		{
			Gdiplus::GdiplusShutdown(gdiplusToken);
		}
	}	m_GdiInit;
#if 0
	struct ComInitializer
	{
		HRESULT m_hr;
		ComInitializer() { m_hr = CoInitializeEx(0, COINIT_MULTITHREADED);}
		~ComInitializer(){ if( m_hr == S_OK) CoUninitialize();}
	} m_ComInit;
#endif	
public:
	CCSClientApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCSClientApp)
	public:
	virtual BOOL InitInstance();
	BOOL OnIdle( LONG lCount );
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CCSClientApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSCLIENT_H__C10D023E_0693_4EBA_9A06_7DB0209D9157__INCLUDED_)
