#if !defined(AFX_VIDEODOCUMENT_H__37CC9FC9_892D_471F_B3E4_BB25F45FD6EE__INCLUDED_)
#define AFX_VIDEODOCUMENT_H__37CC9FC9_892D_471F_B3E4_BB25F45FD6EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoDocument.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVideoDocument document

#include "../CommonView/wv_message.h"
#include "../CommonView/ArrangeDocument.h"

class CVideoView;
class CVideoStatView;

class CVideoDocument : public CArrangeDocument
{
protected:
	CVideoDocument();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CVideoDocument)

	virtual void	LoadRoom( int nRoomID );
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoDocument)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVideoDocument();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CVideoDocument)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEODOCUMENT_H__37CC9FC9_892D_471F_B3E4_BB25F45FD6EE__INCLUDED_)
