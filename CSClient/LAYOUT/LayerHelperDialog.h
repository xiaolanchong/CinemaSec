#pragma once

#include "LayoutWnd.h"
#include "UT/OXLayoutManager.h"
// CLayerHelperDialog dialog

class CLayerHelperDialog :	public CDialog,
							public LayoutWndCB
{
	DECLARE_DYNAMIC(CLayerHelperDialog)

	bool	m_bInit;
public:
	CLayerHelperDialog(UINT nID, bool bAutoUpdate, CWnd* pParent = NULL);   // standard constructor
	virtual ~CLayerHelperDialog();

// Dialog Data
protected:

	COXLayoutManager	m_LayoutManager;
	CLayoutWnd			m_wndLayout;

	void	DrawHeader( Gdiplus::Graphics& gr, CRectF rc,CString sText);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnInitDialogOnce();

	DECLARE_MESSAGE_MAP()
};
