// ==========================================================================
// 							   Class Implementation : 
//									COXItemTip
// ==========================================================================

// Copyright © Dundas Software Ltd. 1997 - 1998, All Rights Reserved
                          
// //////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OXItemTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(COXItemTip, COXHookWnd);

// Constructor
COXItemTip::COXItemTip()
{
	m_pListCtrl=NULL;
	m_pfnCallback=NULL;

	Reset();
}

// Destructor
COXItemTip::~COXItemTip()
{
	if(::IsWindow(m_ItemTipWnd.m_hWnd))
	{
		m_ItemTipWnd.DestroyWindow();
	}
}

void COXItemTip::Reset()
{
	m_pFont=NULL;
	m_clrText=ID_OX_COLOR_NONE;
	m_clrBackground=ID_OX_COLOR_NONE;
	m_nOffset=ID_ITEMTIP_XOFFSET;
}

// Attach ListCtrl handler.
BOOL COXItemTip::Attach(CWnd* pWnd, NEEDITEMTIP_HELPER pfnCallback)
{
	ASSERT(::IsWindow(pWnd->m_hWnd));
	ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CListCtrl)));

	HookWindow(pWnd);

	m_pListCtrl=DYNAMIC_DOWNCAST(CListCtrl,pWnd);
	ASSERT(m_pListCtrl);

	m_pfnCallback=pfnCallback;

	if(!::IsWindow(m_ItemTipWnd.GetSafeHwnd()))
	{
		m_ItemTipWnd.Create(pWnd);
	}

	return TRUE;
}

// Detach ListCtrl handler. Called by default when hooked window is about
// to be destroyed.
void COXItemTip::Detach()
{
	UnhookWindow();

	m_pListCtrl=NULL;
	m_pfnCallback=NULL;
}

////////////////////////////////////////////////////////////////////////////
// Message handler handles ItemTip-related messages
//
LRESULT COXItemTip::WindowProc(UINT msg, WPARAM wp, LPARAM lp)
{
#ifdef _AFXDLL
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#endif

	ASSERT(::IsWindow(m_pWndHooked->m_hWnd));
	ASSERT(m_pWndHooked->IsKindOf(RUNTIME_CLASS(CListCtrl)));
	ASSERT(m_pWndHooked->m_hWnd==m_pListCtrl->m_hWnd);

	switch (msg) 
	{
	case WM_MOUSEMOVE: 
		{
			if(wp==0)
			{
				POINTS points=MAKEPOINTS(lp);	
				CPoint point(points.x,points.y);

				NEEDITEMTIPINFO nitInfo;
				nitInfo.point.x=point.x;
				nitInfo.point.y=point.y;
				nitInfo.row=-1;
				nitInfo.col=-1;
				nitInfo.rectItem.left=0;
				nitInfo.rectItem.right=0;
				nitInfo.rectItem.top=0;
				nitInfo.rectItem.bottom=0;
				nitInfo.sizeText=MAX_ITEMTIP*sizeof(TCHAR);
				nitInfo.offset=m_nOffset;
				nitInfo.pFont=m_pFont==NULL ? 
					(LPARAM)m_pWndHooked->GetFont() : (LPARAM)m_pFont;
				nitInfo.clrText=m_clrText==ID_OX_COLOR_NONE ? 
					m_pListCtrl->GetTextColor() : m_clrText;
				nitInfo.clrBackground=m_clrBackground;
				nitInfo.alignment=DT_LEFT;
				nitInfo.result=ID_NIT_SUCCESS;

				NeedItemTip(&nitInfo);

				if(m_pfnCallback==NULL)
				{
					m_pWndHooked->
						SendMessage(WM_DOYOUNEEDITEMTIP,(WPARAM)0,(LPARAM)&nitInfo);
				}
				else
				{
					m_pfnCallback((LPARAM)this,(LPARAM)&nitInfo);
				}

				if(nitInfo.result==ID_NIT_SUCCESS)
				{
					CString sText=(LPCTSTR)nitInfo.itemText;
					CRect rect=nitInfo.rectItem;
					m_ItemTipWnd.Display(rect,sText,nitInfo.offset,
						nitInfo.alignment,(CFont*)nitInfo.pFont,
						nitInfo.clrText,nitInfo.clrBackground);
				}
			}
		}
	}

	// I don't handle it: pass along
	return COXHookWnd::WindowProc(msg, wp, lp);
}

