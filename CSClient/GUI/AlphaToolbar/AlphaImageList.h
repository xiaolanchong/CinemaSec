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

// image list styles
#define AILS_OLD	0	// use old 16 color images
#define AILS_NEW	1	// use new 8-32 bit images

// image list selectors
#define AIL_NORMAL		0
#define AIL_HOT			1
#define AIL_DISABLED	2


class CAlphaImageList  
{
public:
	CAlphaImageList();
	virtual ~CAlphaImageList();

public:
	// Create with given image size, style and initial count
	BOOL Create(int nWidth, int nHeight, int nStyle, int nCnt=0);

	// Add bitmap resource
	BOOL AddBitmap(UINT nID);

	// Draw image of given type
	BOOL Draw(CDC* pDC, CPoint ptPos, int nImgList, int nIndex);

	// Get handle of image list
	HIMAGELIST GetImageList(int nImgList);

protected:
	CImageList m_ilNormal;		// normal images
	CImageList m_ilHot;			// hot images (saturated)
	CImageList m_ilDisabled;	// disabled images (grayscale)

	int m_nBmpDepth;	// imagelist bitmap depth (ILC_COLOR4/24/32)
	CSize m_szImg;		// size of images
};
