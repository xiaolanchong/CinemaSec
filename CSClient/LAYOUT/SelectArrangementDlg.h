#pragma once


// CSelectArrangementDlg dialog

#include "../MultiImageLayout.h"

class CSelectArrangementDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectArrangementDlg)

	std::vector<int>		m_Cameras;
	SparceRectImageLayout	m_Layout;
public:
	CSelectArrangementDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectArrangementDlg();

// Dialog Data
	enum { IDD = IDD_SELECT_ROOM };

	const std::vector<int>&			GetCameras() const	{ return m_Cameras; }
	const SparceRectImageLayout&	GetLayout() const	{ return m_Layout;	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
};
