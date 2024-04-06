//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		chair content serialize interface
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   15.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#include "stdafx.h"
#include "ChairContentImpl.h"
#include "ContentSerializer.h"
#include "Range.h"
#include "../../CSChair/interfaceEx.h"

//=====================================================================================//
//                               class ChairContentImpl                                //
//=====================================================================================//
ChairContentExImpl::ChairContentExImpl()
{
}

ChairContentExImpl::~ChairContentExImpl()
{
}


HRESULT			ChairContentExImpl::AddToRangeList		( RangeList_t& RangeList, int nStart, int nEnd, Range::CONTENT cont )
{
	::AddToRangeList( RangeList, nStart, nEnd, cont );
	return S_OK;
}

HRESULT			ChairContentExImpl::RemoveFromRangeList	( RangeList_t& RangeList, int nMin, int nMax)
{
	::RemoveFromRangeList( RangeList, nMin, nMax );
	return S_OK;
}

void			ChairContentExImpl::DumpRangeList(const RangeList_t& RangeList) 
{
	::DumpRangeList( RangeList );
}

Range::CONTENT	ChairContentExImpl::GetState( const RangeList_t& RangeList, int nTime )
{
	return ::GetState( RangeList, nTime );
}

void 			ChairContentExImpl::Release()
{
	delete this;
}

HRESULT			ChairContentExImpl::Load(
	LPCWSTR szFileName, 
	const std::vector<BaseChair>&Chairs, 
	std::vector<RangeList_t>& RangeArr,
	std::wstring& sVideoFile,
	std::wstring& sChairFile, 
	std::wstring& sBackgroundFile)
{
	if( !szFileName  || !lstrlen( szFileName ) || !Chairs.size() )
		return E_INVALIDARG;
	try
	{
		ContentLoader cl;
		cl.Load( szFileName, Chairs, RangeArr, sVideoFile, sChairFile, sBackgroundFile ); 
	}
	catch(CommonException)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT			ChairContentExImpl::Save(	LPCWSTR szFileName, 
											const std::vector<BaseChair>&Chairs, 
											const std::vector<RangeList_t>& RangeArr,
											LPCWSTR sVideoFile,
											LPCWSTR sChairFile, 
											LPCWSTR sBackgroundFile)
{
	try
	{

		std::wstring sVF = sVideoFile ? sVideoFile : L"";
		std::wstring sCF = sChairFile ? sChairFile : L"";
		std::wstring sBF = sBackgroundFile ? sBackgroundFile : L"";
		ContentSaver cs;
		cs.Save( szFileName, Chairs, RangeArr, sVF, sCF, sBF );
	}
	catch(CommonException)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT	ChairContentImpl::Load( LPCWSTR szFileName, const std::vector<BaseChair>& Chairs )
{
	ChairContentExImpl cl;
	std::wstring sVideoFile;
	std::wstring sChairFile;
	std::wstring sBackgroundFile;
	HRESULT hr = cl.Load( szFileName, Chairs, m_RangeListArr, sVideoFile, sChairFile, sBackgroundFile  );
	if( hr == S_OK )
	{
		m_IndexArr.resize( Chairs.size() );
		for( size_t i = 0 ; i < Chairs.size(); ++i )
		{
			m_IndexArr[i] = Chairs[i].index;
		}
	}
//	cl.Load(  )
	return hr;
}

HRESULT ChairContentImpl::GetContent( int nFrame, std::map<int, ChairContents > & Contents )
{
	//ChairContentExImpl cl;
	//cl.Load( szFileName, Chairs, RangeListArr  );
	Contents.clear();
	ChairContentExImpl cl;
	for( size_t i = 0; i < m_RangeListArr.size(); ++i)
	{
		Range::CONTENT cnt = cl.GetState( m_RangeListArr[i], nFrame );
		ChairContents ChCnt = CHAIR_UNDEFINED;
		switch( cnt )
		{
		case Range::clothes: ChCnt = CHAIR_CLOTHES; break;
		case Range::human: ChCnt = CHAIR_HUMAN;		break;
		case Range::empty: ChCnt = CHAIR_EMPTY;		break;
		case Range::unknown: ChCnt = CHAIR_UNKNOWN;		break;
		case Range::undefined: ChCnt = CHAIR_UNDEFINED;		break;
		}
		int nIndex = m_IndexArr[ i ];
		if( ChCnt != CHAIR_UNDEFINED )
			Contents.insert(  std::make_pair ( nIndex, ChCnt) );	
	}
#if 0 //def _DEBUG 
	std::map<int, ChairContents >::const_iterator it = Contents.begin();
	for(; it != Contents.end(); ++it )
	{
		CString z;
		z.Format( _T("[%d]=%d\n"), it->first, it->second );
		OutputDebugString(z);
	}
#endif
	return S_OK;
}

EXPOSE_INTERFACE(ChairContentExImpl, IChairContentEx, CHAIR_CONTENT_INTERFACE_EX );
EXPOSE_INTERFACE(ChairContentImpl, IChairContent, CHAIR_CONTENT_INTERFACE);