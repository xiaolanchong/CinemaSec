#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>

#include "VideoRender/IVideoRender.h"

const UINT IDT_FrameBackground		= 0xfe;
const UINT Period_FrameBackground	= 100;

// CArrangeView view
class CArrangeDocument;

class CArrangeView : public CView
{
	DECLARE_DYNCREATE(CArrangeView)

	boost::shared_ptr< IVideoRender >	m_pArrayRender;
	boost::shared_ptr< IVideoRender >	m_pMergeRender;

	boost::shared_ptr< IVideoRender >	m_pRender;
	bool								m_bInCreate;

	bool								m_bUseMergeRender;
	bool								m_bDisableAction;
	bool								m_bShowCameraID;

	CFont								m_fntRender;


	void	SetWindowHeader( );

protected:
	CArrangeView();           // protected constructor used by dynamic creation
	virtual ~CArrangeView();


	bool		m_bAutoUpdate;
	
	boost::shared_ptr<Gdiplus::Image>	TakeScreenShot();
	void								SaveScreenShot( LPCWSTR szFileName, Image* pBmp );

	bool	CanTakeScreenShot();
	virtual void	PostLoadRoom() {};

	virtual void	AddContextMenuItem( CMenu&  ) {};
	virtual void	ProcessContextMenuItem( UINT_PTR  ) {};
public:
	virtual void	LoadRoom( int nRoomID, const WindowArray_t& wa );
	virtual void	UnloadRoom();
	void	SetImage( int nID, const std::vector<BYTE>& ImageArr);
	void	SetImage( int nID, const BITMAPINFO* pBmpInfo, const void* pBits);
	void	SetCameraSource( int nID, /*const std::wstring& sSource*/int nCameraID);
	void	SetVideoSource( int nID, const std::wstring& sSource );

	void	ClearImage();

	void	SetAutoUpdateSource( bool bAutoUpdate ) { m_bAutoUpdate = bAutoUpdate; }

	void	DisableAction( bool bDisableAction ) { m_bDisableAction = bDisableAction; }
	bool	IsActionDisable () const { return m_bDisableAction; }

	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CArrangeDocument* GetDocument();
	//{{AFX_MSG(CVideoView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	afx_msg LRESULT	OnRVideoClick(WPARAM wParam, LPARAM lParam);

	afx_msg void OnLButtonDblClk( UINT nFlags, CPoint point );
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnDestroy();
	//afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
	DECLARE_MESSAGE_MAP()
};


