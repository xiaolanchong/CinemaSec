// AlbertsAlgorithmsPage.cpp : implementation file
//
#include "stdafx.h"
#include "CSPlayList.h"
#include "AlbertsAlgorithmsPage.h"
#include "./albertsalgorithmspage.h"
#include "AlbertsAlgorithmsLoader/AlbertsAlgorithmsLoader.h"
#include ".\albertsalgorithmspage.h"
#include "../CSChair/dmpstuff/ExceptionHandler.h"

#include "./PlayListParser/CSPlayListParser.h"

//#include "AlbertsAlgorithmsLoader/AlbertsAlgorithmsLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// AlbertsAlgorithmsPage dialog

typedef HRESULT (* fnCreateLearningAlgorithmInstance) ( csinterface::IInformationQuery   *, IDebugOutput *,csinterface::ILearningAlgorithm ** );
typedef LPCWSTR (* fnGetLearningAlgorithmName) ();


IMPLEMENT_DYNAMIC(AlbertsAlgorithmsPage, CPropertyPage)
AlbertsAlgorithmsPage::AlbertsAlgorithmsPage()
	: CPropertyPage(AlbertsAlgorithmsPage::IDD)
	, m_sAlgoDescription(_T(""))
	, bUserHitStop(false)
	, pDataManager(0)
	, pPlayList(0)
	, pAlgorithm(0)
	, saveOrNot(false)
	, finished(false)
{
}

AlbertsAlgorithmsPage::~AlbertsAlgorithmsPage()
{
}

void AlbertsAlgorithmsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_LIST, m_ctlFileList);
	DDX_Text(pDX, IDC_STATIC_DESCRIPTION, m_sAlgoDescription);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctlProgress);
	DDX_Text(pDX, IDC_FILENAME_TEXT, m_filename);
}

BOOL AlbertsAlgorithmsPage::OnInitDialog()
{
   
	CPropertyPage::OnInitDialog();
	totalBars = 1000;
	m_ctlProgress.SetRange32(0, totalBars);
	RefreshList();
	pDataManager = new AlbertsAlgorithmsLoader;
	EnableParametersBtns(false);

	LPWSTR *szArglist;
	int nArgs;
	
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	CString fullPath;
	
//	CWnd* pWnd = 

	if( nArgs == 2 )
	{
		fullPath = szArglist[1];
	
	
		CString strToFind =  _T("\\");
		for (int i = fullPath.GetLength() - 1; i > 0; i--)
		{
			if (fullPath[i] == strToFind) break;
		}
		playlistFileName = fullPath.Right(fullPath.GetLength() - i - 1); //_T("training_playlist.xml");
		playlistPath = fullPath.Left(i + 1);
	
		CString s;
		s = playlistPath + L"ini.xml";
		if (myParams.LoadParameters(s) == false)
			return FALSE;

		csinterface::TPlaylistParameters params;
		params.data = myParams;
		pDataManager->SetData(&params);
	}

	EnableStartBtn(false);
	EnableStopBtn(false);

	return TRUE;
}

HRESULT AlbertsAlgorithmsPage::GoToPluginsDirectory()
{
	WCHAR path[MAX_PATH];
	DWORD length = GetModuleFileName(0, path, MAX_PATH);
	//"\CSPlayList.exe
	wsprintf(path + length - 14, _T("plugins") );
	if(SetCurrentDirectory(path) == TRUE)
		return S_OK;
	else
		return E_FAIL;
		
}

