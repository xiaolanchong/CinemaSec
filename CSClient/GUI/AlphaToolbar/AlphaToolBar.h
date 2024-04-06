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

#include "AlphaImageList.h"

class CAlphaToolBar : public CToolBar
{
public:
	CAlphaToolBar();
	virtual ~CAlphaToolBar();

public:
	// Create toolbar
	BOOL Create(CWnd* pParentWnd, UINT nID=0);

	// Load toolbar and bitmap from resources
	BOOL LoadToolBar(UINT nID, int nStyle=AILS_OLD);

protected:
	CAlphaImageList m_ImgList;

protected:
	DECLARE_MESSAGE_MAP()
};
