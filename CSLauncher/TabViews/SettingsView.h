#pragma once

#include "TabSelection.h"
#include "afxcmn.h"
#include "afxwin.h"

// CSettingsView form view

class SettingsTag : public SelectionTag
{
public:
	virtual bool		operator == ( const SelectionTag& rhs )
	{
		// assume it's single
		const SettingsTag* pTag = dynamic_cast< const SettingsTag* >( &rhs );
		return pTag != 0;
	}
	virtual CString		GetTabName() 	
	{
		return CString( _T("Settings"));
	}
};

class CSettingsView :	public CFormView, 
						public TabSelection
{
	DECLARE_DYNCREATE(CSettingsView)

protected:
	CSettingsView();           // protected constructor used by dynamic creation
	virtual ~CSettingsView();

#if 0
	bool	IsOptionChecked( int nID ) 
	{
		if( ! GetDlgItem( nID ) ) return true;
		return IsDlgButtonChecked( nID ) == BST_CHECKED;
	}
#endif

public:
	enum { IDD = IDD_CSLAUNCHER_FORM };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
#if 0
	bool		ShowDebug()		{  return IsOptionChecked( IDC_CHECK_DEBUG );	}
	bool		ShowInfo()		{  return IsOptionChecked( IDC_CHECK_INFO	);	}
	bool		ShowWarning()	{  return IsOptionChecked( IDC_CHECK_WARNING ); }
	bool		ShowError()		{  return IsOptionChecked( IDC_CHECK_ERROR );	}
	bool		ShowUnknown()	{  return IsOptionChecked( IDC_CHECK_UNKNOWN ); }
#endif
	void		SetConnectionString ( LPCWSTR szCS );

	int			GetMaxLineNumber();			

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual	void OnInitialUpdate();

	DECLARE_MESSAGE_MAP()
public:
	CSpinButtonCtrl m_spinLineNumber;
	CRichEditCtrl m_edConnectionString;
#if 0
	CButton m_chkDebug;
	CButton m_chkInfo;
	CButton m_chkWarning;
	CButton m_chkError;
	CButton m_chkUnknown;
#endif
};

