// F:\Project\VCproject\CinemaSec\CSClient\ARCHIVE_MODE\FileTransferDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CSClient.h"
#include "FileTransferDialog.h"
#include "..\..\CSEngine\CSEngine.h"
#include "..\..\CSChair\debug_int.h"
#include ".\filetransferdialog.h"
#include <shlwapi.h>

const UINT Timer_FileTransfer	= 0x15234;
const UINT Period_FileTransfer	= 1000;
// CFileTransferDialog dialog

IMPLEMENT_DYNAMIC(CFileTransferDialog, CDialog)
CFileTransferDialog::CFileTransferDialog( CWnd* pParent /*=NULL*/)
	: CDialog(CFileTransferDialog::IDD, pParent)
{
	IFileTransfer* pInt;
	DWORD dwRes = CreateEngineInterface( FILE_TRANSFER_INTERFACE, (void**)&pInt );
	if( dwRes != 0)
	{
		ASSERT(FALSE);
		// FIXME
		//AfxMessageBox( _T("Failed to create IFileTransfet interface"), MB_OK|MB_ICONERROR);
		return;
	}
	m_pTransfer = boost::shared_ptr<IFileTransfer>( pInt, ReleaseInterface<IFileTransfer>() );
}

CFileTransferDialog::~CFileTransferDialog()
{
}

void CFileTransferDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TRANSFER, m_wndTransferState );
}

void CFileTransferDialog::OnCancel()
{
	HRESULT hr = m_pTransfer->Cancel();
	ASSERT(  hr == S_OK );

	m_wndTransferState.DeleteAllItems();

	__super::OnCancel();
}

BOOL CFileTransferDialog::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();

	bool bFirst = true;
	if( !bFirst ) return res;
	else bFirst = false;
	
	const UINT IDC_STATUS_BAR = 435;
	m_wndStatus.Create(WS_CHILD|WS_VISIBLE|CCS_BOTTOM|SBARS_SIZEGRIP, CRect(0,0,0,0), this, IDC_STATUS_BAR);
//	VERIFY( m_wndStatus.SetText( _T("Text For Pane 0"), 0, 0) );

	CString sTitle;
	sTitle.LoadString( IDS_TITLE_FT_CAMERA );
	m_wndTransferState.InsertColumn( Col_CameraID,	sTitle,	LVCFMT_LEFT,	80 );
	sTitle.LoadString( IDS_TITLE_FT_FILENAME );
	m_wndTransferState.InsertColumn( Col_FileName,	sTitle,	LVCFMT_LEFT,	80 );
	sTitle.LoadString( IDS_TITLE_FT_FILESIZE );
	m_wndTransferState.InsertColumn( Col_FileSize,	sTitle,	LVCFMT_LEFT,	80 );
	sTitle.LoadString( IDS_TITLE_FT_SERVER );
	m_wndTransferState.InsertColumn( Col_Server,	sTitle,	LVCFMT_LEFT,	80 );
	sTitle.LoadString( IDS_TITLE_FT_STATE );
	m_wndTransferState.InsertColumn( Col_State,		sTitle,	LVCFMT_LEFT,	80 );

	m_LayoutManager.Attach(this);

	m_LayoutManager.AddAllChildren();

	m_LayoutManager.SetConstraint(IDCANCEL, OX_LMS_RIGHT,	OX_LMT_SAME, -3);
	m_LayoutManager.SetConstraint(IDCANCEL, OX_LMS_LEFT,	OX_LMT_OPPOSITE, -75-3);

	m_LayoutManager.SetConstraint(IDC_LIST_TRANSFER, OX_LMS_RIGHT,	OX_LMT_SAME,		-3);
	m_LayoutManager.SetConstraint(IDC_LIST_TRANSFER, OX_LMS_BOTTOM, OX_LMT_OPPOSITE,	-1, IDC_STATUS_BAR);

	m_LayoutManager.SetConstraint(IDC_STATUS_BAR, OX_LMS_RIGHT,		OX_LMT_SAME, 0);
	m_LayoutManager.SetConstraint(IDC_STATUS_BAR, OX_LMS_TOP,		OX_LMT_OPPOSITE, -20);
	m_LayoutManager.SetConstraint(IDC_STATUS_BAR, OX_LMS_BOTTOM,	OX_LMT_SAME, 0);

	m_LayoutManager.RedrawLayout();

	return res;
}

