#pragma once

#include "../CSEngine/mainserver/ThreadError.h"
#include "MultiImageLayout.h"
#include "../BACKGROUND/Background.h"
#include "../CSChair/Grabber/IGrabberFactory.h"
#include "../BACKGROUND/PicBackground.h"
#include "LayerHelperDialog.h"
#include "afxcmn.h"

// CFileGrabDialog dialog

class CFileGrabDialog : public CLayerHelperDialog
{
//	DECLARE_DYNAMIC(CFileGrabDialog)

	virtual void						DrawItem( Gdiplus::Graphics& gr, CRect rc, DWORD_PTR dwCookie )	;
	virtual bool						GetItemMenuContent( std::vector<CString> &  MenuItems, WORD x, WORD y, bool& bCanDeleted    );
	virtual std::pair<bool, DWORD_PTR>	SelectMenuItem( size_t nIndex  );
	virtual void						DeleteItem( DWORD_PTR dwCookie );

protected:

	static UINT _stdcall				ThreadProc( void * pParam );
	void								SetFrame();

	typedef		std::vector< boost::shared_ptr<IBaseGrabber> >			FileSrc_t;
	typedef		std::vector< boost::shared_ptr<CBitmapBackground> >		PreviewSrc_t;

	std::vector<Arr2f>	m_Images;

	ThreadSync			m_Sync;

	SparceRectImageLayout&			m_layout;

	PreviewSrc_t					m_Background;
	std::vector<CString>			m_FileNames;

	std::vector<int>				m_IdArr;
	bool							m_fInit;
	CSize							m_ImageDimension;

	LONG							m_lFrames;
	size_t							m_nCurrentInd;
public:
	CFileGrabDialog(SparceRectImageLayout& layout, const std::vector<int>& IdArr, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFileGrabDialog();

	const std::vector<Arr2f>	GetImages() const { return m_Images;};
// Dialog Data
	enum { IDD = IDD_GRAB_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnStop();
//	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);

	virtual void OnOK();
	virtual void OnCancel();
	virtual void OnInitDialogOnce();

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_wndProgress;
};
