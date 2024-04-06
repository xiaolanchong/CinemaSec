// Range.h: interface for the Range class.
//
//////////////////////////////////////////////////////////////////////

//! \author Eugene Gorbachev
//!	\date	2004-10-23
//	modified 2005-10-06

#if !defined(AFX_RANGE_H__77E1C90A_0802_4DAC_B20E_72D8B4B70D96__INCLUDED_)
#define AFX_RANGE_H__77E1C90A_0802_4DAC_B20E_72D8B4B70D96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../IChairContentEx.h"

//! результат пересечения 2х отрезков
enum IntersectResult
{
	ir_left, ir_right, ir_erase, ir_none, ir_divide
};

//! результат объединения 2х отрезков
std::pair<IntersectResult,int> InsertSegment(	const std::pair<int, int>& New,  
												const std::pair<int, int>& Old 
											) ;

//! вставка в список отрезков разметки нового размеченного отрезка
//! \param	RangeList	список отрезков
//! \param	nStart		начало отрезка 
//! \param	nEnd		конец отрезка
//! \param	cont		содержимое отрезка
void			AddToRangeList		( RangeList_t& RangeList, int nStart, int nEnd, Range::CONTENT cont );

//! удалить из список отрезков разметки
//! \param	RangeList	список отрезков
//! \param	nMin		начало отрезка 
//! \param	nMax		конец отрезка
void			RemoveFromRangeList	( RangeList_t& RangeList, int nMin, int nMax);

//! выыести содержимое списка в Output
//! \param	RangeList	список отрезков
void			DumpRangeList(const RangeList_t& RangeList) ;

//! определить текущее состояние для кадра разметки
//! \param	RangeList	список отрезков
//! \param	nTime		номер кадра 
//! \return				содержимое кадра
Range::CONTENT	GetState( const RangeList_t& RangeList, int nTime );

#endif // !defined(AFX_RANGE_H__77E1C90A_0802_4DAC_B20E_72D8B4B70D96__INCLUDED_)