BEGIN_MESSAGE_MAP(CFileTransferDialog, CDialog)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CFileTransferDialog message handlers

void CFileTransferDialog::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	
	if( nIDEvent == Timer_FileTransfer )
	{
		if( CheckState() )
		{
			OnOK();
			m_CameraIDs.clear();
			m_wndTransferState.DeleteAllItems();
			KillTimer( Timer_FileTransfer );
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CFileTransferDialog::Transfer(	IDebugOutput* pDbgInt,
									CString sBasePath,
									const std::vector< std::pair<int, CString> >& Cameras, 
									CTime timeBegin, 
									CTime timeEnd 
								   )
{
	ASSERT( !Cameras.empty() );
	m_CameraIDs.clear();
	m_wndTransferState.DeleteAllItems();
	std::vector<IFileTransfer::CamRequest_t>	Request;
	WCHAR szBuf[MAX_PATH];
	CString sFileName;
	CString sCamera;
	
	for( size_t i = 0; i < Cameras.size(); ++i )
	{
		sFileName.Format( _T("%d.avi"), Cameras[i].first );
		PathCombineW( szBuf, sBasePath, sFileName );
		Request.push_back( IFileTransfer::CamRequest_t( 
			Cameras[i].first,
			(LPCWSTR)Cameras[i].second,
			szBuf
			) );
		m_CameraIDs.push_back( Cameras[i].first );

		sCamera.Format( _T("%d"), Cameras[i].first );

		int nIndex =  m_wndTransferState.GetItemCount(); 
		nIndex = m_wndTransferState.InsertItem( nIndex, sCamera );
		m_wndTransferState.SetItem( nIndex, Col_FileName,	LVIF_TEXT, sFileName, 0, 0, 0, 0  );
		m_wndTransferState.SetItem( nIndex, Col_Server,		LVIF_TEXT, Cameras[i].second, 0, 0, 0, 0  );
		
		CString sState;
		sState.LoadString( IDS_PREPARE );
		m_wndTransferState.SetItem( nIndex, Col_State,		LVIF_TEXT, sState, 0, 0, 0, 0  );
	}
	
	

	for( size_t i = 0; i < m_CameraIDs.size(); ++i )
	{
		
	}

	HRESULT hr = m_pTransfer->Start( pDbgInt, Request, timeBegin.GetTime(), timeEnd.GetTime() );
	if( hr != S_OK )
	{
		//FIXME
		AfxMessageBox( _T("Failed to start downloading"), MB_OK|MB_ICONERROR);
		return ;
	}
	SetTimer( Timer_FileTransfer, Period_FileTransfer, NULL );
}

bool CFileTransferDialog::IsTransfered()
{
	return m_pTransfer->IsBusy();
}

bool	CFileTransferDialog::CheckState()
{
	CString sItem;
	size_t nNumberOfFinished = 0;
	size_t nOkNumber = 0;
	for( size_t i = 0; i < m_CameraIDs.size(); ++i )
	{
		std::auto_ptr<IFileTransfer::IState> st = m_pTransfer->GetState( m_CameraIDs[i] );
		IFileTransfer::Working*					pWorking;
		IFileTransfer::Finished*				pFinished;
		IFileTransfer::CriticalErrorRequest*	pRequest;
		if( ( pRequest = dynamic_cast<IFileTransfer::CriticalErrorRequest*>(  st.get() ) ) != 0)
		{
			m_pTransfer->Cancel();
			KillTimer( Timer_FileTransfer );
			m_wndTransferState.SetItem( i, Col_State,	LVIF_TEXT, _T("Error"), 0, 0, 0, 0  );
			CStringW s;
			s.Format( IDS_ERROR_SERVERREQUEST, pRequest->m_sServerName.c_str() );
			AfxMessageBox(s, MB_OK|MB_ICONERROR);
			return true;
		}
		else if( dynamic_cast<IFileTransfer::CriticalErrorSocket*>(  st.get() ))
		{
			AfxMessageBox( _T("Another client already has been launched"), MB_OK|MB_ICONERROR);
			return true;
		}
		else if( dynamic_cast<IFileTransfer::NotStarted*>(  st.get() ))
		{

		}
		else if( ( pWorking = dynamic_cast<IFileTransfer::Working*>(  st.get() )  ) != 0)
		{
			CString sProgress;
			sProgress.Format( _T("%d%%"), int(pWorking->m_fCompleted * 100));
			m_wndTransferState.SetItem( i, Col_State,	LVIF_TEXT, sProgress, 0, 0, 0, 0  );
			TCHAR szBuf[20];
			StrFormatByteSize( pWorking->m_nFileSize, szBuf, 20 );
			m_wndTransferState.SetItem( i, Col_FileSize,	LVIF_TEXT, szBuf, 0, 0, 0, 0  );
		}
		else if( ( pFinished = dynamic_cast<IFileTransfer::Finished*>(  st.get() ) ) != 0)
		{
			CString sState;
			sState.LoadString( IDS_FINISHED );
			m_wndTransferState.SetItem( i, Col_State,	LVIF_TEXT, sState, 0, 0, 0, 0  );
			KillTimer( Timer_FileTransfer );
			switch( pFinished->m_nResultCode )
			{
			case IFileTransfer::Ok :
				++nOkNumber;
				break;
			case IFileTransfer::ErrorRequest:
				m_wndTransferState.SetItem( i, Col_State,	LVIF_TEXT, _T("Server did not answer"), 0, 0, 0, 0  );
				break;
			case IFileTransfer::ErrorUnhandled:
				m_wndTransferState.SetItem( i, Col_State,	LVIF_TEXT, _T("Unhandled exception"), 0, 0, 0, 0  );
				break;
			case IFileTransfer::ErrorTransfer:
				sItem.LoadString(IDS_TITLE_TRANSFER_ERROR);
				m_wndTransferState.SetItem( i, Col_State,	LVIF_TEXT, sItem, 0, 0, 0, 0  );
				break;
			case IFileTransfer::ErrorServerBusy:
				sItem = _T("Server busy");
				m_wndTransferState.SetItem( i, Col_State,	LVIF_TEXT, sItem, 0, 0, 0, 0  );
				break;
			case IFileTransfer::ErrorSourceError:
				sItem = _T("Source error");
				m_wndTransferState.SetItem( i, Col_State,	LVIF_TEXT, sItem, 0, 0, 0, 0  );
				break;
			default:
				ASSERT(FALSE);
			}
			++nNumberOfFinished;
		}
	}
	if( nOkNumber == m_CameraIDs.size() )
	{
		AfxMessageBox( IDS_TITLE_DOWNLOAD, MB_OK|MB_ICONINFORMATION);
		m_wndTransferState.DeleteAllItems();
		return true;
	}
	else if(nNumberOfFinished == m_CameraIDs.size())
	{
		AfxMessageBox( IDS_TITLE_TRANSFER_ERROR, MB_OK|MB_ICONERROR);
		m_wndTransferState.DeleteAllItems();
		return true;
	}
	return false;
}

void CFileTransferDialog::OnDestroy()
{
	__super::OnDestroy();

	KillTimer( Timer_FileTransfer );
	m_pTransfer->Cancel();
	m_pTransfer.reset();
}