HRESULT AlbertsAlgorithmsPage::RefreshList()
{
	if (GoToPluginsDirectory() != S_OK)
		AfxMessageBox(_T("Cant go to plugins directory"));
	
	CFileFind finder;
	BOOL bWorking = finder.FindFile(_T("*.dll"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (DllIsGood(finder.GetFileName()) == S_OK)
			m_ctlFileList.AddString(finder.GetFileName());
	}
	if (m_ctlFileList.GetCount() > 0)
	{
		CString sFirst;
		m_ctlFileList.SetCurSel(0);
		m_ctlFileList.GetText(0, sFirst);
		GetDescriptionFromDll(sFirst, m_sAlgoDescription);
		UpdateData(FALSE);
	}
	else
	{
		m_sAlgoDescription = _T("Nope!");
		UpdateData(FALSE);
	}
    return S_OK;
}
BEGIN_MESSAGE_MAP(AlbertsAlgorithmsPage, CPropertyPage)

	ON_BN_CLICKED(IDC_BTN_START, OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_END, OnBnClickedBtnEnd)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_FILE_LIST, OnLbnSelchangeFileList)
	ON_BN_CLICKED(IDC_BTN_LOAD_PARAMS, OnBnClickedBtnLoadParams)
	ON_BN_CLICKED(IDC_BTN_LOAD_UNLOAD, OnBnClickedBtnLoadUnload)
	ON_BN_CLICKED(IDC_SAVE_PARAMETERS, OnBnClickedSaveParameters)
	ON_BN_CLICKED(IDC_SET_PARAMETERS_BY_DEFAULT, OnBnClickedSetParametersByDefault)
	ON_BN_CLICKED(IDC_BTN_SHOW_PARAM_DIALOG, OnBnClickedBtnShowParamDialog)
	ON_BN_CLICKED(IDC_BTN_LOAD_PLAYLIST, OnBnClickedBtnLoadPlaylist)
	ON_BN_CLICKED(IDC_BTN_BREAK, OnBnClickedBtnBreak)
	ON_BN_CLICKED(IDC_RESET_PLAYLIST, OnBnClickedResetPlaylist)
END_MESSAGE_MAP()


// AlbertsAlgorithmsPage message handlers


HRESULT AlbertsAlgorithmsPage::DllIsGood(CString s_dllName)
{
	if (GoToPluginsDirectory() != S_OK)
		AfxMessageBox(_T("Cant go to plugins directory"));

	HMODULE myDll = AfxLoadLibrary(s_dllName);
	fnCreateLearningAlgorithmInstance MyCreateFunc = (fnCreateLearningAlgorithmInstance)GetProcAddress(myDll, "CreateLearningAlgorithmInstance");
	fnGetLearningAlgorithmName myGetName = (fnGetLearningAlgorithmName)GetProcAddress(myDll, "GetLearningAlgorithmName");
	AfxFreeLibrary(myDll);
	if (MyCreateFunc != 0 &&  myGetName != 0) return S_OK;
	return E_FAIL;
}

HRESULT AlbertsAlgorithmsPage::GetDescriptionFromDll(CString s_dllName, CString& description)
{
	if (GoToPluginsDirectory() != S_OK)
		AfxMessageBox(_T("Cant go to plugins directory"));

	HMODULE myDll = AfxLoadLibrary(s_dllName);
	//fnCreateLearningAlgorithmInstance MyCreateFunc = (fnCreateLearningAlgorithmInstance)GetProcAddress(myDll, "CreateLearningAlgorithmInstance");
	fnGetLearningAlgorithmName myGetName = (fnGetLearningAlgorithmName)GetProcAddress(myDll, "GetLearningAlgorithmName");
	if (myGetName == 0) 
	{
		AfxFreeLibrary(myDll);
		return E_FAIL;
	}
	description = myGetName();
	AfxFreeLibrary(myDll);
	return S_OK;
}

HRESULT AlbertsAlgorithmsPage::GetAlgorithmFromDll(CString s_dllName,  csinterface::ILearningAlgorithm ** ppAlgorithm)
{
	if (GoToPluginsDirectory() != S_OK)
		AfxMessageBox(_T("Cant go to plugins directory"));

	myDll = AfxLoadLibrary(s_dllName);
	fnCreateLearningAlgorithmInstance MyCreateFunc = (fnCreateLearningAlgorithmInstance)GetProcAddress(myDll, "CreateLearningAlgorithmInstance");
	csinterface::IInformationQuery * p = static_cast< csinterface::IInformationQuery*>(
		static_cast<AlbertsAlgorithmsLoader*>
		(pDataManager) );
		
	
	HRESULT res = MyCreateFunc( p, &myDebugOutput, ppAlgorithm);

	//AfxFreeLibrary(myDll);
	return res;
}

