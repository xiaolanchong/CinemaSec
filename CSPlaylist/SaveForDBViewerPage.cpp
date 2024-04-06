// SaveForDBViewerPage.cpp : implementation file
//

#include "stdafx.h"
#include "CSPlayList.h"
#include "SaveForDBViewerPage.h"
#include ".\savefordbviewerpage.h"

#include "PlayListParser\CSPlayListParser.h"

#include "../CSChair/dmpstuff/ExceptionHandler.h"


// SaveForDBViewerPage dialog

IMPLEMENT_DYNAMIC(SaveForDBViewerPage, CPropertyPage)

SaveForDBViewerPage::SaveForDBViewerPage()
	: CPropertyPage(SaveForDBViewerPage::IDD)
	, pDataManager(0)
	, pPlayList(0)
	, hThread(0)
	, bUserHitStop(true)
{
}

SaveForDBViewerPage::~SaveForDBViewerPage()
{
}

void SaveForDBViewerPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_ctlProgress);
	DDX_Control(pDX, IDC_COMBO1, m_ctlComboSkip);
}


BEGIN_MESSAGE_MAP(SaveForDBViewerPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BTN_START, OnBnClickedBtnStart)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_STP, OnBnClickedBtnStp)
	ON_STN_CLICKED(IDC_TEST_VIDEO, OnStnClickedTestVideo)
END_MESSAGE_MAP()


// SaveForDBViewerPage message handlers

void SaveForDBViewerPage::OnBnClickedBtnStart()
{
	bUserHitStop = false;
	
	pPlayList = new CSPlayListParser;
	pPlayList->LoadPlayList(_T("training_playlist.xml"));

	pDataManager = new CSDBInterfacePreparer;

	CWinThread * pThread = AfxBeginThread(Handled_ServerThreadProc, this);
	hThread = pThread->m_hThread;	if ( !hThread )
	{
		OutputDebugString( _T("Cannot create calculation thread") );
		return;
	}
	csinterface::TSkipFramesNum nSkip;
	nSkip.data = m_ctlComboSkip.GetCurSel();
	pDataManager->SetData(&nSkip);
}

UINT SaveForDBViewerPage::Handled_ServerThreadProc(void * pParam)
{
	//	unsigned int Res = err_exception;
	__try
	{
		CalcThreadFunc(pParam);
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("Main server thread")))
	{
	}
	return 0;
}
UINT SaveForDBViewerPage::CalcThreadFunc(void* parameter)
{
	SaveForDBViewerPage* pDlg = (SaveForDBViewerPage*) parameter;
	VERIFY(pDlg               != NULL);
	VERIFY(pDlg->pDataManager != NULL);
	VERIFY(pDlg->pPlayList    != NULL);

	size_t totalItems = pDlg->pPlayList->size();
	
	for (size_t i = 0; i < totalItems; i++)
	{
		if (pDlg->bUserHitStop == false && 
			pDlg->pPlayList->IsItemProcessed(i) == false &&			
			pDlg->pDataManager->LoadOneItem((*pDlg->pPlayList)[i]) == S_OK )
		{
			pDlg->SetTimer(1, 2000, 0);	
			pDlg->pDataManager->Start();
			pDlg->KillTimer(1);	
			pDlg->pPlayList->MarkAsProcessed(i);
		}
	}
	return 0; 
}

void SaveForDBViewerPage::OnTimer(UINT nIDEvent)
{
	switch( pDataManager->id() )
	{
	// Save for DBInterface
	case 3:
		csinterface::TFrameIteratorProgress progress;
		
		if (pDataManager->GetData(&progress) == S_OK)
			m_ctlProgress.SetPos((int)(progress.data * (float)totalBars));

		csinterface::TQImage outimage;
		if (pDataManager->GetData(&outimage) == S_OK)
		{
			CWnd* pStatic = GetDlgItem(IDC_TEST_VIDEO);

			CRect rect;
			pStatic->GetClientRect(rect);
			CDC*  pDC = pStatic->GetDC();
			csutility::DrawColorImage(&outimage.data, pDC->m_hDC, rect);	
			pStatic->ReleaseDC(pDC);
		}
		break;
	}
	CPropertyPage::OnTimer(nIDEvent);
}


BOOL SaveForDBViewerPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	totalBars = 2000;
	m_ctlProgress.SetRange(0, totalBars);	
	
	
	for (int i = 0; i < 100; i++)
	{
		CString s;
		s.Format(_T("%d"), i); 
		m_ctlComboSkip.AddString(s);
	}
	m_ctlComboSkip.SetCurSel(10);
	return true;
}


HRESULT SaveForDBViewerPage::StopDBPreparer(void)
{
	if (pDataManager) pDataManager->Stop();

	if (hThread)
	{
		DWORD res = WaitForSingleObject( hThread, 5000 );
		if( res != WAIT_OBJECT_0 )
		{
			TerminateThread( hThread, DWORD(-1) );
			OutputDebugString( _T("Thread was terminated\n") );
		}
		else
		{
			OutputDebugString( _T("Thread exits normally\n"));
		}
	}

	if (pPlayList != NULL)
	{
		pPlayList->SavePlayList(_T("Training_playlist.xml"));
		delete pPlayList;
		pPlayList = NULL;
	}
	return S_OK;
}


void SaveForDBViewerPage::OnDestroy()
{
	CPropertyPage::OnDestroy();
	StopDBPreparer();
	if (pDataManager != NULL) 
	{
		pDataManager->Release();
		pDataManager = NULL;
	}
	if (pPlayList   != NULL)
	{
		delete pPlayList;
		pPlayList = NULL;
	}
}

void SaveForDBViewerPage::OnBnClickedBtnStp()
{
	bUserHitStop = true;
	StopDBPreparer();
}

void SaveForDBViewerPage::OnStnClickedTestVideo()
{
	// TODO: Add your control notification handler code here
}
