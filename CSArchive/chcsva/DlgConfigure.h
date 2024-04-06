// DlgConfigure.h: interface for the CDlgConfigure class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CDlgConfigure : public CDialogImpl<CDlgConfigure>
{
public:
	CDlgConfigure();
	~CDlgConfigure();

	enum { IDD = IDD_CONFIGURE};

	BEGIN_MSG_MAP(CDlgConfigure)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_SELECT_CODEC, OnSelectCodec)
		COMMAND_ID_HANDLER(IDC_BROWSE_DATABASE, OnBrowseDatabase)
		COMMAND_ID_HANDLER(IDC_BROWSE_BASEPATH, OnBrowseBasepath)
	END_MSG_MAP()

protected:
	// Message handlers
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelectCodec(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBrowseDatabase(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBrowseBasepath(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

public:
	long m_lVideoFPM;				// Video Frame per minute
	long m_lVideoMaxSize;			// Video max size in seconds

	TCHAR m_stBasePath[MAX_PATH];	// Archive base path
	TCHAR m_stDBInitString[4096];	// DB Initialization string

	bool m_bUpdateBasePath;
	bool m_bUpdateDBString;
	bool m_bUpdateCodec;

	COMPVARS m_Codec;

	bool LoadSettings();
	bool SaveSettings();

private:
	void InitCompressor();
	void FreeCompressor();

	void UpdateCodecName(LPCWSTR szDriverName = NULL);
};