// ChairLink.h: interface for the ChairLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHAIRLINK_H__D383EFFA_3C56_4FA8_92E5_40443C28345B__INCLUDED_)
#define AFX_CHAIRLINK_H__D383EFFA_3C56_4FA8_92E5_40443C28345B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataPrepare/my_chair.h"
#include "UtilityChair.h"

struct ChairLink
{

	MyChair::ChairSet::iterator	m_itrFirst;
	MyChair::ChairSet::iterator	m_itrSecond;
	Utility::CHAIR_POS	m_posFirst;

	ChairLink( MyChair::ChairSet::iterator itf, MyChair::ChairSet::iterator its, Utility::CHAIR_POS pos  ):
		m_posFirst(pos),
		m_itrFirst(itf), m_itrSecond(its){}
	ChairLink( const ChairLink& cl):
		m_posFirst(cl.m_posFirst),
		m_itrFirst(cl.m_itrFirst), m_itrSecond(cl.m_itrSecond){}		
};

typedef std::list< ChairLink >	ChairLinkList_t	;

struct ChairLinkDisk
{
	Utility::CHAIR_POS	m_posFirst;
	int					m_nFirstId, m_nSecondId;
};

typedef std::vector< ChairLinkDisk >	ChairLinkDiskArray_t;

bool	PtInLine( CPoint Point, const CPoint& ptStart, const CPoint & ptEnd, int Gap );

bool	ExistLink( const ChairLinkList_t& cl, MyChair::ChairSet::iterator itF, MyChair::ChairSet::iterator itS );
void	DeleteLinkWithItem(ChairLinkList_t& cl, MyChair::ChairSet::iterator it);

#endif // !defined(AFX_CHAIRLINK_H__D383EFFA_3C56_4FA8_92E5_40443C28345B__INCLUDED_)
