// ChairSerializer.h: interface for the ChairSerializer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHAIRSERIALIZER_H__12EE411E_DB5B_42C8_8154_79A4D905F6CA__INCLUDED_)
#define AFX_CHAIRSERIALIZER_H__12EE411E_DB5B_42C8_8154_79A4D905F6CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataPrepare/my_chair.h"
#include "Common/tstring.h"
#include "ChairLink.h"
#include "../../CSChair/chair_int.h"

MACRO_EXCEPTION( ChairSerializerException,		CommonException )
MACRO_EXCEPTION( ChairSerializerInvalidVersion, ChairSerializerException )
MACRO_EXCEPTION( ChairSerializerInvalidFile,	ChairSerializerException )

class ChairSaver
{
public:
	void	Save( LPCTSTR szFileName, const MyChair::ChairSet& Chairs, const ChairLinkList_t& ChairLinks, const IChairSerializer::PositionMap_t& PosMap  ) ;
	void	SaveToString( std::tstring& strDocument, const MyChair::ChairSet& Chairs, const ChairLinkList_t& ChairLinks, const IChairSerializer::PositionMap_t& PosMap  ) ;
	void	SaveToBinary( std::vector<BYTE>& Data, const MyChair::ChairSet& Chairs, const ChairLinkList_t& ChairLinks, const IChairSerializer::PositionMap_t& PosMap  ) ;
};

class ChairLoader
{
public:
	void	Load( LPCTSTR szFileName, MyChair::ChairSet& Chairs, ChairLinkList_t& ChairLinks, IChairSerializer::PositionMap_t& PosMap   ) ;
	void	LoadFromString( LPCTSTR szDocument, MyChair::ChairSet& Chairs, ChairLinkList_t& ChairLinks, IChairSerializer::PositionMap_t& PosMap   ) ;
	void	LoadFromBinary( const std::vector<BYTE>& Data, MyChair::ChairSet& Chairs, ChairLinkList_t& ChairLinks, IChairSerializer::PositionMap_t& PosMap  );
};

#endif // !defined(AFX_CHAIRSERIALIZER_H__12EE411E_DB5B_42C8_8154_79A4D905F6CA__INCLUDED_)
