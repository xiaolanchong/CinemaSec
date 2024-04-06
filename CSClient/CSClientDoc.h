// CSClientDoc.h : interface of the CCSClientDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSCLIENTDOC_H__287194C0_A4CD_4827_977A_A069EA59C7CC__INCLUDED_)
#define AFX_CSCLIENTDOC_H__287194C0_A4CD_4827_977A_A069EA59C7CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCSClientDoc : public CDocument
{
protected: // create from serialization only
	CCSClientDoc();
	DECLARE_DYNCREATE(CCSClientDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCSClientDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCSClientDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCSClientDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSCLIENTDOC_H__287194C0_A4CD_4827_977A_A069EA59C7CC__INCLUDED_)
