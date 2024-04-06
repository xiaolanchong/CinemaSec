#pragma once

#include "UT/OXLayoutManager.h"
#include "../CSEngine/mainserver/ThreadError.h"
#include "MultiImageLayout.h"
#include "BACKGROUND/Background.h"
#include "../CSChair/Grabber/IGrabberFactory.h"
#include "BACKGROUND/FrameBackground.h"
#include "LayoutWnd.h"
// CameraGrabDialog dialog

class CameraGrabDialog : public CDialog, private LayoutWndCB
{
//	DECLARE_DYNAMIC(CameraGrabDialog)
public:
	typedef std::vector< boost::shared_ptr<IBaseGrabber> >	SourceArr_t;
protected:

	virtual void						DrawItem( Gdiplus::Graphics& gr, CRect rc, DWORD_PTR dwCookie )	;
	virtual bool						GetItemMenuContent( std::vector<CString> &  MenuItems, WORD x, WORD y, bool& bCanDeleted    )	
	{
		UNREFERENCED_PARAMETER(MenuItems);
		UNREFERENCED_PARAMETER(x);
		UNREFERENCED_PARAMETER(y);
		UNREFERENCED_PARAMETER(bCanDeleted);
		return false;
	};
	virtual std::pair<bool, DWORD_PTR>	SelectMenuItem( size_t nIndex  ) 
	{ 
		return std::make_pair(false, 0);
	};
	virtual void						DeleteItem( DWORD_PTR dwCookie ){}
	static UINT _stdcall				ThreadProc( void * pParam );

	COXLayoutManager	m_LayoutManager;
	std::vector<Arr2f>	m_Images;
	bool				m_bUpdate;
	CLayoutWnd			m_wndLayout;

	ThreadSync			m_Sync;
	
	SparceRectImageLayout&			m_layout;
	CameraGrabDialog::SourceArr_t&	m_CamSrc;
	CSize							m_ImageDimension;

	std::vector<CFrameBackground>	m_Background;
	bool							m_fInit;
	CTime							m_timeStart;

	LONG							m_lFrames;
	void							SetFrameAndTime();
public:
	CameraGrabDialog(	SparceRectImageLayout& layout,   
						CameraGrabDialog::SourceArr_t&	CamSrc,
						CWnd* pParent = NULL);   // standard constructor
	virtual ~CameraGrabDialog();

	const std::vector<Arr2f>&	GetImages() { return m_Images; }
// Dialog Data
	enum { IDD = IDD_GRAB_CAMERA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	virtual void	OnOK();
//	virtual void	OnCancel();

	afx_msg void OnStop();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	//afx_msg void OnPaint();
};
