#pragma once

#include "LayerHelperDialog.h"
// CLayerHelperSpinDialog dialog

class CLayerHelperSpinDialog : public CLayerHelperDialog
{
	DECLARE_DYNAMIC(CLayerHelperSpinDialog)

public:
	CLayerHelperSpinDialog(UINT nID, bool bAutoUpdate, CWnd* pParent = NULL);   // standard constructor
	virtual ~CLayerHelperSpinDialog();


protected:
	CSpinButtonCtrl m_spinWidth;
	CSpinButtonCtrl m_spinHeight;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitDialogOnce();

	afx_msg void	OnGridWidthChange( NMHDR* pHdr, LRESULT * pResult );
	afx_msg void	OnGridHeightChange( NMHDR* pHdr, LRESULT * pResult );

	DECLARE_MESSAGE_MAP()
};
