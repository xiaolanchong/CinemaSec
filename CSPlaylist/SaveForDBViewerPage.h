#pragma once
#include "DBInterfacePreparer/CSDBInterfacePreparer.h"
#include "afxcmn.h"
#include "afxwin.h"

// SaveForDBViewerPage dialog

class SaveForDBViewerPage : public CPropertyPage
{
	DECLARE_DYNAMIC(SaveForDBViewerPage)

public:
	BOOL OnInitDialog();

	SaveForDBViewerPage();
	virtual ~SaveForDBViewerPage();

// Dialog Data
	enum { IDD = IDD_SAVE_FOR_DBVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	static UINT CalcThreadFunc(void* parameter);
	static UINT Handled_ServerThreadProc(void * pParam);

	afx_msg void OnBnClickedBtnStart();
	HRESULT StopDBPreparer(void);
	HRESULT HighlightMe(bool DoUndo = true);


	IPlayListDataManager* pDataManager;
	IPlayList           * pPlayList;
	
	
	HANDLE hThread;
	afx_msg void OnTimer(UINT nIDEvent);
	CProgressCtrl m_ctlProgress;
	int totalBars;
	bool bUserHitStop;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnStp();
	CComboBox m_ctlComboSkip;
	afx_msg void OnStnClickedTestVideo();
};
