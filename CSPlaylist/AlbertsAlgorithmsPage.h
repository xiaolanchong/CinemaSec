#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "algo_debug_output.h"



// AlbertsAlgorithmsPage dialog

class AlbertsAlgorithmsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(AlbertsAlgorithmsPage)

	BOOL OnInitDialog();
public:
	AlbertsAlgorithmsPage();
	virtual ~AlbertsAlgorithmsPage();

// Dialog Data
	enum { IDD = IDD_ALBERT_ALGORITHMS_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CListBox m_ctlFileList;
public:
	HRESULT DllIsGood(CString s_dllName);
	HRESULT GetDescriptionFromDll(CString s_dllName, CString& description);
	HRESULT GetAlgorithmFromDll(CString s_dllName, csinterface::ILearningAlgorithm ** ppAlgorithm);
	HRESULT RefreshList();
	static UINT CalcThreadFunc(void* parameter);
	static UINT Handled_ServerThreadProc(void * pParam);
	HRESULT StopAlgorithm(bool saveResult);
	CString m_sAlgoDescription;
	HANDLE hThread;
	IPlayListDataManager * pDataManager;
	IPlayList * pPlayList;
	csinterface::ILearningAlgorithm * pAlgorithm;

	BOOL OnQueryCancel();
	void AskSaveOrNot();
	
	HRESULT HighlightMe(bool DoUndo);
	HRESULT LoadSelectedAlgorithm();
	bool bUserHitStop;
	bool bUserHitBreak;
	bool saveOrNot;
	bool finished;

	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnEnd();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	CProgressCtrl m_ctlProgress;
	int totalBars;
	HMODULE myDll;
	afx_msg void OnLbnSelchangeFileList();
	afx_msg void OnBnClickedBtnLoadParams();
	afx_msg void OnBnClickedBtnLoadUnload();
	afx_msg void OnBnClickedSaveParameters();
	afx_msg void OnBnClickedSetParametersByDefault();
	afx_msg void OnBnClickedBtnShowParamDialog();
	afx_msg void OnBnClickedBtnLoadPlaylist();
private:
	void EnableParametersBtns(bool enable);
	void EnableStartBtn(bool enable);
	void EnableStopBtn(bool enable);
	void EnablePlaylistLoadBtn(bool enable);
	void EnableUnloadLoadAlgoBtn(bool enable);
	void EnableResetPlaylistBtn(bool enable);
	void DeletePlaylist();

	HRESULT GoToPluginsDirectory();
	CString playlistFileName;
	CString playlistPath;
	PlayListParameters myParams;
	AlgoDebugOutput myDebugOutput;
public:
	afx_msg void OnBnClickedBtnBreak();
	afx_msg void OnBnClickedResetPlaylist();

	CString m_filename;
};
