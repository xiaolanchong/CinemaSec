#pragma once
#include "afxcmn.h"


// HistoSavePage dialog

class HistoSavePage : public CPropertyPage
{
	DECLARE_DYNAMIC(HistoSavePage)

public:
	HistoSavePage();
	virtual ~HistoSavePage();
	BOOL OnInitDialog();
	
	// main calculation thread
	static UINT CalcThreadFunc(void* parameter);
	// Dialog Data

	// Play List
	IPlayList* pPlayList;
	// Data Manager
	IPlayListDataManager* pDataManager;
	// current item number (that's being processed)
	size_t itemNum;
	

	enum { IDD = IDC_SAVEHISTO_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartSaveHisto();
	afx_msg void OnTimer(UINT nIDEvent);
	CProgressCtrl m_ctlProgress;
	
private:
	HANDLE hThread;
	int totalBars;
public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnStop();
	HRESULT StopHistoSaver(void);	

	HRESULT HighlightMe(bool DoUndo = true);
};
