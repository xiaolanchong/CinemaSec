#pragma once

#include "../../CSEngine/FileTransfer/IFileTransfer.h"
#include "../UT/OXLayoutManager.h"
// CFileTransferDialog dialog

class CFileTransferDialog : public CDialog
{
	DECLARE_DYNAMIC(CFileTransferDialog)

	boost::shared_ptr<IFileTransfer>	m_pTransfer;
	std::vector<int>					m_CameraIDs;

	COXLayoutManager					m_LayoutManager;
	enum
	{
		Col_CameraID	= 0,
		Col_FileName	= 1,
		Col_FileSize	= 2,
		Col_Server		= 3,
		Col_State		= 4
	};

	bool	CheckState();
public:
	CFileTransferDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFileTransferDialog();

	void	Transfer(	IDebugOutput* pDbgInt,
						CString sBasePath,
						const std::vector< std::pair<int, CString> >& Cameras, 
						CTime timeBegin, 
						CTime timeEnd );
	bool	IsTransfered();

// Dialog Data
	enum { IDD = IDD_FILE_TRANSFER };

protected:
	CListCtrl		m_wndTransferState;
	CStatusBarCtrl	m_wndStatus;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
};
