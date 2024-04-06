// EmptyBackground.h: interface for the CEmptyBackground class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EMPTYBACKGROUND_H__8DB619A7_4ED3_4712_BC9E_100C024463AA__INCLUDED_)
#define AFX_EMPTYBACKGROUND_H__8DB619A7_4ED3_4712_BC9E_100C024463AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Background.h"
#include <string>

class CEmptyBackground : public CBackground  
{
//	static std::wstring s_NoImage;
public:
	CEmptyBackground();
	virtual ~CEmptyBackground();

	virtual void Draw(Graphics& gr, const CRectF& rc) const;
	virtual CSize	GetSize()						 const { return CSize(352, 288);}  ;
};

#endif // !defined(AFX_EMPTYBACKGROUND_H__8DB619A7_4ED3_4712_BC9E_100C024463AA__INCLUDED_)
