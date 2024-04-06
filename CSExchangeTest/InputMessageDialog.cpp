// InputMessageDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CSExchangeTest.h"
#include "InputMessageDialog.h"
#include "IInputMessage.h"
#include ".\inputmessagedialog.h"
#include <atlenc.h>
#include "../CSChair/xml/xmllite.h"

#include "..\CSLauncher\CSLauncher_i.h"
//#include "..\CSLauncher\CSLauncher_i.c"

const UINT WM_INCOMING_MESSAGE = WM_USER + 0x43;

// CInputMessageDialog dialog

IMPLEMENT_DYNAMIC(CInputMessageDialog, CPropertyPage)
CInputMessageDialog::CInputMessageDialog()
	: CPropertyPage(CInputMessageDialog::IDD, IDS_INPUT_MESSAGE)
{
}

CInputMessageDialog::~CInputMessageDialog()
{
}

void CInputMessageDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MESSAGE, m_wndMessage);
}

BOOL CInputMessageDialog::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();
	if(!res) return FALSE;

	m_wndHtml.CreateFromStatic( IDC_STATIC_HTML, this );
	m_IncomingMessages.reserve( 100 );
	GetInputBuffer().RegisterWnd( GetSafeHwnd(), WM_INCOMING_MESSAGE );
	OnIncomingMessage(0, 0);
	//m_wndHtml.Navigate2( _T("F:\\Project\\VCproject\\CinemaSec\\CSExchangeTest\\_QueryTest\\db_hammer_query (1).xml") );
#if 0
	CComPtr<IBoxOfficeRequest> p;
	p.CoCreateInstance( CLSID_BoxOfficeRequest );
	HRESULT hr = p->Request( CComBSTR(L"<html><body>Blah blah</body></html>") );
	p.Release();
#endif
	return TRUE;
}

BEGIN_MESSAGE_MAP(CInputMessageDialog, CPropertyPage)
	ON_MESSAGE( WM_INCOMING_MESSAGE, OnIncomingMessage )
	ON_LBN_SELCHANGE(IDC_LIST_MESSAGE, OnLbnSelchangeListMessage)
END_MESSAGE_MAP()


// CInputMessageDialog message handlers

LRESULT CInputMessageDialog::OnIncomingMessage( WPARAM , LPARAM  )
{
	IInputBuffer::InputBuffer_t& MsgBuffer = GetInputBuffer().Lock();
	while( !MsgBuffer.empty() )
	{
		AddMessage( MsgBuffer.front() );
		std::pair<bool, int> p = ParseMessage( MsgBuffer.front() );
	
		if( p.first )
		{
			SendMessage( p.second );
		}
		MsgBuffer.pop();
	}
	GetInputBuffer().Unlock();
	return 0;
}

void	CInputMessageDialog::AddMessage( const std::wstring& sMsg )
{
	CTime time = CTime::GetCurrentTime();
	CString sTime = time.Format( _T("%H:%M:%S") );
	m_wndMessage.AddString( sTime );
	int nSize = AtlUnicodeToUTF8( sMsg.c_str(), sMsg.length(), 0, 0 );
	std::vector<BYTE> UtfMsg( nSize ) ;
	AtlUnicodeToUTF8( sMsg.c_str(), sMsg.length(), (char*)&UtfMsg[0], nSize );
	m_IncomingMessages.push_back( UtfMsg );
}

void CInputMessageDialog::OnLbnSelchangeListMessage()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_wndMessage.GetCurSel();
	if( nIndex == LB_ERR )
	{
		m_wndHtml.Navigate2( _T("about:blank") );
	}
	else
	{
		size_t uIndex = (size_t)nIndex;
		ASSERT( uIndex >= 0 && uIndex < m_IncomingMessages.size() );
		const std::vector<BYTE>& sMsg = m_IncomingMessages[uIndex];
		WCHAR szPath[MAX_PATH], szFullPath[MAX_PATH];
		GetTempPathW( MAX_PATH, szPath );
		PathCombineW(szFullPath, szPath, L"CSExchangeTest_tmp.xml" );
		CFile f( szFullPath, CFile::modeCreate| CFile::modeWrite );
		f.Write( &sMsg[0], sMsg.size() );
		f.Close();
		m_wndHtml.Navigate2( szFullPath );
	}
}

#define		ELEM_ROOT		_T("component")
#define		ELEM_TIME		_T("time")
#define		ELEM_ERRORDESC	_T("describe")
#define		ELEM_ERROR		_T("error")

#define		TAG_TYPE		_T("type")
#define		TAG_ID			_T("id")
#define		TAG_REFID		_T("ref_id")
#define		TAG_VERSION		_T("version")

std::pair<bool, int> CInputMessageDialog::ParseMessage( const std::wstring& sMsg )
try
{
	XmlLite::XMLDocument doc;
	doc.LoadFromString( sMsg.c_str() );

	XmlLite::XMLElement elRoot = doc.GetRoot();
	int nQueryID;
	elRoot.GetAttr( TAG_ID, nQueryID );
	std::tstring sType;
	elRoot.GetAttr( TAG_TYPE, sType );
	if( sType == _T("SEAT") )
	{
		return std::make_pair(true, nQueryID);	
	}
	else
		return std::make_pair(false, -1);
}
catch( XMLException  )
{
	return std::make_pair(false, -1);
};

void	CInputMessageDialog::SendMessage( int nID)
{
	XmlLite::XMLDocument doc;
	doc << XmlLite::XMLProcInstr( doc, _T("xml"), _T("version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"") );
	XmlLite::XMLElement elRoot( doc, ELEM_ROOT ), 
		elError( doc, ELEM_ERROR ),
		elDesc( doc, ELEM_ERRORDESC ) ;
	elRoot	<< XmlLite::XMLAttribute( elRoot, TAG_VERSION, _T("1.0")		)
		<< XmlLite::XMLAttribute( elRoot, TAG_TYPE,	_T("BOERROR")	)
		<< XmlLite::XMLAttribute( elRoot, TAG_REFID,	nID	);
	elError	<< XmlLite::XMLAttribute( elRoot, TAG_ID, 0 );
	elDesc	<< XmlLite::XMLText( elError, _T("No error")	);
	doc.SetRoot( elRoot );
	elRoot << ( elError << elDesc );
	std::wstring sText;
	doc.SaveToString( sText );
	
	CComPtr<ICCSRequest> pRequest;
	HRESULT hr = pRequest.CoCreateInstance( CLSID_CCSRequest );
	if( hr != S_OK )
	{
		//			AfxMessageBoxErr( _T("Failed to get ICCSRequest") );
		//			return;
	}
	CComBSTR sMsg( sText.c_str() );
	hr = pRequest->Request( sMsg );
}