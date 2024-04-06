#pragma once

#include "LayerHelperSpinDialog.h"
#include "afxcmn.h"

// CLayoutDialog dialog
typedef std::vector< std::pair< int , boost::shared_ptr<CBackground> > >	AvailableSourceArr_t;

class CLayoutDialog :	public	CLayerHelperSpinDialog
{
	DECLARE_DYNAMIC(CLayoutDialog)

	virtual void						DrawItem( Gdiplus::Graphics& gr, CRect rc, DWORD_PTR dwCookie )	;
	virtual bool						GetItemMenuContent( std::vector<CString> &  MenuItems, WORD x, WORD y, bool& bCanDeleted    );
	virtual std::pair<bool, DWORD_PTR>	SelectMenuItem( size_t nIndex  );
	virtual void						DeleteItem( DWORD_PTR dwCookie );

public:

	CLayoutDialog( const  AvailableSourceArr_t& Arr, bool bUpdate, CWnd* pParent = NULL);   // standard constructor
	virtual ~CLayoutDialog();

	SparceRectImageLayout&	GetResult() { return m_ResultLayout;};
// Dialog Data
	enum { IDD = IDD_LAYOUT };
protected:
	
	typedef std::map< DWORD, size_t >	AttachedSourceMap_t;

	virtual void OnInitDialogOnce();
	virtual void OnOK();

	AvailableSourceArr_t	m_Available;
	std::set<size_t>		m_NonAttached;
	bool					m_bUpdate;
	SparceRectImageLayout	m_ResultLayout;

	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnTimer(UINT nIDEvent);
};
