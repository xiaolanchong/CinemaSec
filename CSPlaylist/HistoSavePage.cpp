// HistoSavePage.cpp : implementation file
//

#include "stdafx.h"
#include "CSPlayList.h"
#include "HistoSavePage.h"
#include ".\histosavepage.h"

#include "CSPlayList.h"
#include "HistogramPreparer/CinemaHistogram.h"
//
#include "PlayListParser\CSPlayListParser.h"
#include "HistogramPreparer\CSHistogramPreparer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// HistoSavePage dialog

IMPLEMENT_DYNAMIC(HistoSavePage, CPropertyPage)
HistoSavePage::HistoSavePage()
	: CPropertyPage(HistoSavePage::IDD),
	 pDataManager(0),
	 pPlayList(0),
	 hThread(0)
{
}

HistoSavePage::~HistoSavePage()
{
}

BOOL HistoSavePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	totalBars = 2000;
	m_ctlProgress.SetRange(0, totalBars);	
	return true;
};

void HistoSavePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SAVE_PROGRESS, m_ctlProgress);
}

UINT HistoSavePage::CalcThreadFunc(void* parameter)
{
	HistoSavePage* pDlg = (HistoSavePage*) parameter;


	VERIFY(pDlg               != NULL);
	VERIFY(pDlg->pDataManager != NULL);
	VERIFY(pDlg->pPlayList    != NULL);
	/*
	CinemaHistogram<float> testHist(0.0, 10.0, 5);
	testHist.push_value(0.1, CinemaHistogram <float>::CLOTH);
	Int8Arr text;
	testHist[2].layer[1] = 1;

	//testHist.SaveToText(text);

	CinemaHistogram<float> empHist(0.0, 10.0, 5);

	empHist = testHist + testHist;
	empHist.SaveToText(text);
	*/
	size_t totalItems = pDlg->pPlayList->size();
	CinemaHistogram <float> totalStaticHist(0.0f, 2.0f, 256);

	for (size_t i = 0; i < totalItems; i++)
	{
		pDlg->SetTimer(1, 500, 0);	
		if (pDlg->pDataManager->LoadOneItem((*pDlg->pPlayList)[i]) == S_OK)
		{
			pDlg->pDataManager->Start();
			CinemaHistogram <float>  staticHist(0.0f, 2.0f, 256);				
			csinterface::TPStaticHistogram sHtransport(&staticHist);
			pDlg->pDataManager->GetData(&sHtransport);
			totalStaticHist = totalStaticHist + staticHist;
			pDlg->pDataManager->Stop();
		}
		pDlg->KillTimer(1);
	}

	//save to file // temporary code

	Int8Arr data;
	totalStaticHist.Normilize();
	totalStaticHist.SaveToText(data);

	HANDLE hFile;
	DWORD dwBytesWritten;
	hFile = CreateFile(_T("output.dat"),    // file name 
		GENERIC_WRITE,						// open for writing 
		0,					                // do not share 
		NULL,					            // no security 
		CREATE_ALWAYS,				       
		FILE_ATTRIBUTE_NORMAL,			    // normal file 
		NULL);  
	WriteFile(hFile, &(data[0]), (DWORD)data.size()*sizeof(__int8), &dwBytesWritten, NULL); 
	CloseHandle(hFile);
	//end temporary

	return 0;
}


BEGIN_MESSAGE_MAP(HistoSavePage, CPropertyPage)
	ON_BN_CLICKED(IDC_START_SAVE_HISTO, OnBnClickedStartSaveHisto)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_STOP, OnBnClickedBtnStop)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// HistoSavePage message handlers

void HistoSavePage::OnBnClickedStartSaveHisto()
{
	XmlLite::XMLDocument myDoc;
	VARIANT_BOOL b;

	HRESULT res = myDoc.m_pDoc->load( CComVariant(L"Training_playlist.xml"), &b );

	pPlayList = new CSPlayListParser;
	pPlayList->LoadPlayList(myDoc);

	pDataManager = new CSHistogramPreparer;

	DWORD dwThreadId;
	hThread = CreateThread (NULL,  0, (LPTHREAD_START_ROUTINE) CalcThreadFunc, this, 0, &dwThreadId );
	if ( !hThread )
	{
		OutputDebugString( _T("Cannot create calculation thread") );
		return;
	}
	HighlightMe();
}

void HistoSavePage::OnTimer(UINT nIDEvent)
{
	switch( pDataManager->id() )
	{
		//save histogram
	case 1:
		csinterface::TFrameIteratorProgress progress;

		pDataManager->GetData(&progress);
		m_ctlProgress.SetPos((int)(progress.data * (float)totalBars));

		csinterface::TQImage outimage;
		pDataManager->GetData(&outimage);
		CWnd* pStatic = GetDlgItem(IDC_STATIC_VIDEO);

		CRect rect;
		pStatic->GetClientRect(rect);
		CDC*  pDC = pStatic->GetDC();

		MyDrawColorImage(outimage.data, pDC->m_hDC, rect, INVERT_AXIS_Y);	
		break;
	}
	CPropertyPage::OnTimer(nIDEvent);
}

void HistoSavePage::OnDestroy()
{
	CPropertyPage::OnDestroy();

	StopHistoSaver();
	if (pDataManager != NULL) 
	{
		pDataManager->Release();
		pDataManager = NULL;
	}
	if (pPlayList    != NULL)
	{
		delete pPlayList;
		pPlayList = NULL;
	}
}


void HistoSavePage::OnBnClickedBtnStop()
{
	StopHistoSaver();
}

HRESULT HistoSavePage::StopHistoSaver(void)
{
	if (pDataManager) pDataManager->Stop();

	if (hThread)
	{
		DWORD res = WaitForSingleObject( hThread, 10000 );
		if( res != WAIT_OBJECT_0 )
		{
			TerminateThread( hThread, -1 );
			OutputDebugString( _T("Thread was terminated\n") );
		}
		else
		{
			OutputDebugString( _T("Thread exits normally\n"));
		}
	}
	HighlightMe(false);
	return S_OK;
}

HRESULT HistoSavePage::HighlightMe(bool DoUndo )
{
	if (DoUndo)
	{
		CPropertySheet * pSheet = (CPropertySheet *)this->GetParent();
		CTabCtrl * pTab = pSheet->GetTabControl();
		pTab->HighlightItem(pTab->GetCurSel());
	}
	else
	{
		CPropertySheet * pSheet = (CPropertySheet *)this->GetParent();
		CTabCtrl * pTab = pSheet->GetTabControl();
		pTab->HighlightItem(pTab->GetCurSel(), FALSE);
	}
	return S_OK;
}

