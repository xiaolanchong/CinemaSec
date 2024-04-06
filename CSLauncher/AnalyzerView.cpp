// AnalyzerView.cpp : implementation file
//

#include "stdafx.h"
#include "CSLauncher.h"
#include "AnalyzerView.h"


// CAnalyzerView

IMPLEMENT_DYNCREATE(CAnalyzerView, CScrollView)

CAnalyzerView::CAnalyzerView()
{
}

CAnalyzerView::~CAnalyzerView()
{
}


BEGIN_MESSAGE_MAP(CAnalyzerView, CScrollView)
END_MESSAGE_MAP()


// CAnalyzerView drawing

void CAnalyzerView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = 640;
	sizeTotal.cy = 480;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CAnalyzerView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CAnalyzerView diagnostics

#ifdef _DEBUG
void CAnalyzerView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CAnalyzerView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG


// CAnalyzerView message handlers