UINT AlbertsAlgorithmsPage::Handled_ServerThreadProc(void * pParam)
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
UINT AlbertsAlgorithmsPage::CalcThreadFunc(void* parameter)
{
	AlbertsAlgorithmsPage* pDlg = (AlbertsAlgorithmsPage*) parameter;

	VERIFY(pDlg               != NULL);
	VERIFY(pDlg->pDataManager != NULL);
	VERIFY(pDlg->pPlayList    != NULL);

	size_t totalItems = pDlg->pPlayList->size();

	// if all items are not processed we force algorithm to clean up temporary files
	bool bCleanUp = true;
	for (size_t i = 0; i < totalItems; i++)
	{
		bCleanUp = bCleanUp & !pDlg->pPlayList->IsItemProcessed(i);
	}

	if ( bCleanUp )
	{
		csinterface::TMsgCleanTempData cleanUpMsg;
		pDlg->pAlgorithm->SetData(&cleanUpMsg);
	}

    for (size_t i = 0; i < totalItems; i++)
	{	
		if ( pDlg->bUserHitStop == false && 
			 pDlg->bUserHitBreak == false &&
             pDlg->pPlayList->IsItemProcessed(i) == false &&			
			 pDlg->pDataManager->LoadOneItem((*pDlg->pPlayList)[i]) == S_OK )
		{
			
		//	m_filename = (*pDlg->pPlayList)[i].video.c_str();
		//	pDlg->UpdateData(FALSE);

 			VERIFY(SUCCEEDED(pDlg->pDataManager->Start()));
			if (pDlg->bUserHitBreak == false) pDlg->pPlayList->MarkAsProcessed(i);			
		}
	}
	pDlg->finished = true;
	return 0;
}

void AlbertsAlgorithmsPage::OnBnClickedBtnStart()
{	
	bUserHitStop = false;
	bUserHitBreak = false;
	finished = false;

	pPlayList = new CSPlayListParser;
	SetCurrentDirectory(playlistPath);
	if (pPlayList->LoadPlayList(playlistPath + playlistFileName) != S_OK)
	{
		AfxMessageBox(_T("this playlist cannot be loaded: ") + playlistPath + playlistFileName);
		return;
	}

	std::wstring resPath;
	pPlayList->GetResultDirectory(resPath);
	
	CString dllName;
	m_ctlFileList.GetText(m_ctlFileList.GetCurSel(), dllName);
	dllName = CString(resPath.c_str()) + _T("\\") + dllName;

	int resCreate = SHCreateDirectory(0, dllName);

	switch(resCreate) {
	case ERROR_FILE_EXISTS:
	case ERROR_ALREADY_EXISTS:
	case 0:
		{
			csinterface::TResultFilename fn;
			fn.data = dllName;
			pAlgorithm->SetData(&fn);
		}
		break;
	case ERROR_BAD_PATHNAME:
		AfxMessageBox(_T("bad result directory name"));
		return;
	case ERROR_FILENAME_EXCED_RANGE:
		AfxMessageBox(_T("result directory name exceded range"));
		return;		
	}

		
	//hThread = CreateThread (NULL,  0, CalcThreadFunc, this, 0, &dwThreadId );
	CWinThread * pThread = AfxBeginThread(Handled_ServerThreadProc, this);
	hThread = pThread->m_hThread;
	if ( !hThread )
	{
		OutputDebugString( _T("Cannot create calculation thread") );
		return;
	}
	EnableStopBtn(true);
	EnableStartBtn(false);
	bUserHitStop = false;

	SetTimer(1, 25, 0);	
	EnablePlaylistLoadBtn(false);
	EnableParametersBtns(false);
	EnableUnloadLoadAlgoBtn(false);

}

HRESULT AlbertsAlgorithmsPage::LoadSelectedAlgorithm()
{
	if (GoToPluginsDirectory() != S_OK)
		AfxMessageBox(_T("Cant go to plugins directory"));

	CString sDllName;
	m_ctlFileList.GetText(m_ctlFileList.GetCurSel(), sDllName);
	GetAlgorithmFromDll(sDllName, &pAlgorithm);

	csinterface::TPLearnAlgorithm algo;
	algo.data = pAlgorithm;
	return pDataManager->SetData(&algo);
}

void AlbertsAlgorithmsPage::OnBnClickedBtnEnd()
{
	bUserHitStop = true;
	StopAlgorithm(true);
}

void AlbertsAlgorithmsPage::OnBnClickedBtnBreak()
{
	bUserHitBreak = true;
	StopAlgorithm(false);
}

