#pragma once

class CCamAnalyzerDoc;

//-------------------------------------------------------------------------------------------------
/** \class CCamAnalyzerView.
    \brief CCamAnalyzerView. */
//-------------------------------------------------------------------------------------------------
class CCamAnalyzerView : public CView
{
protected:
  CCamAnalyzerView();
  virtual ~CCamAnalyzerView();
  DECLARE_DYNCREATE(CCamAnalyzerView)

public:
  CCamAnalyzerDoc * GetDocument() const;

  //{{AFX_VIRTUAL(CCamAnalyzerView)
  virtual void OnUpdate( CView * pSender, LPARAM lHint, CObject * pHint );
  virtual void OnDraw( CDC * pDC );
  virtual void OnInitialUpdate();
  //}}AFX_VIRTUAL

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

public:
  //{{AFX_MSG(CCamAnalyzerView)
  afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CCamAnalyzerDoc * CCamAnalyzerView::GetDocument() const
{
  return reinterpret_cast<CCamAnalyzerDoc*>( m_pDocument );
}
#endif

