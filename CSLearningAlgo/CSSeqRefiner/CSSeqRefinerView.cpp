// CSSeqRefinerView.cpp : implementation of the CCSSeqRefinerView class
//

#include "stdafx.h"
#include "CSSeqRefiner.h"
#include "MainFrm.h"

#include "CSSeqRefinerDoc.h"
#include "CSSeqRefinerView.h"
#include ".\csseqrefinerview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCSSeqRefinerView
IMPLEMENT_DYNCREATE(CCSSeqRefinerView, CListView)
BEGIN_MESSAGE_MAP(CCSSeqRefinerView, CListView)
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnLvnBegindrag)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CCSSeqRefinerView construction/destruction
CCSSeqRefinerView::CCSSeqRefinerView()
{
	m_iDragItem  = -1;
	m_bDragging  = FALSE;
	m_pImageList = NULL;

	m_bSource = FALSE;
	m_bTarget = FALSE;

	// By default, all options are available
	m_bLocalDD   = TRUE; // Local Drag & Drop
	m_bScrolling = TRUE; // Auto Scrolling
	m_bRemove    = TRUE; // Remove items

	m_pSaveIndItem = NULL;
	m_nItem = 0;

	m_nDelayInterval = 300;  // Default delay interval = 500 milliseconds
	m_nScrollMargin = 10;    // Default scroll margin = 10 pixels
	m_nScrollInterval = 100; // Default scroll interval = 200 milliseconds
}

CCSSeqRefinerView::~CCSSeqRefinerView()
{
	// Delete the image list created by CreateDragImage
	if (m_pImageList != NULL)
		delete m_pImageList;
}

BOOL CCSSeqRefinerView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CListView::PreCreateWindow(cs);
}

void CCSSeqRefinerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	CListCtrl& listCtrl = GetListCtrl();
	CCSSeqRefinerDoc* pDoc = GetDocument();

	listCtrl.SetExtendedStyle( LVS_EX_BORDERSELECT | LVS_EX_FLATSB );
	listCtrl.SetBkColor( 0x00e0e0e0 );
	listCtrl.SetOutlineColor( 0x00ff00ff );
	listCtrl.SetIconSpacing( 90,90 );

	int imageCount = pDoc->GetImageCount();
	int imageWidth = pDoc->GetImageWidth();
	int imageHeight = pDoc->GetImageHeight();
	int imageByteSize = imageWidth * imageHeight * 4;

	if( imageCount > 0 )
	{

		m_pDocImageList = boost::shared_ptr< CImageList >( new CImageList() );
		m_pDocImageList->Create( imageWidth, imageHeight, ILC_COLOR32, imageCount,16 );
		listCtrl.SetImageList( m_pDocImageList.get(),  LVSIL_NORMAL );

		CBitmap bmp;
		bmp.CreateBitmap( imageWidth, imageHeight, 1, 32, NULL );
		boost::scoped_array< BYTE > buffer( new BYTE[ imageByteSize ] );

		unsigned __int32 frameTime; 
		Arr2ub image;

		for( int i = 0; i < imageCount; ++i )
		{
            pDoc->GetImage( i, frameTime, image );


			for( int j = 0, k = 0; j < imageByteSize; j+=4, ++k )
			{
				buffer[j+0] = image[k];
				buffer[j+1] = image[k];
				buffer[j+2] = image[k];
			}
			
			bmp.SetBitmapBits( imageByteSize, buffer.get() );
			m_pDocImageList->Add( &bmp, (CBitmap*)(0) );
		}
	}
    

	boost::scoped_array<TCHAR> chBuffer( new TCHAR[32] );
	listCtrl.SetRedraw( FALSE );

	for( int i = 0; i < imageCount; ++i )
	{
	//	swprintf( &chBuffer[0], L"#%d", i );
		listCtrl.InsertItem( i, 0, i );
	}

	listCtrl.SetRedraw( TRUE );
}


// CCSSeqRefinerView diagnostics
#ifdef _DEBUG
void CCSSeqRefinerView::AssertValid() const
{
	CListView::AssertValid();
}

void CCSSeqRefinerView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CCSSeqRefinerDoc* CCSSeqRefinerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCSSeqRefinerDoc)));
	return (CCSSeqRefinerDoc*)m_pDocument;
}
#endif //_DEBUG


