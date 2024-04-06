// ChairLink.cpp: implementation of the ChairLink class.
//
//////////////////////////////////////////////////////////////////////
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "csclient.h"
#include "ChairLink.h"

#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace
{

bool	PtInRect( CPoint Point, CPoint pt1, CPoint pt2, int Gap )
{
	CSize a = pt2 - pt1;
	double d2 = a.cx / sqrt( double(a.cx * a.cx + a.cy * a.cy) );
	double d1 = a.cy / sqrt( double(a.cx * a.cx + a.cy * a.cy) );
	CSize off (int(Gap * d1), -int (Gap * d2) );
	CPoint pt[4] = 
	{
		pt1 + off, pt2 + off,
		pt2 - off, pt1 - off
	};
	CRgn rgn;
	rgn.CreatePolygonRgn( pt, 4, WINDING );
	return rgn.PtInRegion( Point ) == TRUE;
}

struct PairPred
{
	MyChair::ChairSet::iterator itF, itS;
	bool operator() ( const ChairLink& cl ) const 
	{
		return (( cl.m_itrFirst == itF ) && ( cl.m_itrSecond == itS )) ||
				( cl.m_itrFirst == itS ) && ( cl.m_itrSecond == itF );
	}

	PairPred(const PairPred& pp):
		itF(pp.itF), itS(pp.itS){}

	PairPred( MyChair::ChairSet::iterator itf ,MyChair::ChairSet::iterator its ) : itF(itf), itS( its ){}
};

struct ItemPred
{
	MyChair::ChairSet::iterator m_it;
	bool operator() ( const ChairLink& cl ) const 
	{
		return (( cl.m_itrFirst == m_it ) || ( cl.m_itrSecond == m_it ));
	}

	ItemPred(const ItemPred& pp):
		m_it(pp.m_it){}

	ItemPred( MyChair::ChairSet::iterator itf  ) : m_it(itf){}
};

}

bool	PtInLine( CPoint Point, const CPoint& ptStart, const CPoint & ptEnd, int Gap )
{
	return PtInRect( Point, ptStart, ptEnd, Gap );
}

bool	ExistLink( const ChairLinkList_t& cl, MyChair::ChairSet::iterator itF, MyChair::ChairSet::iterator itS )
{
	return std::find_if( cl.begin(), cl.end(), PairPred( itF, itS ) )  != cl.end();
}

void	DeleteLinkWithItem(ChairLinkList_t& cl, MyChair::ChairSet::iterator it)
{
	ItemPred _Pr(it);
	ChairLinkList_t::iterator _L = cl.end();
	for (ChairLinkList_t::iterator _F = cl.begin(); _F != _L; )
		if (_Pr(*_F))
			cl.erase(_F++);
		else
			++_F; 
}