/*********************************************************
* Simple IE-like Menu And Toolbar
* Version: 1.3
* Date: Semptember 1, 2003
* Autor: Michal Mecinski
* E-mail: mimec@mimec.w.pl
* WWW: http://www.mimec.w.pl
*
* Copyright (C) 2002-03 by Michal Mecinski
*********************************************************/

#include "stdafx.h"
#include "MenuBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// size of menu icons
#define MB_CX_ICON	16
#define MB_CY_ICON	16

#define max_stub(a,b)            (((a) > (b)) ? (a) : (b))

CMenuBar::CMenuBar()
{
	m_pMenu = NULL;
	m_pPopupMenu = NULL;
	m_pPrevFocus = NULL;
	m_nItemCount = 0;
	m_nPressed = -1;
	m_bPrefix = TRUE;
	m_bKeyboard = FALSE;
	m_bWndActive = TRUE;
	m_bPopMenu = FALSE;
}

CMenuBar::~CMenuBar()
{
	delete m_pMenu;
	delete m_pPopupMenu;
}

BEGIN_MESSAGE_MAP(CMenuBar, CToolBarCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_INITMENUPOPUP()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_WM_ENTERIDLE()
	ON_WM_MENUSELECT()
	ON_MESSAGE(MBM_POPUPMENU, OnPopupMenu)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMenuBar public functions

BOOL CMenuBar::Create(CWnd* pParentWnd, UINT nID)
{
	return CToolBarCtrl::Create(WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_LIST | CCS_NODIVIDER | CCS_NORESIZE,
		CRect(0, 0, 0, 0), pParentWnd, nID);
}


BOOL CMenuBar::LoadMenuBar(UINT nID, int nStyle)
{
	if (m_pMenu)
		m_pMenu->DestroyMenu();
	else
		m_pMenu = new CMenu;

	// load menu resource
	if (!m_pMenu->LoadMenu(nID))
		return FALSE;

	// create image list
	if (!m_ImgList.Create(MB_CX_ICON, MB_CY_ICON, nStyle))
		return FALSE;

	m_CmdArray.RemoveAll();

	// delete previous buttons if any
	while (m_nItemCount--)
		DeleteButton(0);

	m_nItemCount = m_pMenu->GetMenuItemCount();

	SetBitmapSize(CSize(1, 1));

	TBBUTTON tbb; 
	ZeroMemory(&tbb, sizeof(tbb));

	tbb.fsState = TBSTATE_ENABLED;
	tbb.fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;

	// add button for each menu
	for(int i=0; i<m_nItemCount; i++)
	{
		CString strText;
		m_pMenu->GetMenuString(i, strText, MF_BYPOSITION);

		tbb.iString = AddStrings(strText);
		tbb.idCommand = i;

		if (!AddButtons(1, &tbb))
			return FALSE;
	}

	// get system settings and resize toolbar
	UpdateSettings();

	return TRUE;
}

BOOL CMenuBar::AttachMenu(HMENU hMenu)
{
	if (m_pMenu)
		m_pMenu->Detach();
	else
		m_pMenu = new CMenu;

	m_pMenu->Attach(hMenu);

	while (m_nItemCount--)
		DeleteButton(0);

	m_nItemCount = m_pMenu->GetMenuItemCount();

	SetBitmapSize(CSize(1, 1));

	TBBUTTON tbb; 
	ZeroMemory(&tbb, sizeof(tbb));

	tbb.fsState = TBSTATE_ENABLED;
	tbb.fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;

	// add button for each menu
	for(int i=0; i<m_nItemCount; i++)
	{
		CString strText;
		m_pMenu->GetMenuString(i, strText, MF_BYPOSITION);

		tbb.iString = AddStrings(strText);
		tbb.idCommand = i;

		if (!AddButtons(1, &tbb))
			return FALSE;
	}

	UpdateSettings();

	return TRUE;
}

BOOL CMenuBar::LoadToolBar(UINT nID)
{
	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(nID), RT_TOOLBAR);
	HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nID), RT_TOOLBAR);
	if (hRsrc == NULL)
		return FALSE;

	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	if (hGlobal == NULL)
		return FALSE;

	struct TBDATA
	{
		WORD wVersion;
		WORD wWidth;
		WORD wHeight;
		WORD wItemCount;
		WORD aItems[1];
	};

	// get the toolbar data
	TBDATA* pData = (TBDATA*)LockResource(hGlobal);
	if (pData == NULL)
		return FALSE;
	ASSERT(pData->wVersion == 1);

	// add command IDs to the array
	for (int i=0; i < pData->wItemCount; i++)
	{
		int nCmd = pData->aItems[i];
		if (nCmd != 0)	// ignore separators
			m_CmdArray.Add(nCmd);
	}

	// load bitmap image
	BOOL bResult = m_ImgList.AddBitmap(nID);

	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	return bResult;
}