// CCSSeqRefinerView message handlers
void CCSSeqRefinerView::OnTimer(UINT nIDEvent)
{
	CListView::OnTimer(nIDEvent);

	CListCtrl& listCtrl = GetListCtrl();

	if (!m_bScrolling)
		return;

	// Reset the timer.
	SetTimer (1, m_nScrollInterval, NULL);

	// Get the current cursor position and window height.
	DWORD dwPos = ::GetMessagePos ();
	CPoint point (LOWORD (dwPos), HIWORD (dwPos));
	ScreenToClient (&point);

	CRect rect;
	GetClientRect (rect);
	int cy = rect.Height ();

	// Scroll the window if the cursor is near the top or bottom.
	if (point.y <= m_nScrollMargin) 
	{
		int iFirstVisible = listCtrl.GetTopIndex();

		m_pImageList->DragShowNolock (FALSE);
		SendMessage (WM_VSCROLL, MAKEWPARAM (SB_LINEUP, 0), NULL);
		m_pImageList->DragShowNolock (TRUE);

		//  Kill the timer if the window did not scroll, or redraw the
		//  drop target highlight if the window did scroll.
		if ( listCtrl.GetTopIndex() == iFirstVisible)
			KillTimer (1);
		else 
		{
			HighlightDropTarget (point);
			return;
		}
	}
	else if (point.y >= cy - m_nScrollMargin) 
	{
		int iFirstVisible = listCtrl.GetTopIndex ();
		m_pImageList->DragShowNolock (FALSE);

		SendMessage (WM_VSCROLL, MAKEWPARAM (SB_LINEDOWN, 0), NULL);
		m_pImageList->DragShowNolock (TRUE);

		// Kill the timer if the window did not scroll, or redraw the
		// drop target highlight if the window did scroll.
		if ( listCtrl.GetTopIndex() == iFirstVisible)
			KillTimer (1);
		else 
		{
			HighlightDropTarget (point);
			return;
		}
	}
}

void CCSSeqRefinerView::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	CListCtrl& listCtrl = GetListCtrl();


	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int iItem =  ((NM_LISTVIEW*) pNMHDR)->iItem;

	m_iDragItem = iItem;   // Item on which D&D is started
	m_iDropItem = -1;      // Not yet known
	m_bDragging = TRUE;    // D&D started

	m_bSource = TRUE;      // We start D&D on the source
	m_bTarget = FALSE;     // Not yet known 

	m_nItem = listCtrl.GetSelectedCount();    // Used for counting number of selected items

	// Allocate an array for saving item indexes
	m_pSaveIndItem = new int[m_nItem];

	// Create global memory for sharing dragdrop data, we allocate
	// space for 2 int and nItem ListInfo structures
	HGLOBAL hgData = GlobalAlloc(GPTR,m_nItem*sizeof(ListInfo)+2*sizeof(int));   

	ASSERT(hgData!=NULL);
	char* p = (char*) GlobalLock(hgData);
	ASSERT(p!=NULL);

	// Place control type in first 2 bytes
	*(int *) (p) = CT_LIST;

	// Place number of list items in next 2 bytes
	*(int *) (p+sizeof(int)) = m_nItem;

	//Get imageList
	CImageList* pImageList = listCtrl.GetImageList(0);

	//  Then the nItem ListInfo structures will start at:
	ListInfo* lpData = (ListInfo*) (p + 2*sizeof(int));

	CCSSeqRefinerDoc* pDoc = GetDocument();


	int jItem = -1;
	int i = 0;
	while ((jItem = listCtrl.GetNextItem(jItem,LVNI_ALL | LVNI_SELECTED)) > -1)
	{
		ASSERT(i>=0 && i <m_nItem);
		ZeroMemory(&lpData[i], sizeof(ListInfo));

		lpData[i].lvi.mask       = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
		lpData[i].lvi.stateMask  = LVIS_DROPHILITED | LVIS_FOCUSED | LVIS_SELECTED;
		lpData[i].lvi.pszText    = (wchar_t*) &(lpData[i].szLabel);
		lpData[i].lvi.iItem      = jItem;
		lpData[i].lvi.cchTextMax = 255;

		
		lpData[i].w = pDoc->GetImageWidth();
		lpData[i].h = pDoc->GetImageHeight();

		//////////////////////////////////////////////////////////////////////////
		
		pDoc->GetImage( jItem, lpData[i].image.second, lpData[i].image.first );

		//////////////////////////////////////////////////////////////////////////
		

		BOOL bSuccess = listCtrl.GetItem(&(lpData[i].lvi));
		ASSERT(bSuccess);

		m_pSaveIndItem[i] = jItem;
		i++;
	}

	// Cache the data, and initiate DragDrop
	mOleDataSource.CacheGlobalData(CF_TEXT, hgData);           

	// Set drag/drop source rectangle (screen coord)
	SetDragSourceRect();

	DROPEFFECT dropEffect =
		mOleDataSource.DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE,NULL);  


	if (m_bRemove && (dropEffect&DROPEFFECT_MOVE)==DROPEFFECT_MOVE)     
	{
		//  Delete items in reverse order so that indexes are preserved
		for (i=m_nItem-1; i>=0; i--)
		{
			jItem = m_pSaveIndItem[i];	
			// If the list is source and target and..
			// Index of dragged item will change if dropped above itself
			if (m_bSource && m_bTarget)
			{
				ASSERT(m_iDropItem>=0);
				if (jItem > m_iDropItem) jItem += m_nItem;
			}
			BOOL bSuccess = listCtrl.DeleteItem(jItem);
			pDoc->RemoveImage( jItem );
			ASSERT(bSuccess);
		}
		// Set the list box selected item to the previous item
		//SetCurSel(nItem-1);                                            
	}
	m_bSource = FALSE;
	m_bTarget = FALSE;
	m_iDropItem = -1;
	delete[] m_pSaveIndItem;
	m_pSaveIndItem = NULL;
	m_nItem = 0;
	*pResult = 0;
}

int CCSSeqRefinerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bSuccess = mOleListDropTarget.Register(this);

	// TODO:  Add your specialized creation code here
	return 0;
}


//----------------------------------------------------------------------------------
// int CCSSeqRefinerView::HighlightDropTarget(CPoint point)
//
// Highlights the item on which the cursor is over.
//----------------------------------------------------------------------------------
int CCSSeqRefinerView::HighlightDropTarget(CPoint point)
{
	CListCtrl& listCtrl = GetListCtrl();

	// Find out which item (if any) the cursor is over.
	UINT nFlags;
	int iItem = listCtrl.HitTest (point, &nFlags);
	if (iItem == -1)
		return iItem;

	//  Highlight the item, or unhighlight all items
	//  if the cursor isn't over an item.
	SelectDropTarget (iItem);

	// Return the index of the highlighted item.
	return iItem;
}


void CCSSeqRefinerView::RemoveHighlightFromDropTarget()
{
	CListCtrl& listCtrl = GetListCtrl();

	if (m_iDropItem >= 0)
	{
		BOOL bSuccess = listCtrl.SetItemState (m_iDropItem, 0, LVIS_DROPHILITED);
		//ASSERT(bSuccess);
		/**
		* Redraw item
		**/
		listCtrl.RedrawItems (m_iDropItem, m_iDropItem);
	}
}

//----------------------------------------------------------------------------------
// void CCSSeqRefinerView::SelectDropTarget(int iItem)
// 
// if iItem >= 0, updates the highlighted item, else if iItem<0, unlights all items 
//----------------------------------------------------------------------------------
void CCSSeqRefinerView::SelectDropTarget(int iItem)
{
	CListCtrl& listCtrl = GetListCtrl();


	BOOL bSuccess;
	if (iItem >=0)
	{
		RemoveHighlightFromDropTarget();

		// Highlight the new one
		bSuccess = listCtrl.SetItemState(iItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
		ASSERT(bSuccess);
		listCtrl.RedrawItems (iItem, iItem);
		m_iDropItem = iItem;
		listCtrl.UpdateWindow ();
	}
	else
	{
		for (int i=0; i < listCtrl.GetItemCount(); i++)
		{
			bSuccess = listCtrl.SetItemState (i, 0, LVIS_DROPHILITED);
			ASSERT(bSuccess);
		}
		listCtrl.UpdateWindow ();
	}
}

CRect CCSSeqRefinerView::GetDragSourceRect()
{   
	//  return the drag/drop source rect (maintained in view class)
	return m_DragSourceRect;
}

void CCSSeqRefinerView::SetDragSourceRect()
{                                                            
	CRect ListWindowRect;

	GetWindowRect(&ListWindowRect);

	// Set drag/drop source rectangle (screen coord)
	// *(maintained in the view class)
	m_DragSourceRect = ListWindowRect;    
}

void CCSSeqRefinerView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	CCSSeqRefinerDoc* pDocument = GetDocument();

	std::wstring statusText = boost::str( boost::wformat( L"Frames: %d" ) % pDocument->GetImageCount() );
	pMainFrame->SetMessageText( statusText.c_str() );
}


/////////////////////////////////////////////////////////////////////////////
// COleListCtrlDropTarget - Enable OLE dragdrop for the ListCtrl control

