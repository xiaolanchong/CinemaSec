// IFrameIterator.h: interface for the IFrameIterator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IFRAMEITERATOR_H__CAD696A5_29E3_4ACE_8E6A_DE0289427E9B__INCLUDED_)
#define AFX_IFRAMEITERATOR_H__CAD696A5_29E3_4ACE_8E6A_DE0289427E9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "my_chair.h"

interface IFrameIterator  
{
public:
	virtual int GetTotalFrameNum() = 0;
	virtual int GetBackGround(int nFrameNum, Arr2ub& bg) = 0;
	virtual int GetChairs    (int nFrameNum, MyChair::ChairSet& chairs) = 0;

	// for inheritance
	virtual ~IFrameIterator() {};
};

#endif // !defined(AFX_IFRAMEITERATOR_H__CAD696A5_29E3_4ACE_8E6A_DE0289427E9B__INCLUDED_)