HMENU CMenuBar::GetMenuHandle()
{
	return m_pMenu ? m_pMenu->m_hMenu : NULL;
}


BOOL CMenuBar::LoadPopupMenu(UINT nID)
{
	if (m_pPopupMenu)
		m_pPopupMenu->DestroyMenu();
	else
		m_pPopupMenu = new CMenu;

	return m_pPopupMenu->LoadMenu(nID);
}

void CMenuBar::TrackPopup(int nIndex, CPoint ptPos)
{
	// use window menu if popup menu is not loaded
	CMenu* pSubMenu = m_pPopupMenu ? m_pPopupMenu->GetSubMenu(nIndex) : m_pMenu->GetSubMenu(nIndex);

	m_bPopMenu = m_pPopupMenu!=NULL;

	if (pSubMenu)
		pSubMenu->TrackPopupMenu(0, ptPos.x, ptPos.y, this);
}

void CMenuBar::TrackPopup(LPCTSTR lpszName, CPoint ptPos)
{
	CMenu* pMenu = m_pPopupMenu ? m_pPopupMenu : m_pMenu;

	for (UINT i=0; i<pMenu->GetMenuItemCount(); i++)
	{
		CString str;
		pMenu->GetMenuString(i, str, MF_BYPOSITION);
		if (str.Compare(lpszName) == 0)
		{
			TrackPopup(i, ptPos);
			return;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMenuBar input filter

static CMenuBar* g_pMenuBar	= NULL;
static HHOOK g_hMsgHook	= NULL;

LRESULT CMenuBar::MenuInputFilter(int nCode, WPARAM wParam, LPARAM lParam)
{
	MSG* pMsg = (MSG*)lParam;

	if (g_pMenuBar && nCode == MSGF_MENU)
	{
		// process menu message
		if (g_pMenuBar->OnMenuInput(pMsg))
			return TRUE;
	}

	return CallNextHookEx(g_hMsgHook, nCode, wParam, lParam);
}

BOOL CMenuBar::OnMenuInput(MSG* pMsg)
{
	BOOL bResult = FALSE;
	UNREFERENCED_PARAMETER(bResult);
	switch(pMsg->message)
	{
	case WM_MOUSEMOVE:
		{
			CPoint pt;
			pt.x = LOWORD(pMsg->lParam);
			pt.y = HIWORD(pMsg->lParam);
			ScreenToClient(&pt);

			if (pt == m_ptLast)	// ignore if mouse not moved
				return TRUE;

			m_ptLast = pt;

			int nTest = HitTest(&pt);

			// check if another menu was highlighted
			if (nTest>=0 && nTest<m_nItemCount && nTest != m_nPressed)
			{
				PressButton(m_nPressed, FALSE);
				m_bKeepPrefix = TRUE;
				m_bNextMenu = TRUE;
				SendMessage(WM_CANCELMODE);	// close current menu
				m_nPressed = nTest;
				PostMessage(MBM_POPUPMENU);	// open highlighted menu
				return TRUE;
			}
		}
		break;

	case WM_LBUTTONDOWN:
		{
			POINT pt;
			pt.x = LOWORD(pMsg->lParam);
			pt.y = HIWORD(pMsg->lParam);
			ScreenToClient(&pt);

			int nTest = HitTest(&pt);

			if (nTest == m_nPressed)	// close menu if clicked again
			{
				m_bKeepPrefix = TRUE;
				m_bNextMenu = TRUE;
				SendMessage(WM_CANCELMODE,0,0);
				return TRUE;
			}
		}
		break;

	case WM_KEYDOWN:
		{
			switch (pMsg->wParam)
			{
			case VK_LEFT:
				if (InSubMenu())	// default action - close submenu
					return FALSE;
				PressButton(m_nPressed, FALSE);
				m_bKeepPrefix = TRUE;
				m_bNextMenu = TRUE;
				SendMessage(WM_CANCELMODE);
				m_nPressed--;		// open previous menu
				m_bKeyboard = TRUE;
				PostMessage(MBM_POPUPMENU);
				return TRUE;

			case VK_RIGHT:
				if (m_bSelPopup)	// default action - open submenu
					return FALSE;
				PressButton(m_nPressed,FALSE);
				m_bKeepPrefix = TRUE;
				m_bNextMenu = TRUE;
				SendMessage(WM_CANCELMODE);
				m_nPressed++;		// open next menu
				m_bKeyboard = TRUE;
				PostMessage(MBM_POPUPMENU);
				return TRUE;

			case VK_ESCAPE:
				if (InSubMenu())	// default action - close submenu
					return FALSE;
				m_ptLast = CPoint(-1,-1);
				m_bKeepPrefix = TRUE;
				SendMessage(WM_CANCELMODE);
				Highlight(m_nPressed);	// activate the menu bar
				return TRUE;
			}
		}
		break;

	case WM_MENUSELECT:
		{
			// store info about selected item
			m_hSelMenu = (HMENU)pMsg->lParam;
			m_bSelPopup = (HIWORD(pMsg->wParam) & MF_POPUP) != 0;
			// notify main frame
			if (HIWORD(pMsg->wParam) != 0xFFFF)
				GetParentFrame()->SendMessage(WM_MENUSELECT, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

BOOL CMenuBar::InSubMenu()
{
	for (int i=0; i<m_nItemCount; i++)
	{
		// check if current menu is one of top level menus
		if (m_hSelMenu == m_pMenu->GetSubMenu(i)->m_hMenu)
			return FALSE;
	}

	// otherwise it's a submenu
	return TRUE;
}

void CMenuBar::Highlight(int nItem)
{
	SetHotItem(nItem);	// hihglight item

	if (nItem < 0)
	{
		// restore focus
		if (m_pPrevFocus && IsWindow(m_pPrevFocus->m_hWnd))
			m_pPrevFocus->SetFocus();
		m_pPrevFocus = NULL;
	}
	else
	{
		// steal focus
		m_pPrevFocus = SetFocus();
	}
}

void CMenuBar::UpdateSettings()
{
	m_bCues = TRUE;
	m_bFlatMenu = FALSE;
#if (_WIN32_WINNT >= 0x0501) /*|| (WINVER >= 0x0501)*/
	SystemParametersInfo(SPI_GETKEYBOARDCUES, 0, &m_bCues, 0);
	SystemParametersInfo(SPI_GETFLATMENU, 0, &m_bFlatMenu, 0);
#endif
	SetPrefix(m_bCues);

	// create menu font
	NONCLIENTMETRICS ncmet;
	ncmet.cbSize = sizeof(ncmet);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncmet), &ncmet, 0);

	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&ncmet.lfMenuFont);

	SetFont(&m_Font);	// use it to draw toolbar buttons too

	// get last item position to measure bar size
	CRect rcItem;
	GetItemRect(m_nItemCount-1, &rcItem);

	CRect rcRect(0, 0, rcItem.right, rcItem.bottom);

	if (GetParent() == GetParentFrame())
	{
		// not in a rebar yet, just resize the toolbar
		MoveWindow(&rcRect, FALSE);
	}
	else
	{
		// update band information
		REBARBANDINFO rbbi;
		ZeroMemory(&rbbi, sizeof(rbbi));
		rbbi.cbSize = sizeof(rbbi);
		rbbi.fMask = RBBIM_CHILD;

		CReBarCtrl* pReBar = (CReBarCtrl*)GetParent();

		for (UINT i=0; i<pReBar->GetBandCount(); i++)
		{
			pReBar->GetBandInfo(i, &rbbi);

			if (rbbi.hwndChild == m_hWnd)
			{
				rbbi.fMask = RBBIM_CHILDSIZE;
		
				rbbi.cyMinChild = rbbi.cyMaxChild = rbbi.cyChild = rcRect.Height();
				rbbi.cxMinChild = rcRect.Width();

				pReBar->SetBandInfo(i, &rbbi);

				// force updating bands layout
				CRect rcClient;
				pReBar->GetClientRect(&rcClient);
				pReBar->MoveWindow(0, 0, 1, 1, FALSE);
				pReBar->MoveWindow(&rcClient);

				// recalc frame window layout
				GetParentFrame()->RecalcLayout();
		
				break;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMenuBar menu popup

LRESULT CMenuBar::OnPopupMenu(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	// correct index if necessary
	if (m_nPressed<0)
		m_nPressed = m_nItemCount - 1;
	if (m_nPressed>=m_nItemCount)
		m_nPressed = 0;

	OpenPopup();

	return 0;
}

BOOL CMenuBar::OpenMenu(UINT nChar)
{
	for (int i=0; i<m_nItemCount; i++)
	{
		CString strText;
		m_pMenu->GetMenuString(i, strText, MF_BYPOSITION);

		// check menu mnemonic
		int iAmpersand = strText.Find('&');
		if (iAmpersand >=0 && toupper(nChar)==toupper(strText[iAmpersand+1]))
		{
			// open matching menu
			m_nPressed = i;
			m_bKeyboard = TRUE;
			PostMessage(MBM_POPUPMENU);
			SetPrefix(TRUE);
			return TRUE;
		}
	}
	return FALSE;
}


void CMenuBar::OpenPopup()
{
	// if the menu bar has focus, give it back
	if (GetFocus()->m_hWnd==m_hWnd && m_pPrevFocus && IsWindow(m_pPrevFocus->m_hWnd))
	{
		BOOL bRest = m_bPrefix;	// prevent from removing prefixes
		m_pPrevFocus->SetFocus();
		SetPrefix(bRest);
		m_pPrevFocus = NULL;
	}

	m_bSelPopup = FALSE;
	m_hSelMenu = (HMENU)-1;

	m_bKeepPrefix = FALSE;
	m_bNextMenu = FALSE;

	m_bPopMenu = FALSE;

	CMenu *pSubMenu = m_pMenu->GetSubMenu(m_nPressed);

	// change the state of the menu button
	SetHotItem(-1);
	PressButton(m_nPressed, TRUE);

	// notify frame window that menu is selected
	GetParentFrame()->SendMessage(WM_MENUSELECT, MAKELONG(m_nPressed, MF_POPUP | MF_HILITE | MF_MOUSESELECT), (LPARAM)m_pMenu->m_hMenu);

	CRect rc;
	GetItemRect(m_nPressed, &rc);
	ClientToScreen(&rc);

	CRect rcScreen;
	GetDesktopWindow()->GetWindowRect(rcScreen);
	rc.IntersectRect(rc, rcScreen);

	TPMPARAMS tpm;
	tpm.cbSize = sizeof(tpm);
	tpm.rcExclude = rc;

	// install message filter
	g_pMenuBar = this;
	g_hMsgHook = SetWindowsHookEx(WH_MSGFILTER,	MenuInputFilter, NULL, GetCurrentThreadId());

	// do menu loop
	TrackPopupMenuEx(pSubMenu->GetSafeHmenu(),
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_VERTICAL | TPM_VERPOSANIMATION,
		rc.left, rc.bottom, m_hWnd, &tpm);

	// remove message filter
	UnhookWindowsHookEx(g_hMsgHook);

	g_hMsgHook = NULL;
	g_pMenuBar = NULL;

	// unpress the button
	PressButton(m_nPressed, FALSE);

	// remove prefixes if not asked to keep
	if (!m_bKeepPrefix)
		SetPrefix(FALSE);

	// notify frame window that menu is closing
	// if another menu is not about to open
	if (!m_bNextMenu)
		GetParentFrame()->SendMessage(WM_MENUSELECT, MAKELONG(m_nPressed, 0xFFFF), (LPARAM)m_pMenu->GetSafeHmenu());
}


/////////////////////////////////////////////////////////////////////////////
// CMenuBar message handlers

void CMenuBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UNREFERENCED_PARAMETER(nFlags);
	int	nTest = HitTest(&point);

	if (nTest<0 || nTest>=m_nItemCount)
		return;

	m_nPressed = nTest;
	m_bKeyboard = FALSE;

	// open menu when button is clicked
	OpenPopup();
}

void CMenuBar::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CToolBarCtrl::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// pass message to frame window to let the update handlers do their job
	GetParentFrame()->SendMessage(WM_INITMENUPOPUP, (WPARAM)pPopupMenu->GetSafeHmenu(), MAKELONG(nIndex, bSysMenu));

	MENUITEMINFO miinfo;
	ZeroMemory(&miinfo, sizeof(miinfo));
	miinfo.cbSize = sizeof(miinfo);
	miinfo.fMask = MIIM_FTYPE | MIIM_ID | MIIM_STATE;

	// set ownerdraw flag for menu items
	for (int i=0; i<(int)pPopupMenu->GetMenuItemCount(); i++)
	{
		GetMenuItemInfo(pPopupMenu->m_hMenu, i, TRUE, &miinfo);
		if (miinfo.fType & MFT_SEPARATOR)
			continue;	// use standard separators
		miinfo.fType |= MFT_OWNERDRAW;
		SetMenuItemInfo(pPopupMenu->m_hMenu, i, TRUE, &miinfo);
	}

	if (m_bKeyboard)
	{
		// a simple trick to highlight the first menu item
		PostMessage(WM_KEYDOWN, VK_DOWN, 0);
		m_bKeyboard = FALSE;
	}
}


BOOL CMenuBar::PreTranslateMessage(MSG* pMsg) 
{
	// handle key messages when the menu bar has focus
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		int nItem = GetHotItem();

		switch (pMsg->wParam)
		{
		case VK_RETURN:	// open selected menu
		case VK_UP:
		case VK_DOWN:
			m_nPressed = nItem;
			m_bKeyboard = TRUE;
			OpenPopup();
			return TRUE;

		case VK_ESCAPE:	// restore focus
			m_pPrevFocus->SetFocus();
			m_pPrevFocus = NULL;
			return TRUE;

		default:		// check mnemonic
			if (OpenMenu((UINT)pMsg->wParam))
				return TRUE;
			break;
		}
	}
	
	return CToolBarCtrl::PreTranslateMessage(pMsg);
}

void CMenuBar::OnKillFocus(CWnd* pNewWnd) 
{
	SetHotItem(-1);

	SetPrefix(FALSE);

	CToolBarCtrl::OnKillFocus(pNewWnd);
}

// pass these messages to parent frame to enable status bar updating
void CMenuBar::OnEnterIdle(UINT nWhy, CWnd* pWho) 
{
	GetParentFrame()->SendMessage(WM_ENTERIDLE, nWhy, (LPARAM)pWho->m_hWnd);
}

void CMenuBar::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu) 
{
	GetParentFrame()->SendMessage(WM_MENUSELECT, MAKELONG(nItemID, nFlags), (LPARAM)hSysMenu);
}


BOOL CMenuBar::OnKeyMenu(UINT nKey)
{
	if (nKey == VK_SPACE)
	{
		// window menu will be opened
		if (GetFocus()->m_hWnd == m_hWnd)
			Highlight(-1);
		else
			SetPrefix(FALSE);
	}
	else if (nKey == 0)
	{
		// activate or deactivate menu bar
		if (GetFocus()->m_hWnd == m_hWnd)
			Highlight(-1);
		else
		{
			SetPrefix();
			Highlight();
		}

		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMenuBar menu bar drawing

void CMenuBar::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTBCUSTOMDRAW lpNMCustomDraw = (LPNMTBCUSTOMDRAW)pNMHDR;

	switch (lpNMCustomDraw->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
			if (m_bFlatMenu || !m_bWndActive)
				*pResult = CDRF_NOTIFYITEMDRAW;
			break;

		case CDDS_ITEMPREPAINT:
			if (m_bFlatMenu)	// draw flat-style menu bar
			{
				CDC dc;
				dc.Attach(lpNMCustomDraw->nmcd.hdc);
				CRect rcRect = lpNMCustomDraw->nmcd.rc;

				int nState = lpNMCustomDraw->nmcd.uItemState;

				BOOL bHilight = nState & (CDIS_HOT | CDIS_SELECTED);

				if (bHilight)	// draw highlight rectangle
				{
					CBrush brHighlight;
					brHighlight.CreateSysColorBrush(COLOR_HIGHLIGHT);
					CBrush brMenuHilight;
					brMenuHilight.CreateSysColorBrush(COLOR_MENUHILIGHT);
					dc.FillRect(&rcRect, &brMenuHilight);
					dc.FrameRect(&rcRect, &brHighlight);
				}

				char buf[80];
				SendMessage(TB_GETBUTTONTEXT, lpNMCustomDraw->nmcd.dwItemSpec, (LPARAM)buf);

				CString strText = buf;

				// draw text
				dc.SelectObject(GetFont());
				dc.SetTextColor(GetSysColor(m_bWndActive
					? (bHilight ? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT)
					: COLOR_GRAYTEXT));
				dc.SetBkMode(TRANSPARENT);
				const UINT	c_uHidePrefix = 
#if (_WINNT_WIN32 >= 0x0501)
					(m_bPrefix ? 0 : DT_HIDEPREFIX);
#else
					0;
#endif
				dc.DrawText(strText, &rcRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE
					| c_uHidePrefix);

				dc.Detach();
				*pResult = CDRF_SKIPDEFAULT;
			}
			else if (!m_bWndActive)	// draw gray text if window is inactive
			{
				lpNMCustomDraw->clrText = GetSysColor(COLOR_GRAYTEXT);
				*pResult = CDRF_DODEFAULT;
			}
			break;
	}
}


void CMenuBar::SetPrefix(BOOL bPrefix)
{
	if (m_bPrefix == bPrefix)
		return;

	// ignore if cues are always displayed
	if (!bPrefix && m_bCues)
		return;

	m_bPrefix = bPrefix;
	RedrawWindow();
}

void CMenuBar::Activate(BOOL bActive)
{
	if (m_bWndActive == bActive)
		return;

	SetPrefix(FALSE);

	// change the color of menu text
	m_bWndActive = bActive;
	RedrawWindow();
}


/////////////////////////////////////////////////////////////////////////////
// CMenuBar menu item drawing

void CMenuBar::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	UNREFERENCED_PARAMETER(nIDCtl);
	CDC dc;
	dc.CreateCompatibleDC(NULL);

	dc.SelectObject(&m_Font);

	UINT nID = lpMeasureItemStruct->itemID;

	CMenu* pMenu = m_bPopMenu ? m_pPopupMenu : m_pMenu;

	CString strString;
	pMenu->GetMenuString(nID, strString, MF_BYCOMMAND);

	// split string to command name and keyboard shortcut
	CString strName, strKey;
	AfxExtractSubString(strName, strString, 0, '\t');
	AfxExtractSubString(strKey, strString, 1, '\t');

	// measure text size
	CRect rcRect;
	dc.DrawText(strName, &rcRect, DT_SINGLELINE | DT_CALCRECT);

	// both icon and text must fit
	int nHeight = max_stub(MB_CY_ICON + 4, rcRect.Height());

	// calculate total item width
	int nWidth = 2 + MB_CX_ICON + 6 + rcRect.Width() + nHeight/2;

	if (!strKey.IsEmpty())
	{
		dc.DrawText(strKey, &rcRect, DT_SINGLELINE | DT_CALCRECT);
		nWidth += rcRect.Width() + nHeight/2;
	}

	lpMeasureItemStruct->itemWidth = nWidth;
	lpMeasureItemStruct->itemHeight = nHeight;
}

void CMenuBar::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	UNREFERENCED_PARAMETER(nIDCtl);
	if (m_bFlatMenu)
		DrawItemFlat(lpDrawItemStruct);	// Windows XP style
	else
		DrawItem3D(lpDrawItemStruct);	// old style
}

void CMenuBar::DrawItemFlat(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	int fState = lpDrawItemStruct->itemState;

	dc.SetBkMode(TRANSPARENT);

	CBrush brHighlight;
	brHighlight.CreateSysColorBrush(COLOR_HIGHLIGHT);
	CBrush brMenuHilight;
	brMenuHilight.CreateSysColorBrush(COLOR_MENUHILIGHT);
	CBrush brMenu;
	brMenu.CreateSysColorBrush(COLOR_MENU);

	CRect rcItem = lpDrawItemStruct->rcItem;

	if (fState & ODS_SELECTED)
	{
		dc.FillRect(&rcItem, &brMenuHilight);
		dc.FrameRect(&rcItem, &brHighlight);
	}
	else
		dc.FillRect(&rcItem, &brMenu);

	UINT nID = lpDrawItemStruct->itemID;

	// look for image associated with command ID
	int nImage = -1;
	for (int i=0; i<m_CmdArray.GetSize(); i++)
		if (nID == m_CmdArray[i])
		{
			nImage = i;
			break;
		}

	// center icon vertically
	int nOffY = (rcItem.Height() - MB_CY_ICON) / 2;

	if (fState & ODS_CHECKED)
	{
		CRect rcCheck(2, rcItem.top + nOffY, 2 + MB_CX_ICON, rcItem.top + MB_CY_ICON + nOffY);

		if (fState & ODS_SELECTED)
		{
			dc.FillRect(&rcCheck, &brMenu);
		}
		else
		{
			CRect rcFrame = rcCheck;
			rcFrame.InflateRect(1, 1);
			dc.FrameRect(&rcFrame, &brHighlight);
		}

		// draw checkmark using the special system font
		if (nImage < 0)
		{
			CFont font;
			font.CreatePointFont(100, _T("Marlett"));
			CFont* pOldFont = dc.SelectObject(&font);

			dc.DrawText(_T("a"), 1, &rcCheck, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			dc.SelectObject(pOldFont);
		}
	}

	if (nImage >= 0)
	{
		int nImgList = (fState & (ODS_DISABLED | ODS_GRAYED)) ? AIL_DISABLED
			: (fState & ODS_SELECTED) ? AIL_HOT : AIL_NORMAL;

		m_ImgList.Draw(&dc, CPoint(rcItem.left + 2, rcItem.top + nOffY),
			nImgList, nImage);
	}

	CMenu* pMenu = m_bPopMenu ? m_pPopupMenu : m_pMenu;

	CString strString;
	pMenu->GetMenuString(nID, strString, MF_BYCOMMAND);

	CString strName, strKey;
	AfxExtractSubString(strName, strString, 0, '\t');
	AfxExtractSubString(strKey, strString, 1, '\t');

	CRect rcText = rcItem;
	rcText.DeflateRect(2 + MB_CX_ICON + 6, 0, rcItem.Height(), 0);

	const UINT	c_uHidePrefix = 
#if (_WINNT_WIN32 >= 0x0501)
					(m_bPrefix ? 0 : DT_HIDEPREFIX);
#else
					0;
#endif
					
	UINT fFormat = DT_SINGLELINE | DT_VCENTER | c_uHidePrefix;

	int nColorText = (fState & (ODS_DISABLED | ODS_GRAYED))
		? (fState & ODS_SELECTED) ? COLOR_3DSHADOW : COLOR_GRAYTEXT
		: (fState & ODS_SELECTED) ? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT;

	dc.SetTextColor(GetSysColor(nColorText));

	dc.DrawText(strName, &rcText, fFormat | DT_LEFT);
	if (!strKey.IsEmpty())
		dc.DrawText(strKey, &rcText, fFormat | DT_RIGHT);

	dc.Detach();
}

void CMenuBar::DrawItem3D(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	int fState = lpDrawItemStruct->itemState;

	dc.SetBkMode(TRANSPARENT);

	CBrush brHighlight;
	brHighlight.CreateSysColorBrush(COLOR_HIGHLIGHT);
	CBrush brMenu;
	brMenu.CreateSysColorBrush(COLOR_MENU);

	UINT nID = lpDrawItemStruct->itemID;

	int nImage = -1;
	for (int i=0; i<m_CmdArray.GetSize(); i++)
		if (nID == m_CmdArray[i])
		{
			nImage = i;
			break;
		}

	CRect rcItem = lpDrawItemStruct->rcItem;

	if (fState & ODS_SELECTED)
	{
		dc.FillRect(&rcItem, &brHighlight);
		if (nImage >= 0 || fState & ODS_CHECKED)
		{
			CRect rcBack = rcItem;	// don't highlight the image
			rcBack.right = 2 + MB_CX_ICON + 3;
			dc.FillRect(&rcBack, &brMenu);
		}
	}
	else
		dc.FillRect(&rcItem, &brMenu);

	int nOffY = (rcItem.Height() - MB_CY_ICON) / 2;

	if (!(fState & (ODS_DISABLED | ODS_GRAYED)) && (fState & ODS_SELECTED
		&& nImage >= 0 || fState & ODS_CHECKED))
	{
		// draw 3D edge around image
		CRect rcEdge(0, rcItem.top + nOffY - 2, 2 + MB_CX_ICON + 2, rcItem.top + nOffY + MB_CY_ICON + 2);

		if (fState & ODS_CHECKED && !(fState & ODS_SELECTED))
		{
			// draw checked item background with a halftone brush
			WORD pat[8];
			for (int i=0; i<8; i++)
				pat[i] = (WORD)(0x5555 << (i & 1));

			CBitmap bmp;
			bmp.CreateBitmap(8, 8, 1, 1, pat);

			CBrush brush;
			brush.CreatePatternBrush(&bmp);

			dc.SetTextColor(GetSysColor(COLOR_BTNFACE));
			dc.SetBkColor(GetSysColor(COLOR_BTNHILIGHT));
			dc.FillRect(&rcEdge, &brush);
		}
		else
			dc.FillRect(&rcEdge, &brMenu);

		if (nImage < 0 && fState & ODS_CHECKED)
		{
			CFont font;
			font.CreatePointFont(100, _T("Marlett"));
			CFont* pOldFont = dc.SelectObject(&font);

			dc.SetTextColor(GetSysColor(COLOR_MENUTEXT));
			dc.DrawText(_T("a"), 1, &rcEdge, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			dc.SelectObject(pOldFont);
		}
		
		dc.DrawEdge(&rcEdge, fState & ODS_CHECKED ? BDR_SUNKENOUTER : BDR_RAISEDINNER, BF_RECT);
	}

	if (nImage >= 0)
	{
		int nImgList = (fState & (ODS_DISABLED | ODS_GRAYED)) ? AIL_DISABLED
			: (fState & ODS_SELECTED) ? AIL_HOT : AIL_NORMAL;

		m_ImgList.Draw(&dc, CPoint(rcItem.left + 2, rcItem.top + nOffY),
			nImgList, nImage);
	}

	CMenu* pMenu = m_bPopMenu ? m_pPopupMenu : m_pMenu;

	CString strString;
	pMenu->GetMenuString(nID, strString, MF_BYCOMMAND);

	CString strName, strKey;
	AfxExtractSubString(strName, strString, 0, '\t');
	AfxExtractSubString(strKey, strString, 1, '\t');

	CRect rcText = rcItem;
	rcText.DeflateRect(2 + MB_CX_ICON + 6, 0, rcItem.Height(), 0);

	const UINT	c_uHidePrefix = 
#if (_WINNT_WIN32 >= 0x0501)
					(m_bPrefix ? 0 : DT_HIDEPREFIX);
#else
					0;
#endif
					
	UINT fFormat = DT_SINGLELINE | DT_VCENTER | c_uHidePrefix;

	if (fState & (ODS_DISABLED | ODS_GRAYED))
	{
		if (!(fState & ODS_SELECTED))
		{
			// draw 3D text highlight
			dc.SetTextColor(GetSysColor(COLOR_3DHIGHLIGHT));

			CRect rcHilite = rcText;
			rcHilite.OffsetRect(1, 1);

			dc.DrawText(strName, &rcHilite, fFormat | DT_LEFT);
			if (!strKey.IsEmpty())
				dc.DrawText(strKey, &rcHilite, fFormat | DT_RIGHT);

			dc.SetTextColor(GetSysColor(COLOR_3DSHADOW));
		}
		else
			dc.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
	}
	else
		dc.SetTextColor(GetSysColor((fState & ODS_SELECTED) ? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT));

	dc.DrawText(strName, &rcText, fFormat | DT_LEFT);
	if (!strKey.IsEmpty())
		dc.DrawText(strKey, &rcText, fFormat | DT_RIGHT);

	dc.Detach();
}

BOOL CMenuBar::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return GetParentFrame()->SendMessage(WM_COMMAND, wParam, lParam);
}
