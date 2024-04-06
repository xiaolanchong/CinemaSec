// Range.cpp: implementation of the Range class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "Slider.h"
#include "Range.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void AddToRangeList( RangeList_t& RangeList, int nStart, int nEnd, Range::CONTENT cont )
{	
	RangeList_t::iterator it = RangeList.begin();
	while( it != RangeList.end() )
	{
		std::pair<IntersectResult, int> res = 
			InsertSegment(	std::make_pair( nStart, nEnd ) , 
							std::make_pair( it->m_nStart, it->m_nEnd ) );
		switch( res.first )
		{
			case ir_none : ++it; break;
			case ir_erase : it = RangeList.erase( it++ );break;
			case ir_left	:
				   {
						it->m_nStart = res.second;
						++it;
						break;
				   }
			case ir_right	:
				   {
						it->m_nEnd = res.second;
						++it;
						break;
				   }
			case ir_divide :
				{
					int end = it->m_nEnd;
					it->m_nEnd = nStart;
					Range::CONTENT cntOld = it->m_nContent;
					++it;
					it = RangeList.insert(it, Range( nEnd, end, cntOld ) );
					break;
				}
			default:
				ASSERT(FALSE);
		}
		;
	}
	RangeList.push_back( Range( nStart, nEnd, cont ) );
	DumpRangeList(RangeList);
}

void RemoveFromRangeList(RangeList_t& RangeList,int nStart, int nEnd)
{
	RangeList_t::iterator it = RangeList.begin();
	while( it != RangeList.end() )
	{
		std::pair<IntersectResult, int> res = 
			InsertSegment(	std::make_pair( nStart, nEnd ) , 
							std::make_pair( it->m_nStart, it->m_nEnd ) );
		switch( res.first )
		{
			case ir_none : ++it; break;
			case ir_erase : it = RangeList.erase( it++ );break;
			case ir_left	:
				   {
						it->m_nStart = res.second;
						++it;
						break;
				   }
			case ir_right	:
				   {
						it->m_nEnd = res.second;
						++it;
						break;
				   }
			case ir_divide :
				{
					int end = it->m_nEnd;
					it->m_nEnd = nStart;
					Range::CONTENT cntOld = it->m_nContent;
					++it;
					it = RangeList.insert(it, Range( nEnd, end, cntOld ) );
					break;
				}
			default:
				ASSERT(FALSE);
		}
	}
	DumpRangeList(RangeList);
}

std::pair<IntersectResult,int> InsertSegment
								(	const std::pair<int, int>& New,  
									const std::pair<int, int>& Old 
								) 
{
	if( Old.first > New.second  ||
		Old.second < New.first)
	{
		return std::make_pair( ir_none, 0);
	}

	if( New.first	<=	Old.second &&
		New.first	<=	Old.first &&
		New.second	>=	Old.second &&
		New.second	>=	Old.first )
	{
		return std::make_pair( ir_erase, 0);
	}

	if( Old.first	<=	New.second &&
		Old.first	<=	New.first &&
		Old.second	>=	New.second &&
		Old.second	>=	New.first 
	) 
	{
		return std::make_pair( ir_divide, 0);
	}
	if( New.first <= Old.first &&
		New.second < Old.second )
	{
		return std::make_pair( ir_left, New.second );
	}
	if( New.first > Old.first &&
		New.second > Old.second )
	{
		return std::make_pair( ir_right, New.first );
	}

	ASSERT(FALSE);
	return std::make_pair( ir_none, 0 );
}

void	DumpRangeList(const RangeList_t& RangeList) 
{
	RangeList_t::const_iterator it = RangeList.begin();
	CString strText;
	while( it != RangeList.end() )
	{
		strText.Format(_T("[%d,%d]=%d "), it->m_nStart, it->m_nEnd, (int)it->m_nContent);
		OutputDebugString(strText);
		++it;
	}
	OutputDebugString(_T("\n"));
}

Range::CONTENT	GetState( const RangeList_t& RangeList, int nTime )
{
	RangeList_t::const_iterator it = RangeList.begin();
	while( it != RangeList.end() )
	{
		if( it->m_nStart	<= nTime  &&
			it->m_nEnd		>= nTime )
		{
			ASSERT( it->m_nContent != Range::undefined );
			return it->m_nContent;
		}
		++it;
	}
	return Range::undefined;
}