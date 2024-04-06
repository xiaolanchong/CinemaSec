// CSSeqRefinerView.h : interface of the CCSSeqRefinerView class
//


#pragma once


/////////////////////////////////////////////////////////////////////////////
// COleListCtrlDropTarget window
class COleListCtrlDropTarget : public COleDropTarget
{
public:
	COleListCtrlDropTarget();
	virtual ~COleListCtrlDropTarget();   

public:
	void OnDragLeave(CWnd* pWnd);               
	DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );
};


struct ListInfo
{
	LV_ITEM lvi;
	char szLabel[256];
	std::pair < Arr2ub, unsigned __int32 > image;
	int w;
	int h;
};

//////////////////////////////////
// Control types

enum {CT_LIST, CT_EDIT, CT_TREE};

//////////////////////////////////////////////////////////////////////////

class CCSSeqRefinerView : public CListView
{
protected: 
	CCSSeqRefinerView();
	DECLARE_DYNCREATE(CCSSeqRefinerView)


public:
	CCSSeqRefinerDoc* GetDocument() const;


public:
	//Get and set the source rectangle for drag/drop operation
	void  SetDragSourceRect();
	CRect GetDragSourceRect();
	void RemoveHighlightFromDropTarget();
	int HighlightDropTarget (CPoint point);
	void SelectDropTarget(int iItem);
	void SetLocalDD(BOOL bState)   { m_bLocalDD   = bState; }
	void SetScrolling(BOOL bState) { m_bScrolling = bState; }
	void SetRemove(BOOL bState)    { m_bScrolling = bState; }

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate(); 

public:
	virtual ~CCSSeqRefinerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	COleDataSource mOleDataSource;
	COleListCtrlDropTarget mOleListDropTarget;
	CRect m_DragSourceRect;
	boost::shared_ptr<CImageList> m_pDocImageList;


	int m_nDelayInterval;
	int m_nScrollMargin;
	int m_nScrollInterval;

	//
	CImageList* m_pImageList;
	BOOL        m_bDragging;
	int         m_iDragItem; // Source item index on which d&d started
	int         m_iDropItem; // Target item index

	BOOL        m_bSource;   // True if list is source
	BOOL        m_bTarget;   // True if list is target

	//Options available
	BOOL        m_bLocalDD;   // True if local Drag&Drop allowed
	BOOL        m_bScrolling; // True if auto scrolling
	BOOL        m_bRemove;    // True if D&D items must be removed

	//Array used for saving D&D item indexes and delete them
	int*        m_pSaveIndItem;
	int         m_nItem;
public:

protected:

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // debug version in CSSeqRefinerView.cpp
inline CCSSeqRefinerDoc* CCSSeqRefinerView::GetDocument() const
   { return reinterpret_cast<CCSSeqRefinerDoc*>(m_pDocument); }
#endif

