// VideoView.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "ReportView.h"
#include "ReportDocument.h"
#include "../res/resource.h"
#include "../../CSChair/XML/XmlLite.h"
#include "../GUI/DHTML/HtmlResourceHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportView

IMPLEMENT_DYNCREATE(CReportView, ParentClass)

CReportView::CReportView() : m_bInit(false)
{
}

CReportView::~CReportView()
{
}

BEGIN_MESSAGE_MAP(CReportView, ParentClass)
	//{{AFX_MSG_MAP(CReportView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportView diagnostics

#ifdef _DEBUG
void CReportView::AssertValid() const
{
	ParentClass::AssertValid();
}

void CReportView::Dump(CDumpContext& dc) const
{
	ParentClass::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CReportView message handlers

CReportDocument* CReportView::GetDocument()
{
	return dynamic_cast<CReportDocument*>( m_pDocument );
}

int CReportView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (ParentClass::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CReportView::OnInitialUpdate() 
{
//#define CHECK_HTML
	ParentClass::OnInitialUpdate();
	if(!m_bInit)
	{
		Navigate2(L"about:blank");
		UseCustomContextMenu(FALSE
#ifdef CHECK_HTML
		FALSE
#else
	//	TRUE
#endif
			);
		SetRegisterAsDropTarget( FALSE );

		m_bInit = true;
#ifdef CHECK_HTML
#if 1

		TCHAR szInitWrap[] = _T(
			"<html> \
				<head> \
				<meta content=\"text/html; charset=utf-16\"></meta> \
				<style> \
			table { border : 0 ; text-align : center ; width : 100% ; height : 100% } \
			body { color : silver} \
				</style> \
				</head> \
				<body> \
					<table><tr><td> \
					<h2>Выберите параметры отчета и нажмите <font color=\"dimgray\">Создать</font> button</h2> \
					</td></tr></table> \
				</body> \
			</html> \
			"
			);
#else
		TCHAR szInitWrap[] = _T(
			"<html><body>Выберите</body></html>"
			);
#endif
		size_t nLen = lstrlen(szInitWrap);
		std::vector<char>	InitArrMB( 2*nLen + 1, 0 );
		LPSTR szCvt = &InitArrMB[0];
		int res = WideCharToMultiByte( CP_OEMCP, 0, szInitWrap, nLen, szCvt, 2*nLen + 1, 0, 0 );
		LPCSTR szInit = (LPCSTR)&InitArrMB[0];
		NavigateFromString( szInit );
#endif
	}
}



void	CReportView::CreateReport( XmlLite::XMLDocument& xmlDoc )
{
	HtmlResourceHelper hh;
	HRESULT hr;

	

	IStream* pXslStream = hh.LoadToStream( MAKEINTRESOURCE(ID_HTML_REPORT1_XSL), MAKEINTRESOURCE(RT_HTML) );

	CComPtr<IXMLDOMDocument>	pXslDoc;
	hr = pXslDoc.CoCreateInstance( CLSID_DOMDocument );
	ASSERT(hr == S_OK);
	VARIANT_BOOL bLoad;
	hr = pXslDoc->load( CComVariant(pXslStream), &bLoad );
	ASSERT(hr == S_OK && bLoad == VARIANT_TRUE );	
/*
	IStream* pStr;
	CreateStreamOnHGlobal( NULL, TRUE, &pStr  );
	CComVariant var( pStr );
	hr = xmlDoc.m_pDoc->transformNodeToObject( pXslDoc, var );
	STATSTG st;
	pStr->Stat( &st, 0  );
	IUnknown* pUnk = var.punkVal;
	pUnk->QueryInterface( IID_IStream, (void**)&pStr );
	ASSERT( pStr );
	NavigateFromStream( pStr );*/

	CComBSTR bstrXml;
	hr = xmlDoc.m_pDoc->transformNode( pXslDoc, &bstrXml );
	ASSERT(hr == S_OK);

	std::wstring sHtmlDoc = bstrXml;
	std::wstring sCSSDoc = hh.LoadTextResource( MAKEINTRESOURCE(ID_HTML_REPORT_CSS), MAKEINTRESOURCE(RT_HTML) );

#if 1
	bool bRes = hh.InjectCSSIntoHTML( sHtmlDoc, sCSSDoc );
	ASSERT(bRes);
#endif

	CStringW zzz = sHtmlDoc.c_str();
	zzz.Replace( L"UTF-16", L"UTF-8" );
	zzz.TrimRight( L"\r\n");
	sHtmlDoc = (LPCWSTR)zzz;
#if 1
	size_t nLen = sHtmlDoc.length();
	std::vector<char>	InitArrMB( 2*nLen + 1, 0 );
	LPSTR szCvt = &InitArrMB[0];
	int res = WideCharToMultiByte( CP_UTF8, 0, sHtmlDoc.c_str(), nLen, szCvt, 2*nLen + 1, 0, 0 );
//	std::fill( InitArrMB.begin() + res, InitArrMB.end(), '\0'  );
//	NavigateFromString(szCvt );
#else
	NavigateFromString( sHtmlDoc.c_str() );
#endif

	TCHAR szPathBuf[MAX_PATH], szBuf[MAX_PATH];
	GetTempPath(MAX_PATH, szPathBuf);
	PathCombine( szBuf, szPathBuf, _T("report_cache.html") );

	CFile file( szBuf, CFile::modeCreate|CFile::modeWrite ) ;
	file.Write( &InitArrMB[0], res );
	file.Close();
	Navigate2( szBuf );

	pXslDoc.Release();
	pXslStream->Release();
}

void	CReportView::PreviewReport()
{
	m_pBrowserApp->ExecWB( OLECMDID_PRINTPREVIEW, OLECMDEXECOPT_DODEFAULT , NULL, NULL );
}

void	CReportView::PrintReport()
{
	m_pBrowserApp->ExecWB( OLECMDID_PRINT, OLECMDEXECOPT_DODEFAULT , NULL, NULL );
}

void	CReportView::SaveReport()
{
	m_pBrowserApp->ExecWB( OLECMDID_SAVEAS, OLECMDEXECOPT_DONTPROMPTUSER , &CComVariant(L"report.html"), &CComVariant(L"report.html") );
}