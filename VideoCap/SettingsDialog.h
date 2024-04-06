#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include <vector>
#include <string>
// CSettingsDialog dialog

class CSettingsDialog : public CDialog
{
	DECLARE_DYNAMIC(CSettingsDialog)

	int				m_nFPS;
	std::wstring	m_sEncoder;
	void	GetParam();

	std::vector< std::wstring >	m_Encoders;
public:
	CSettingsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDialog();

	static int GetDefaultFPS() { return 10;}
// Dialog Data
	enum { IDD = IDD_SETTINGS };

	int				GetFPS()		const { return m_nFPS;	 }
	std::wstring	GetEncoder()	const { return m_sEncoder;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CSpinButtonCtrl m_spinFPS;
	CComboBox m_wndEncoder;
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
};
