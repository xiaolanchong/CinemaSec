// CSClientView.cpp : implementation of the CCSClientView class
//

#include "stdafx.h"
#include "CSClient.h"

#include "CSClientDoc.h"
#include "CSClientView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCSClientView

IMPLEMENT_DYNCREATE(CCSClientView, CView)

BEGIN_MESSAGE_MAP(CCSClientView, CView)
	//{{AFX_MSG_MAP(CCSClientView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCSClientView construction/destruction

CCSClientView::CCSClientView()
{
	// TODO: add construction code here

}

CCSClientView::~CCSClientView()
{
}

BOOL CCSClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CCSClientView drawing

void CCSClientView::OnDraw(CDC* pDC)
{
	CCSClientDoc* pDoc = GetDocument();
	UNREFERENCED_PARAMETER(pDC);
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CCSClientView printing

BOOL CCSClientView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CCSClientView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CCSClientView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CCSClientView diagnostics

#ifdef _DEBUG
void CCSClientView::AssertValid() const
{
	CView::AssertValid();
}

void CCSClientView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCSClientDoc* CCSClientView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCSClientDoc)));
	return (CCSClientDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCSClientView message handlers
