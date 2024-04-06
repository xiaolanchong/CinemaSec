// DlgShowStream.h: interface for the CDlgShowStream class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CDlgShowStream
{
public:
	explicit CDlgShowStream(CHCS::IStream* pStream);
	virtual ~CDlgShowStream();

	int DoModal(HWND hWndParent, LPARAM dwInitParam = NULL);

	static bool Create(HWND hWndParent, CHCS::IStream* pStream);

private:
	static BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	
	UINT m_nTimer;
	CHCS::IStream* m_pStream;
	CHCS::IFrame* m_pFrame;

	bool m_bOwner;

	// No copies do not implement
	CDlgShowStream(const CDlgShowStream &rhs);
	CDlgShowStream &operator=(const CDlgShowStream &rhs);
};