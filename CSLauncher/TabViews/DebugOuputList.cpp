//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Description is not available
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 20.02.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "DebugOuputList.h"
//======================================================================================//
//                                 class DebugOuputList                                 //
//======================================================================================//
DebugOuputList::DebugOuputList( ):
	m_pCB(0), 
	m_Hook(this),
	m_pWnd(0)
{
}

DebugOuputList::~DebugOuputList()
{
}

void	DebugOuputList::Attach( UINT nListID, CWnd* pWnd, DebugOutputListCB* pCB)
{
	m_pCB	= pCB;
	m_pWnd	= pWnd;
	if( nListID )
		m_wndDebugMessage.SubclassDlgItem( nListID, pWnd );
	m_Hook.HookWindow ( pWnd );
	//FIXME: put to resources
	LVCOLUMN lvc;
	ZeroMemory( &lvc, sizeof(LVCOLUMN) );
	lvc.pszText = _T("Time");
	lvc.cx		= 100;
	lvc.mask	= LVCF_TEXT| LVCF_WIDTH;

	if( nListID )
		m_wndDebugMessage.SetColumn( 0, &lvc);
	else
		m_wndDebugMessage.InsertColumn( 0, &lvc );
	m_wndDebugMessage.InsertColumn( 1, _T("Message"), LVCFMT_LEFT, 400 );
	if( nListID )
		m_wndDebugMessage.ModifyExStyle( 0, TVOXS_ROWSEL );
}

void	DebugOuputList::PrintW( __int32 mt, LPCWSTR szMessage/*, LPCWSTR, LPCWSTR, LPCWSTR */)
{
	cs.Lock();
	m_MsgBuffer.push_back( std::make_pair( mt, std::wstring(szMessage)  )  );
	m_pWnd->PostMessageW( WM_DBGMESSAGE );
	cs.Unlock();
}

void	DebugOuputList::PrintA( __int32 mt, LPCSTR szMessage/*, LPCSTR, LPCSTR, LPCSTR */)
{
	USES_CONVERSION;
	LPCWSTR szNewMsg = A2CW ( szMessage );
	PrintW( mt, szNewMsg );
}

void DebugOuputList::OnDebugMessage( )
{
	cs.Lock();
	const int c_nDefCount = 100;
	int nCount = m_pCB ? m_pCB->GetMaxLineNumber() : c_nDefCount;

	if( !nCount)
	{
		m_wndDebugMessage.DeleteAllItems();
		m_MsgBuffer.clear();
		cs.Unlock();
	}
	if( m_wndDebugMessage.GetItemCount( ) > nCount )
	{
		for( UINT i = 0; i < m_wndDebugMessage.GetCount() - nCount; ++i )
		{
			HTREEITEM hti = m_wndDebugMessage.GetItemFromIndex( 0 );
			m_wndDebugMessage.DeleteItem( hti );
		}
	}

	MsgBuffer_t::iterator it = m_MsgBuffer.begin();
	for( ; it != m_MsgBuffer.end(); ++it )
	{
		unsigned int mt		= it->first;
		LPCWSTR szMessage	= it->second.c_str();

		COLORREF clMsg;
		CString s;
		switch(mt)
		{
		case IDebugOutput::mt_debug_info :	clMsg = RGB(192, 192, 192);  break;
		case IDebugOutput::mt_info :		clMsg = RGB(128, 128, 128); break;
		case IDebugOutput::mt_warning :		clMsg = RGB(255, 128, 64); break;
		case IDebugOutput::mt_error :		clMsg = RGB(255, 0, 0); break;
		default :							clMsg = RGB( 0, 0, 255 ); break;
		}
		CString strTime = COleDateTime::GetCurrentTime().Format( _T("%b%d %H:%M:%S") );

		if( m_pCB && m_pCB->OnReceiveMessage( mt, szMessage ) )
		{
			CStringW sTrimmedMsg( szMessage );
			sTrimmedMsg.TrimRight( L"\r\n" );
			HTREEITEM hItem = m_wndDebugMessage.InsertItem( strTime );
			m_wndDebugMessage.SetItemColor( hItem, clMsg, 0 );
			m_wndDebugMessage.SetSubItem( hItem, 1, OX_SUBITEM_COLOR | OX_SUBITEM_TEXT, sTrimmedMsg, 0, 0,  clMsg );
		}
	}
	m_MsgBuffer.clear();
	cs.Unlock();
}