HRESULT AlbertsAlgorithmsPage::StopAlgorithm(bool saveResult)
{
	
	if (pDataManager) 
	{
		pDataManager->Stop(saveResult);
	}

	if (hThread)
	{
		DWORD res;

		if(myParams.waitForDllStop)
			res = WaitForSingleObject( hThread, INFINITE );
		else
			res = WaitForSingleObject(hThread, 30000);

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
	
	DeletePlaylist();
	return S_OK;
}

void AlbertsAlgorithmsPage::DeletePlaylist()
{
	if (pPlayList != 0)
	{
		pPlayList->SavePlayList( playlistPath + playlistFileName );
		pPlayList->Release();
		pPlayList = 0;
	}
}

void AlbertsAlgorithmsPage::OnDestroy()
{
	CPropertyPage::OnDestroy();
	if (saveOrNot)
	{
		bUserHitBreak = false;
		bUserHitStop = true;
		StopAlgorithm(true);
	}
	else
	{
		bUserHitStop = false;
		bUserHitBreak = true;
		StopAlgorithm(false);
	}

	if (pDataManager != NULL) 
	{
		pDataManager->Release();
		pDataManager = NULL;
	}

	if (pAlgorithm != 0)
	{
		pAlgorithm->Release();
		pAlgorithm = 0;
		AfxFreeLibrary(myDll);
	}
}
void AlbertsAlgorithmsPage::AskSaveOrNot()
{
	if (bUserHitBreak == false && bUserHitStop == false && finished == false)
	{
		if (AfxMessageBox(_T("Save result?"), MB_YESNO) == IDYES)
			saveOrNot = true;
		else
			saveOrNot = false;
	}
}
void AlbertsAlgorithmsPage::OnTimer(UINT nIDEvent)
{
	DWORD res = WaitForSingleObject( hThread, 0 );
	switch(res) {
	case WAIT_FAILED:
	case WAIT_OBJECT_0:
		EnableStartBtn(true);
		EnableStopBtn(false);
		EnablePlaylistLoadBtn(true);
		EnableParametersBtns(true);
		EnableUnloadLoadAlgoBtn(true);
		DeletePlaylist();

		KillTimer(1);
		break;
	case WAIT_TIMEOUT:

		break;
	default:;
	}

	switch( pDataManager->id() )
	{
	case 2:
		csinterface::TFrameIteratorProgress progress;

		if (pDataManager->GetData(&progress) == S_OK)
		{
			m_ctlProgress.SetPos((int)(progress.data * (float)totalBars));
		}
		csinterface::TQImage outimage;
		if (pDataManager->GetData(&outimage) == S_OK)
		{
			CWnd* pStatic = GetDlgItem(IDC_STATIC_VIDEO);
			CRect rect;
			pStatic->GetClientRect(rect);
			CDC*  pDC = pStatic->GetDC();
			//MyDrawColorImage(outimage.data, pDC->m_hDC, rect, INVERT_AXIS_Y);	
			csutility::DrawColorImage( &outimage.data, pDC->m_hDC, rect );
			pStatic->ReleaseDC(pDC);
		}

		break;	
	}
	CPropertyPage::OnTimer(nIDEvent);
}

void AlbertsAlgorithmsPage::OnLbnSelchangeFileList()
{
	CString dllName;
	m_ctlFileList.GetText(m_ctlFileList.GetCurSel(), dllName);
	GetDescriptionFromDll(dllName, m_sAlgoDescription);
	UpdateData(false);
}

void AlbertsAlgorithmsPage::OnBnClickedBtnLoadParams()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("parameters file (*.param)|*.param||"), this);
	dlg.DoModal();
	std::wstring path = dlg.GetFileName();
	csinterface::TMsgLoadParameters loadMsg;
	loadMsg.data = path;	
	pAlgorithm->SetData(&loadMsg);
}

void AlbertsAlgorithmsPage::EnableParametersBtns(bool enable)
{
	CWnd * pBtnLoad = GetDlgItem(IDC_BTN_LOAD_PARAMS); //Access to the button
	CWnd * pBtnSave = GetDlgItem(IDC_SAVE_PARAMETERS); //Access to the button
	CWnd * pBtnSetDefault = GetDlgItem(IDC_SET_PARAMETERS_BY_DEFAULT); //Access to the button
	CWnd * pBtnShow = GetDlgItem(IDC_BTN_SHOW_PARAM_DIALOG); //Access to the button

	pBtnLoad->EnableWindow(enable);
	pBtnSave->EnableWindow(enable); 
	pBtnSetDefault->EnableWindow(enable);
	pBtnShow->EnableWindow(enable);
}
void AlbertsAlgorithmsPage::EnableStartBtn(bool enable)
{
	CWnd * pBtnStart = GetDlgItem(IDC_BTN_START); //Access to the button
	pBtnStart->EnableWindow(enable);
}

