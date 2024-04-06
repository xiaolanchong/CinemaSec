// CSClientDoc.cpp : implementation of the CCSClientDoc class
//

#include "stdafx.h"
#include "CSClient.h"

#include "CSClientDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCSClientDoc

IMPLEMENT_DYNCREATE(CCSClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CCSClientDoc, CDocument)
	//{{AFX_MSG_MAP(CCSClientDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCSClientDoc construction/destruction

CCSClientDoc::CCSClientDoc()
{
	// TODO: add one-time construction code here

}

CCSClientDoc::~CCSClientDoc()
{
}

BOOL CCSClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CCSClientDoc serialization

void CCSClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCSClientDoc diagnostics

#ifdef _DEBUG
void CCSClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCSClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCSClientDoc commands