void COXItemTip::NeedItemTip(LPNEEDITEMTIPINFO pnitInfo)
{
	ASSERT(::IsWindow(m_pWndHooked->m_hWnd));
	ASSERT(m_pWndHooked->m_hWnd==m_pListCtrl->m_hWnd);

	// Make sure that the ListCtrl is in report view
	if((m_pListCtrl->GetStyle()&LVS_TYPEMASK)!=LVS_REPORT)
	{
		pnitInfo->result=ID_NIT_WRONGFORMAT;
		return;
	}

	CPoint point;
	point.x=pnitInfo->point.x;
	point.y=pnitInfo->point.y;

	CRect rectItem=pnitInfo->rectItem;

	// Get the top and bottom row visible
	int nTopRow=m_pListCtrl->GetTopIndex();
	int nBottomRow=nTopRow+m_pListCtrl->GetCountPerPage();
	if(nBottomRow>m_pListCtrl->GetItemCount())
	{
		nBottomRow=m_pListCtrl->GetItemCount();
	}

	int nRow=nTopRow;
	while(TRUE)
	{
		m_pListCtrl->GetItemRect(nRow,&rectItem,LVIR_BOUNDS);
		if(rectItem.PtInRect(point))
		{
			break;
		}
		nRow++;

		if(nRow>nBottomRow)
		{
			pnitInfo->result=ID_NIT_OUTOFCONTROLBORDER;
			return;
		}
	}
	pnitInfo->row=nRow;

	int nCol=0;
	int nColReal=0;
	while(TRUE)
	{
		nColReal=GetRealColumn(nCol);
		int nColWidth=m_pListCtrl->GetColumnWidth(nColReal);
		if(nColWidth<0)
		{
			pnitInfo->result=ID_NIT_OUTOFCONTROLBORDER;
			return;
		}

		rectItem.right=rectItem.left+nColWidth;
		if(rectItem.PtInRect(point))
		{
			break;
		}
		rectItem.left=rectItem.right;
		nCol++;
	}
	pnitInfo->col=nColReal;

	if(nColReal==0)
	{
		CRect rcLabel;
		m_pListCtrl->GetItemRect(nRow, &rcLabel, LVIR_LABEL);

		rectItem=rcLabel;
		rectItem.InflateRect(1,0);
		if(!rectItem.PtInRect(point))
		{
			pnitInfo->result=ID_NIT_OUTOFITEMBORDER;
			return;
		}
	}

	int nOffset=(nColReal==0 ? pnitInfo->offset/2 : pnitInfo->offset);
	rectItem.DeflateRect(nOffset,0);

	CString sText=m_pListCtrl->GetItemText(nRow,nColReal);
	if(sText.IsEmpty())
	{
		pnitInfo->result=ID_NIT_NOTHIDDEN;
		return;
	}
	lstrcpyn(pnitInfo->itemText,sText,pnitInfo->sizeText);

	CClientDC dc(m_pListCtrl);
	CFont* pOldFont=pnitInfo->pFont==NULL ? NULL : 
		dc.SelectObject((CFont*)pnitInfo->pFont);
	CRect rectText;
	dc.DrawText(sText,&rectText,DT_CALCRECT|DT_LEFT|DT_SINGLELINE);
	if(pOldFont)
	{
		dc.SelectObject(pOldFont);
	}

	CRect rectClient;
	m_pListCtrl->GetClientRect(&rectClient);

	LV_COLUMN lvc;
	memset(&lvc,0,sizeof(LV_COLUMN));
	lvc.mask=LVCF_FMT;
	VERIFY(m_pListCtrl->GetColumn(nColReal, &lvc));

	int fmt=lvc.fmt&LVCFMT_JUSTIFYMASK;
	int nTextWidth=rectText.Width();
	switch(fmt)
	{
	case LVCFMT_LEFT:
		{
			rectText.left=rectItem.left;
			rectText.right=rectText.left+nTextWidth;
			pnitInfo->alignment=DT_LEFT;
			break;
		}
	case LVCFMT_RIGHT:
		{
			rectText.right=rectItem.right;
			rectText.left=rectText.right-nTextWidth;
			pnitInfo->alignment=DT_RIGHT;
			break;
		}
	case LVCFMT_CENTER:
		{
			int nCenter=rectItem.left+(rectItem.right-rectItem.left)/2;
			rectText.left=nCenter-nTextWidth/2;
			rectText.right=nCenter+nTextWidth/2;
			pnitInfo->alignment=DT_CENTER;
			break;
		}
	default:
		ASSERT(FALSE);
	}

	CRect rectItemWindow=rectItem;
	m_pListCtrl->ClientToScreen(rectItemWindow);
	BOOL bTopMostParent=(m_pListCtrl->GetTopLevelParent()!=NULL &&
		m_pListCtrl->GetTopLevelParent()->GetExStyle()&WS_EX_TOPMOST);
	// check if item rectangle fits into the screen
	CRect rectDisplay;
	if(bTopMostParent)
		CWnd::GetDesktopWindow()->GetWindowRect(rectDisplay);
	else
		::SystemParametersInfo(SPI_GETWORKAREA,NULL,&rectDisplay,NULL);

	if(rectText.Width()<=rectItem.Width() && 
		rectText.right<=rectClient.right && 
		rectText.left>=rectClient.left &&
		rectItemWindow.right<=rectDisplay.right &&
		rectItemWindow.left>=rectDisplay.left)
	{
		pnitInfo->result=ID_NIT_NOTHIDDEN;
		return;
	}

	if(rectItem.Width()<rectText.Width())
	{
		rectItem.right=rectItem.left+rectText.Width();
	}
	rectItem.InflateRect(nOffset,0);

	pnitInfo->rectItem.left=rectItem.left;
	pnitInfo->rectItem.right=rectItem.left+__min(rectItem.Width(),rectText.Width());
	pnitInfo->rectItem.top=rectItem.top;
	pnitInfo->rectItem.bottom=rectItem.bottom;

	pnitInfo->offset=nOffset;

	pnitInfo->result=ID_NIT_SUCCESS;
}


int COXItemTip::GetRealColumn(int nCol)
{
#if (_WIN32_IE >= 0x0300)
	ASSERT(::IsWindow(m_pWndHooked->m_hWnd));
	ASSERT(m_pWndHooked->IsKindOf(RUNTIME_CLASS(CListCtrl)));
	ASSERT(m_pWndHooked->m_hWnd==m_pListCtrl->m_hWnd);

    // Get the header control 
	CHeaderCtrl* pHeader = (CHeaderCtrl*)m_pListCtrl->GetDlgItem(0);
    ASSERT(pHeader);

    // get the current number of columns 
    int nCount=pHeader->GetItemCount();
	ASSERT(nCol<nCount);

    // find the real column number. We will request new HDI_ORDER info
    for (int nIndex=0; nIndex<nCount; nIndex++)
    {
        HD_ITEM hdItem;
        hdItem.mask=HDI_ORDER;
        BOOL bReturn = pHeader->GetItem(nIndex,&hdItem);
        ASSERT(bReturn);

        if(hdItem.iOrder==nCol)
        {
            return nIndex;
        }
    }

	// we shouldn't be here
    ASSERT(FALSE);

    return -1;
#else
	return nCol;
#endif
}

