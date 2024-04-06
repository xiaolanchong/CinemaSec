// VideoCapDlg.h : header file
//

#pragma once

#include "VideoCapControl.h"
#include "ImageStatic.h"
#include "TimeTable.h"
#include "CameraState.h"
#include <boost/shared_ptr.hpp>

// CVideoCapDlg dialog
class CVideoCapDlg : public CDialog
{
	VideoCapControlFactory		m_VideoCap;

	CTreeCtrl					m_wndCameraTree;
	CImageStatic				m_wndImage;
#if 0
	CSpinButtonCtrl				m_wndFPS;
	CComboBox					m_wndEncoder;
#endif
	TimeTableLoader				m_TimeLoader;
	std::vector<boost::shared_ptr< CameraState > >	m_CameraStates;

	void						CreateTree();
	void						Initialize();

//	void						InitControls();
// Construction
public:
	CVideoCapDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VIDEOCAP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void	OnNotifyItemSel( NMHDR* pHdr, LRESULT * pResult );
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonDirectory();
};