COleListCtrlDropTarget::COleListCtrlDropTarget() {}
COleListCtrlDropTarget::~COleListCtrlDropTarget() {}

DROPEFFECT COleListCtrlDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* 
											   pDataObject, DWORD dwKeyState, CPoint point )
{
	CCSSeqRefinerView* pDDList = (CCSSeqRefinerView*) pWnd;

	// If local Drag&Drop not allowed and point is inside of the
	// source list ctrl, don't allow a drop
	if (!(pDDList->m_bLocalDD) && pDDList->m_bSource)
	{
		CPoint pt = point;
		pWnd->ClientToScreen(&pt); // Convert client point to screen point
		if (pDDList->GetDragSourceRect().PtInRect(pt))
			return DROPEFFECT_NONE;
	}

	// Check if the Control key was pressed
	if ((dwKeyState&MK_CONTROL) == MK_CONTROL)
		return DROPEFFECT_COPY; // Copy source text
	else
		return DROPEFFECT_MOVE; // Move source text   
}

void COleListCtrlDropTarget::OnDragLeave(CWnd* pWnd)
{
	CCSSeqRefinerView* pDDList = (CCSSeqRefinerView*) pWnd;
	pDDList->RemoveHighlightFromDropTarget();

	// Call base class implementation
	COleDropTarget:: OnDragLeave(pWnd);
}

DROPEFFECT COleListCtrlDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* 
											  pDataObject, DWORD dwKeyState, CPoint point )
{                              

	CCSSeqRefinerView* pDDList = (CCSSeqRefinerView*) pWnd;
	CListCtrl& listCtrl = pDDList->GetListCtrl();


	pDDList->KillTimer (1);

	// If local Drag & Drop not allowed and drop point is inside 
	// of the source list ctrl, don't allow a drop.
	if (!(pDDList->m_bLocalDD) && pDDList->m_bSource)
	{
		CPoint pt = point;
		pWnd->ClientToScreen(&pt);   // convert client to screen 
		if (pDDList->GetDragSourceRect().PtInRect(pt))
			return DROPEFFECT_NONE;
	}

	if (pDDList->m_bScrolling)
	{
		// Stop the scroll timer if it's running.
		pDDList->KillTimer (1);
	}
	// Highlight the drop target if the cursor is over an item.
	int iItem = pDDList->HighlightDropTarget (point);

	if (pDDList->m_bScrolling)
	{
		// Set a timer if the cursor is at the top or 
		// bottom of the window.
		CRect rect;
		pDDList->GetClientRect (rect);
		int cy = rect.Height ();

		if (( point.y <= pDDList->m_nScrollMargin) ||
			(point.y >= cy - pDDList->m_nScrollMargin))
		{
			TRACE("\nTimer set for scrolling");
			pDDList->SetTimer (1, pDDList->m_nDelayInterval, NULL);
		}
	}
	if ((dwKeyState&MK_CONTROL) == MK_CONTROL)
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_MOVE;    
}


