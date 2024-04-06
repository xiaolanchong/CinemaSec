// HtmlDialog.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include <mshtmhst.h>
#include "HtmlDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int BSTRToLocal(LPTSTR pLocal, BSTR pWide, DWORD dwChars) 
{ 
	*pLocal = 0; 

	#ifdef UNICODE 
	lstrcpyn(pLocal, pWide, dwChars); 
	#else 
	WideCharToMultiByte(CP_ACP,0,pWide,-1,pLocal,dwChars,NULL,NULL); 
	#endif 

	return lstrlen(pLocal); 
} 
 
int LocalToBSTR(BSTR pWide, LPTSTR pLocal, DWORD dwChars) 
{ 
	*pWide = 0; 

	#ifdef UNICODE
	lstrcpyn(pWide, pLocal, dwChars);
	#else
	MultiByteToWideChar(CP_ACP,0,pLocal,-1,pWide,dwChars);
	#endif 

	return lstrlenW(pWide); 
} 

/////////////////////////////////////////////////////////////////////////////
// CHtmlDialog

CHtmlDialog::CHtmlDialog()
{
	#ifdef _DEBUG
	TRACE(_T("Warning: Empty Constructor Called - Proper Initialization not done!"));
	#endif // _DEBUG
}

CHtmlDialog::CHtmlDialog(UINT nResID, CWnd* pParent/*= NULL*/)
{
	m_hWndParent = pParent ? pParent->GetSafeHwnd() : NULL;

	CString strTemp;
	strTemp.Format(_T("%d"), nResID);
	
	ResourceToURL(strTemp);
}

CHtmlDialog::CHtmlDialog(LPCTSTR lpszURL, BOOL bRes, CWnd* pParent/*= NULL*/)
{
	m_hWndParent = pParent ?  pParent->GetSafeHwnd() : NULL;

	if (bRes)
	{
		ResourceToURL(lpszURL);
	}
	else
		m_strURL = lpszURL;
}

void CHtmlDialog::CommonConstruct()
{
}

CHtmlDialog::~CHtmlDialog()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlDialog diagnostics

#ifdef _DEBUG
void CHtmlDialog::AssertValid() const
{

}

void CHtmlDialog::Dump(CDumpContext& dc) const
{
	UNUSED_ALWAYS(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CHtmlDialog message handlers

int CHtmlDialog::DoModal()
{
	HINSTANCE hinstMSHTML = LoadLibrary(TEXT("MSHTML.DLL")); 

	if(hinstMSHTML) 
	{ 
		SHOWHTMLDIALOGFN  *pfnShowHTMLDialog; 

		pfnShowHTMLDialog = (SHOWHTMLDIALOGFN*)GetProcAddress(hinstMSHTML, ("ShowHTMLDialog")); 

		if(pfnShowHTMLDialog) 
		{ 
			//Now create a URL Moniker
			IMoniker* pmk = NULL;
			BSTR bstrURL = m_strURL.AllocSysString();
			CreateURLMoniker(NULL, bstrURL, &pmk);

			if(pmk) 
			{ 
				HRESULT  hr; 

				WCHAR* pchOptions = (WCHAR*)(m_strOptions.IsEmpty() ? NULL : m_strOptions.GetBuffer(0));

				hr = (*pfnShowHTMLDialog)(NULL, pmk, &m_varArgs, pchOptions, &m_varReturn); 

				pmk->Release(); 

				if(FAILED(hr)) 
				{
					FreeLibrary(hinstMSHTML);
					return -1;
				}
			} 
		} 
		FreeLibrary(hinstMSHTML);
	} 

	return 0; 

}

inline void CHtmlDialog::ResourceToURL(LPCTSTR lpszURL)
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);
	
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];
	
	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		m_strURL.Format(_T("res://%s/%s"), lpszModule, lpszURL);
	}
	
	delete []lpszModule;
}

void CHtmlDialog::SetParam(VARIANT *pvarArgs)
{
	m_varArgs = pvarArgs;
}

void CHtmlDialog::SetParam(LPCTSTR lpszArgs)
{
	m_varArgs = lpszArgs;
}

CString CHtmlDialog::GetReturnString()
{
	ASSERT(m_varReturn.vt == VT_BSTR);
	CString str = m_varReturn.bstrVal;
	return str;
}

LPCVARIANT CHtmlDialog::GetReturnVariant()
{
	return (LPCVARIANT)m_varReturn;
}

void CHtmlDialog::SetSize(int x, int y)
{
	m_strOptions.Format(_T("dialogWidth: %d; dialogHeight: %d"), x, y);
}

void CHtmlDialog::SetDlgOptions(LPCTSTR lpszOptions)
{
	m_strOptions = lpszOptions;
}
