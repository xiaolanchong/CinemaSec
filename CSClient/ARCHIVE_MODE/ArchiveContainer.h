#pragma once

#include "../gui/Slider/SeekToolBar.h"
#include "../gui/RangeCtrl/RangeSlider.h"
#include "../UT/OXLayoutManager.h"
// CArchiveContainer view
class CArchiveDoc;
class CArrangeView;
class CVideoControlView;

class CArchiveContainerView : public CView
{
	bool							m_bInitialized;

protected:
	DECLARE_DYNCREATE(CArchiveContainerView)

	CArrangeView*					m_pView;
	CVideoControlView*				m_pControlView;
	COXLayoutManager				m_LayoutManager;

public:


	CArchiveContainerView();
	virtual ~CArchiveContainerView();

	virtual void OnInitialUpdate( );
	virtual void OnDraw( CDC* pDC);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif
	CArchiveDoc * GetDocument();
		
	afx_msg void OnBarUpdate( CCmdUI* pCmdUI );
	afx_msg BOOL OnEraseBkgnd(CDC* pDC) ;
	afx_msg void OnSize( UINT nType, int cx, int cy );

	DECLARE_MESSAGE_MAP()
};

inline CArchiveDoc * CArchiveContainerView::GetDocument() // non-debug version
{
	return reinterpret_cast<CArchiveDoc*>( m_pDocument );
}