BOOL COleListCtrlDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, 
									DROPEFFECT dropEffect, CPoint point )
{           
	HGLOBAL   hGlobal;

	TRACE("\nOnDrop");

	CCSSeqRefinerView* pDDList = (CCSSeqRefinerView*) pWnd;
	CListCtrl& listCtrl = pDDList->GetListCtrl();

	// Stop the scroll timer if it's running.
	pDDList->KillTimer (1);

	// Flag the list as target
	pDDList->m_bTarget = TRUE;

	// Find out the drop item
	int iItem;
	if (listCtrl.GetItemCount() == 0)
		iItem = 0;  // List is empty
	else
	{
		// Find drop element in list
		UINT nFlags;
		point.y += 5;
		iItem = listCtrl.HitTest (point, &nFlags);
		if (iItem == -1)
		{
			// Get coordinates of top and bottom visible points
			int iTopItem = listCtrl.GetTopIndex();
			int iBotItem = iTopItem + listCtrl.GetCountPerPage() -1;
			if (iBotItem > listCtrl.GetItemCount()-1)
				iBotItem = listCtrl.GetItemCount() - 1;
			CPoint topPoint, botPoint;
			BOOL bSuccess = listCtrl.GetItemPosition(iTopItem, &topPoint);
			ASSERT(bSuccess);
			bSuccess = listCtrl.GetItemPosition(iBotItem, &botPoint);
			ASSERT(bSuccess);

			if (point.y <= topPoint.y+5)
				iItem = 0;
			else if (point.y >botPoint.y)
				iItem = listCtrl.GetItemCount(); // Add at the end
			else
			{
				// If there was an error adding the text to the control
				// return FALSE so that the text is not removed from
				// the drop source
				return FALSE;
			}

		}
	}
	pDDList->m_iDropItem = iItem;
	ASSERT(pDDList->m_iDropItem>=0);

	if (pDDList->m_bSource && pDDList->m_bTarget)
	{	
		// Check that we don't drop on dragged items
		TRACE("\niItem=%d pSaveInItem[0]=%d pSaveInItem[0]=%d",
			iItem,pDDList->m_pSaveIndItem[0],pDDList->m_pSaveIndItem[pDDList->m_nItem-1]); 
		if (iItem >= pDDList->m_pSaveIndItem[0] &&
			iItem <= pDDList->m_pSaveIndItem[pDDList->m_nItem-1])
		{
			// Display an error message if the move is illegal.
			AfxMessageBox (_T ("An item can't be dropped onto itself"));
			pDDList->SelectDropTarget(-1);
			// If there was an error adding the text to the control
			// return FALSE so that the text is not removed from
			// the drop source
			return FALSE;
		}
	}

	// Get List data from COleDataObject
	hGlobal = pDataObject->GetGlobalData(CF_TEXT);

	// Get pointer to data
	PVOID p = GlobalLock(hGlobal);    
	ASSERT(p!=NULL);

	CCSSeqRefinerDoc* pDoc = pDDList->GetDocument();

	int imageCount = pDoc->GetImageCount();

	// Get the type of source control
	int ctrlType = *(int *) (p);
	if (ctrlType == CT_LIST)
	{
		// Source Ctrl is a list, get number of items
		int nItem = *(int *) ((char*)p + sizeof(int));
		ListInfo* pData = (ListInfo*) ((char*)p + 2*sizeof(int));

		if ( !pDoc->SetImageWidth( pData[0].w ) || !pDoc->SetImageHeight( pData[0].h ))
		{
			AfxMessageBox( L"Not right dimensions" );
			return FALSE;
		}

		int imageWidth = pDoc->GetImageWidth();
		int imageHeight = pDoc->GetImageHeight();

		if ( pDDList->m_pDocImageList.get() == 0 )
		{
			pDDList->m_pDocImageList = boost::shared_ptr< CImageList >( new CImageList() );
			pDDList->m_pDocImageList->Create( imageWidth, imageHeight, ILC_COLOR32, imageCount,16 );
			listCtrl.SetImageList( pDDList->m_pDocImageList.get(),  LVSIL_NORMAL );
		}

		int imageByteSize = imageWidth * imageHeight * 4;

		CBitmap bmp;

		bmp.CreateBitmap( imageWidth, imageHeight, 1, 32, NULL );
		boost::scoped_array< BYTE > buffer( new BYTE[ imageByteSize ] );


		// Add the item(s) to the list
		int iNewItem;
		for (int i=0; i<nItem; i++)
		{
			//////////////////////////////////////////////////////////////////////////
			pDoc->AddImage( pData[i].image.second, pData[i].image.first );

			for( int j = 0, k = 0; j < imageByteSize ; j+=4, ++k )
			{
				buffer[j+0] = pData[i].image.first[k];
				buffer[j+1] = pData[i].image.first[k];
				buffer[j+2] = pData[i].image.first[k];
			}

			bmp.SetBitmapBits( imageByteSize, buffer.get() );
			pData[i].lvi.iImage = pDDList->m_pDocImageList->Add( &bmp, (CBitmap*)(0) );;
       			

			//////////////////////////////////////////////////////////////////////////
			pData[i].lvi.iItem = iItem;
			iItem++;
			iNewItem = listCtrl.InsertItem( &(pData[i].lvi));
			ASSERT(iNewItem >= 0);
		}

		// Unlight all list items
		pDDList->SelectDropTarget(-1);

		// Select the newly added item.
		BOOL bSuccess = listCtrl.SetItemState(iNewItem, LVIS_SELECTED, LVIS_SELECTED);
	}
	else
	{
		AfxMessageBox( L"List -- Source control not defined");
	}

	// Unlock memory
	GlobalUnlock(hGlobal);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

void CCSSeqRefinerView::OnSize(UINT nType, int cx, int cy)
{
	CListView::OnSize(nType, cx, cy);

	CListCtrl& listCtrl = GetListCtrl();
	listCtrl.Arrange( LVA_DEFAULT );
}
