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

//! ��������� ����������� 2� ��������
enum IntersectResult
{
	ir_left, ir_right, ir_erase, ir_none, ir_divide
};

//! ��������� ����������� 2� ��������
std::pair<IntersectResult,int> InsertSegment(	const std::pair<int, int>& New,  
												const std::pair<int, int>& Old 
											) ;

//! ������� � ������ �������� �������� ������ ������������ �������
//! \param	RangeList	������ ��������
//! \param	nStart		������ ������� 
//! \param	nEnd		����� �������
//! \param	cont		���������� �������
void			AddToRangeList		( RangeList_t& RangeList, int nStart, int nEnd, Range::CONTENT cont );

//! ������� �� ������ �������� ��������
//! \param	RangeList	������ ��������
//! \param	nMin		������ ������� 
//! \param	nMax		����� �������
void			RemoveFromRangeList	( RangeList_t& RangeList, int nMin, int nMax);

//! ������� ���������� ������ � Output
//! \param	RangeList	������ ��������
void			DumpRangeList(const RangeList_t& RangeList) ;

//! ���������� ������� ��������� ��� ����� ��������
//! \param	RangeList	������ ��������
//! \param	nTime		����� ����� 
//! \return				���������� �����
Range::CONTENT	GetState( const RangeList_t& RangeList, int nTime );

#endif // !defined(AFX_RANGE_H__77E1C90A_0802_4DAC_B20E_72D8B4B70D96__INCLUDED_)
