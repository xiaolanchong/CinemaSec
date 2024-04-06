// ChildView.h : interface of the CChildView class
//


#pragma once


// CChildView window

struct IRoomRender;

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

protected:
	int m_nCounter;
	IRoomRender* m_pRoomRender;
	std::pair<int,int> m_CameraId;
	std::pair<int,int> m_LMBDownPos;

	BITMAPINFO m_biTL;
	BITMAPINFO m_biTR;
	BITMAPINFO m_biBL;
	BITMAPINFO m_biBR;


	boost::shared_array<BYTE> m_pTL;
	boost::shared_array<BYTE> m_pTR;
	boost::shared_array<BYTE> m_pBL;
	boost::shared_array<BYTE> m_pBR;

	CStatic m_wndStatic;
	CStatic m_wndStatic2;




	// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
};

