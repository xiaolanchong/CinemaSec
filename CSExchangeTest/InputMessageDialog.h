#pragma once

#include "..\CSClient\GUI\HtmlCtrl\HtmlCtrl.h"
#include "afxwin.h"

// CInputMessageDialog dialog

class CInputMessageDialog : public CPropertyPage
{
	DECLARE_DYNAMIC(CInputMessageDialog)

	CHtmlCtrl					m_wndHtml;
	CListBox					m_wndMessage;

	std::vector<std::vector<BYTE> >	m_IncomingMessages;

	void	AddMessage( const std::wstring& sMsg );
	std::pair<bool, int> ParseMessage( const std::wstring& sMsg );
	void	SendMessage( int nID);
public:
	CInputMessageDialog();
	virtual ~CInputMessageDialog();

// Dialog Data
	enum { IDD = IDD_INPUT_MESSAGE };

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL	OnInitDialog();
	

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnIncomingMessage( WPARAM wParam, LPARAM lParam );
	afx_msg void OnLbnSelchangeListMessage();
};
