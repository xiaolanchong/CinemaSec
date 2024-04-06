/*********************************************************
* Simple IE-like Menu And Toolbar
* Version: 1.3
* Date: Semptember 1, 2003
* Autor: Michal Mecinski
* E-mail: mimec@mimec.w.pl
* WWW: http://www.mimec.w.pl
*
* You may freely use and modify this code, but don't remove
* this copyright note.
*
* There is no warranty of any kind, express or implied, for this class.
* The author does not take the responsibility for any damage
* resulting from the use of it.
*
* Let me know if you find this code useful, and
* send me any modifications and bug reports.
*
* Copyright (C) 2002-03 by Michal Mecinski
*********************************************************/

#pragma once

#define	MBM_POPUPMENU	(WM_USER + 1801)

#include "AlphaImageList.h"


class CMenuBar : public CToolBarCtrl
{
public:
	CMenuBar();
	virtual ~CMenuBar();

public:
	// Create menu bar
	BOOL Create(CWnd* pParentWnd, UINT nID=0);

	// Load menu from menu resource
	BOOL LoadMenuBar(UINT nID, int nStyle=AILS_OLD);

	// Attach menu from handle
	BOOL AttachMenu(HMENU hMenu);

	// Add images from toolbar resource
	BOOL LoadToolBar(UINT nID);


	// Process the WM_MENUCHAR message
	BOOL OpenMenu(UINT nChar);

	// Process the SC_KEYMENU system command
	BOOL OnKeyMenu(UINT nKey);


	// Toggle menu prefixes on/off under WinXP
	void SetPrefix(BOOL bPrefix=TRUE);

	// Highlight the given menu
	void Highlight(int nItem=0);


	// Change menu text color when parent frame was (de)activated
	void Activate(BOOL bActive);

	// Get system settings and resize bar
	void UpdateSettings();


	// Load popup menu
	BOOL LoadPopupMenu(UINT nID);

	// Track popup menu
	void TrackPopup(int nIndex, CPoint ptPos);
	// Track popup menu (given the menu name)
	void TrackPopup(LPCTSTR lpszName, CPoint ptPos);

	HMENU GetMenuHandle();


public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);


protected:
	// Open current menu and install hook
	void OpenPopup();

	// Process menu message
	BOOL OnMenuInput(MSG* pMsg);

	// Hook procedure
	static LRESULT CALLBACK MenuInputFilter(int nCode, WPARAM wParam, LPARAM lParam);

	// Check if a submenu is currently open
	BOOL InSubMenu();

	// Draw flat menu item (WinXP style)
	void DrawItemFlat(LPDRAWITEMSTRUCT lpDrawItemStruct);
	// Draw 3D menu item (traditional style)
	void DrawItem3D(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
	CMenu* m_pMenu;		// window menu
	CMenu* m_pPopupMenu;// popup menu

	CFont m_Font;		// window font

	int m_nItemCount;	// number of menus
	int m_nPressed;		// index of selected menu

	CPoint m_ptLast;	// last mouse position

	BOOL m_bKeyboard;	// menu opened via keyboard

	CWnd* m_pPrevFocus;	// window to restore focus

	BOOL m_bPrefix;		// prefixes are displayed
	BOOL m_bKeepPrefix;	// prefixes shouldn't be hidden

	BOOL m_bNextMenu;	// another menu will be opened

	BOOL m_bSelPopup;	// selected item is a popup menu
	HMENU m_hSelMenu;	// selected item's menu handle

	CAlphaImageList m_ImgList;	// image list to draw items
	CUIntArray m_CmdArray;		// command IDs associated with images

	BOOL m_bFlatMenu;	// menu is in flat mode
	BOOL m_bCues;		// always display visual cues

	BOOL m_bWndActive;	// parent frame is active

	BOOL m_bPopMenu;	// which menu is used

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg LRESULT OnPopupMenu(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