void AlbertsAlgorithmsPage::EnableResetPlaylistBtn(bool enable)
{
	CWnd * pBtnStart = GetDlgItem(IDC_RESET_PLAYLIST); //Access to the button
	pBtnStart->EnableWindow(enable);
}


void AlbertsAlgorithmsPage::EnableStopBtn(bool enable)
{
	CWnd * pBtnStop = GetDlgItem(IDC_BTN_END); //Access to the button
	CWnd * pBtnBreak = GetDlgItem(IDC_BTN_BREAK); //Access to the button
	pBtnStop->EnableWindow(enable);
	pBtnBreak->EnableWindow(enable);
}
void AlbertsAlgorithmsPage::EnablePlaylistLoadBtn(bool enable)
{
	CWnd * pBtnLoadPlayList = GetDlgItem(IDC_BTN_LOAD_PLAYLIST); //Access to the button
	pBtnLoadPlayList->EnableWindow(enable);
}
void AlbertsAlgorithmsPage::EnableUnloadLoadAlgoBtn(bool enable)
{
	CWnd * pBtn = GetDlgItem(IDC_BTN_LOAD_UNLOAD); //Access to the button
	pBtn->EnableWindow(enable);
}


void AlbertsAlgorithmsPage::OnBnClickedBtnLoadUnload()
{
    CWnd * pBtn = GetDlgItem(IDC_BTN_LOAD_UNLOAD); //Access to the button
	CWnd * pLst = GetDlgItem(IDC_FILE_LIST); //Access to the button


	// if not loaded
	if (pAlgorithm == 0)
	{
		LoadSelectedAlgorithm();
		pBtn->SetWindowText(_T("Unload"));
		pLst->EnableWindow(false);
		EnableParametersBtns(true);
		EnableStartBtn(true);
	}
	else
	{
		pAlgorithm->Release();
		pAlgorithm = 0;
		pBtn->SetWindowText(_T("Load"));
		pLst->EnableWindow();
		AfxFreeLibrary(myDll);
		EnableParametersBtns(false);
		EnableStartBtn(false);
	}
	//enable buttons with options of the algorithm
	//change caption of the button to Unload
	
	// if loaded 
	// delete algorithm
	//
	//change caption of the button to Load
}

void AlbertsAlgorithmsPage::OnBnClickedSaveParameters()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("parameters file (*.param)|*.param||"), this);
	dlg.DoModal();
	std::wstring path = dlg.GetFileName();
	csinterface::TMsgSaveParameters saveMsg;
	saveMsg.data = path;	
	pAlgorithm->SetData(&saveMsg);
}

void AlbertsAlgorithmsPage::OnBnClickedSetParametersByDefault()
{
	csinterface::TMsgSetParametersToDefaults msg;
	pAlgorithm->SetData(&msg);
}

void AlbertsAlgorithmsPage::OnBnClickedBtnShowParamDialog()
{
	csinterface::TMsgShowParamDialog msg;
	pAlgorithm->SetData(&msg);
}

void AlbertsAlgorithmsPage::OnBnClickedBtnLoadPlaylist()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("playlist files (*.xml)|*.xml||"), this);
	dlg.DoModal();
	playlistFileName = dlg.GetFileName();
	playlistPath = dlg.GetPathName();
	playlistPath = playlistPath.Left(playlistPath.GetLength() - playlistFileName.GetLength());

	CString s;
	s = playlistPath + L"ini.xml";
	if (myParams.LoadParameters(s) == false)
		return;

	
	csinterface::TPlaylistParameters params;
	params.data = myParams;
	pDataManager->SetData(&params);
	EnableResetPlaylistBtn(true);
}

BOOL AlbertsAlgorithmsPage::OnQueryCancel()
{
	if (AfxMessageBox(_T("Exit?"), MB_YESNO) == IDYES)
	{
		AskSaveOrNot();
		return TRUE;
	}
	else
	return FALSE;
}


void AlbertsAlgorithmsPage::OnBnClickedResetPlaylist()
{

	CSPlayListParser parser;
	SetCurrentDirectory(playlistPath);
	if (parser.LoadPlayList(playlistPath + playlistFileName) != S_OK)
	{
		AfxMessageBox(_T("this playlist cannot be loaded: ") + playlistPath + playlistFileName);
		return;
	}
	else
	{
		for (int i = 0; i < (int)parser.size(); i++)
		{
			parser.MarkAsNotProcessed( i );
		}
	}
	parser.SavePlayList(playlistPath + playlistFileName);
}
