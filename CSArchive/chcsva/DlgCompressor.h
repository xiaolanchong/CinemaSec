// DlgCompressor.h: interface for the CDlgCompressor class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CDlgCompressor : public CDialogImpl<CDlgCompressor>
{
public:
	enum { IDD = IDD_COMPRESSOR };

	explicit CDlgCompressor();
	virtual ~CDlgCompressor();

	BEGIN_MSG_MAP(CDlgCompressor)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		COMMAND_ID_HANDLER(IDOK, OnCloseOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCancel)
		COMMAND_HANDLER(IDC_CONFIGURE, BN_CLICKED, OnConfigure)
		COMMAND_HANDLER(IDC_USE_DATARATE, BN_CLICKED, OnUseDatarate)
		COMMAND_HANDLER(IDC_USE_KEYFRAMES, BN_CLICKED, OnForceKeyframes)
		COMMAND_HANDLER(IDC_COMP_LIST, LBN_SELCHANGE, OnChangeCompressor)
		COMMAND_HANDLER(IDC_EDIT_QUALITY, EN_KILLFOCUS, OnQulityEditKillFocus)
	END_MSG_MAP()

public:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCloseOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCloseCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnConfigure(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnUseDatarate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnForceKeyframes(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnChangeCompressor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnQulityEditKillFocus(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	void SelectCompressor(DWORD fcc);
	void FreeCompressor();

	bool IsUsingSelected();
private:
	HIC    m_hic;
	ICINFO m_info;

	bool m_bSelected;
	COMPVARS* m_pCodec;
};