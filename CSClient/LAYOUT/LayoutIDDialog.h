#pragma once

#include "LayerHelperSpinDialog.h"

// CLayoutIDDialog dialog

class CLayoutIDDialog : public CLayerHelperSpinDialog
{
	DECLARE_DYNAMIC(CLayoutIDDialog)

	virtual void						DrawItem( Gdiplus::Graphics& gr, CRect rc, DWORD_PTR dwCookie )	;
	virtual bool						GetItemMenuContent( std::vector<CString> &  MenuItems, WORD x, WORD y, bool& bCanDeleted    );
	virtual std::pair<bool, DWORD_PTR>	SelectMenuItem( size_t nIndex  );
	virtual void						DeleteItem( DWORD_PTR dwCookie );

public:
	CLayoutIDDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLayoutIDDialog();
protected:

	std::vector<CLayoutWnd::Coord>	m_Coord;
	//std::set<DWORD_PTR>		m_AttachedId;
	DWORD_PTR				m_dwCount;
// Dialog Data
	enum { IDD = IDD_LAYOUT };
public:
	const std::vector<CLayoutWnd::Coord>	GetResult() const { return m_Coord; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	void OnInitDialogOnce();
	

	DECLARE_MESSAGE_MAP()
};
