// Background.h: interface for the CBackground class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BACKGROUND_H__407E020F_BE20_4760_BAF2_24D49B8D25C4__INCLUDED_)
#define AFX_BACKGROUND_H__407E020F_BE20_4760_BAF2_24D49B8D25C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/*
#include <gdiplus.h>
using namespace Gdiplus;
*/
#include "../Common/RectF.h"

class CBackground  
{
public:
	CBackground();
	virtual ~CBackground();

	virtual void	Draw(Graphics& gr, const CRectF& rc) const = 0;
	virtual CSize	GetSize()							 const = 0;
};

class CControlBackground : public CBackground
{
public:
	virtual void Stop()		= 0;
	virtual void Play()		= 0;
//	virtual void Pause()	= 0;
};

#endif // !defined(AFX_BACKGROUND_H__407E020F_BE20_4760_BAF2_24D49B8D25C4__INCLUDED_)
