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
#ifndef __CHAIR_CONTENT_IMPL_H_INCLUDED_0667165370143302__
#define __CHAIR_CONTENT_IMPL_H_INCLUDED_0667165370143302__

//=====================================================================================//
//                               class ChairContentImpl                                //
//=====================================================================================//
#include "IChairContent.h"
#include "IChairContentEx.h"
#include "Range.h"

//! реализация интерфейса загрузки разметки IChairContentEx
class ChairContentExImpl : public IChairContentEx 
{
public:
	ChairContentExImpl();
	virtual ~ChairContentExImpl();

	virtual HRESULT			AddToRangeList		( RangeList_t& RangeList, int nStart, int nEnd, Range::CONTENT cont );
	virtual HRESULT			RemoveFromRangeList	( RangeList_t& RangeList, int nMin, int nMax);
	virtual void			DumpRangeList(const RangeList_t& RangeList) ;

	virtual Range::CONTENT	GetState( const RangeList_t& RangeList, int nTime );

	virtual HRESULT			Load(	LPCWSTR szFileName, 
									const std::vector<BaseChair>&Chairs, 
									std::vector<RangeList_t>& RangeArr,
									std::wstring& sVideoFile,
									std::wstring& sChairFile, 
									std::wstring& sBackgroundFile
									);
	virtual HRESULT			Save(	LPCWSTR szFileName, const std::vector<BaseChair>&Chairs, 
									const std::vector<RangeList_t>& RangeArr,
									LPCWSTR sVideoFile,
									LPCWSTR sChairFile, 
									LPCWSTR sBackgroundFile
									);
	virtual void 			Release();
};

//! реализация интерфейса загрузки разметки IChairContent, упрощенная версия IChairContentEx
class ChairContentImpl : public IChairContent 
{
	//! текущая разметка, с которой производят действия
	//! индекс керсел в файле кресел, см. BaseChair
	std::vector<RangeList_t>	m_RangeListArr ;
	std::vector<int>			m_IndexArr;
public:
	HRESULT	Load( LPCWSTR szFileName, const std::vector<BaseChair>& Chairs );
	HRESULT GetContent( int nFrame, std::map<int, ChairContents >& Contents  );
	void	Release()
	{
		delete this;
	}
};

#endif //__CHAIR_CONTENT_IMPL_H_INCLUDED_0667165370143302__
