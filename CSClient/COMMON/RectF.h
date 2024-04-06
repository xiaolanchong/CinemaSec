//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: For CRect <-> Gdiplus::RectF converting
//
//=============================================================================


// RectF.h: interface for the CRectF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECTF_H__6F941D3B_9676_4561_9ED4_B2CE5B2DB785__INCLUDED_)
#define AFX_RECTF_H__6F941D3B_9676_4561_9ED4_B2CE5B2DB785__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <gdiplus.h>

struct CRectF : Gdiplus::RectF 
{
	CRectF(float x, float y, float w, float h) : Gdiplus::RectF(x,y,w,h){}

	CRectF(const Gdiplus::RectF& rc) : Gdiplus::RectF(rc){}
	CRectF(const CRectF& rc) : Gdiplus::RectF(rc){}
	CRectF(const CRect& rc) : Gdiplus::RectF(	float(rc.left), float(rc.top), 
												float(rc.Width()), float(rc.Height())){}

//	~CRectF(){}



	void Deflate(float l, float t, float r, float b)
	{
		X += l;
		Y += t;
		Width -= r+l;
		Height -= b+t;
	}

	void Inflate(float l, float t, float r, float b)
	{
		X -= l;
		Y -= t;
		Width += r+l;
		Height += b+t;
	}

	Gdiplus::PointF	Center() const { return Gdiplus::PointF(X+Width/2, Y+Height/2);}

	float Left() const { return X;}
	float Top() const {return Y;}
	float Right() const { return X + Width;}
	float Bottom() const { return Y + Height;}

	CRect ToCRect() const { return CRect( int(Left()), int(Top()), int(Right()), int(Bottom()));}
};

#endif // !defined(AFX_RECTF_H__6F941D3B_9676_4561_9ED4_B2CE5B2DB785__INCLUDED_)
