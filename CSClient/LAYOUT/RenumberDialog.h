#pragma once

#include "LayerHelperDialog.h"
#include "../MultiImageLayout.h"
#include "../Chair/ChairSerializer.h"
// CRenumberDialog dialog

class CRenumberDialog : public CLayerHelperDialog
{
	DECLARE_DYNAMIC(CRenumberDialog)

	virtual void						DrawItem( Gdiplus::Graphics& gr, CRect rc, DWORD_PTR dwCookie )	;
	virtual bool						GetItemMenuContent( std::vector<CString> &  MenuItems, WORD x, WORD y, bool& bCanDeleted    );
	virtual std::pair<bool, DWORD_PTR>	SelectMenuItem( size_t nIndex  );
	virtual void						DeleteItem( DWORD_PTR dwCookie ) { UNREFERENCED_PARAMETER(dwCookie);};

public:
	typedef std::vector<Arr2ub*>  SourceArr_t;

	CRenumberDialog(const SourceArr_t&				SrcArr,
					const std::vector<int>&			IdArr,
					const SparceRectImageLayout&	Layout,
					MyChair::ChairSet&				ChSet,
					CWnd* pParent = NULL);   // standard constructor
	virtual ~CRenumberDialog();

// Dialog Data
	enum { IDD = IDD_RENUMBER };

protected:
	const SourceArr_t&				m_SrcArr;
	const std::vector<int>&			m_IdArr;
	const SparceRectImageLayout&	m_Layout;
	MyChair::ChairSet&				m_ChSet;

	std::vector<int>				m_AttachedId;

	std::set< int >					m_AllChairId;
	
	size_t							m_nSelectedIndex;
	bool							m_bInit;

	void							MatchId();
	std::set<int>					GetNonAttached();
	void							RenumberAndErase();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitDialogOnce();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
