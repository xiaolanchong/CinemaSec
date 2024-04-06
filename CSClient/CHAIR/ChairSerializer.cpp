// ChairSerializer.cpp: implementation of the ChairSerializer class.
//
//////////////////////////////////////////////////////////////////////
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "csclient.h"
#include "ChairSerializer.h"
#include "../CSChair/ChairSerializer.h"
#include "../CSChair/chair_int.h"
#include "../CSChair/interface.h"
#include "UtilityChair.h"
#include "ChairLink.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool ConvertToBaseChair( const MyChair::ChairSet& Chairs, const ChairLinkList_t& ChairLinks , std::vector<BaseChair>& BaseChairArr )
{
	UNREFERENCED_PARAMETER(ChairLinks);
	MyChair::ChairSet::const_iterator itrCL = Chairs.begin();
	std::map<const MyChair::Chair*, int> IndexMap;

	int nIndex = 0;
	for( nIndex = 0;itrCL != Chairs.end(); ++itrCL, ++nIndex)
	{
		IndexMap.insert( std::make_pair( &(*itrCL), nIndex ) );
	}
	itrCL = Chairs.begin();
	BaseChairArr.clear();
	BaseChairArr.resize(nIndex);

	for(nIndex = 0;itrCL != Chairs.end(); ++itrCL, ++nIndex)
	{
		BaseChair& cd = BaseChairArr[ nIndex ];
		cd.index		= itrCL->id;
		cd.curve		= itrCL->curve;
		cd.cameraNo		= itrCL->subFrameNo;

	//	AssignIndex<const Chair*, int>( IndexMap, itrCL->pLeftChair, cd.nLeft );
	}

	MyChair::ChairSet::const_iterator it = Chairs.begin();
	for( ;it != Chairs.end(); ++it  )
	{

	}
	return true;
}

bool ConvertFromBaseChair( MyChair::ChairSet& Chairs, ChairLinkList_t& ChairLinks , const std::vector<BaseChair>& BaseChairArr  )
{
	UNREFERENCED_PARAMETER(ChairLinks);
	std::vector<BaseChair>::const_iterator itrCL = BaseChairArr.begin();
	int nIndex = 0;
	
#ifdef CREATE_LINK
	std::map<const Chair*, int> IndexMap;
	for( nIndex = 0;itrCL != BaseChairArr.end(); ++itrCL, ++nIndex)
	{
	IndexMap.insert( std::make_pair( &(*itrCL), nIndex ) );
	}
	itrCL = BaseChairArr.begin();
#endif
	Chairs.clear();

	for(nIndex = 0;itrCL != BaseChairArr.end(); ++itrCL, ++nIndex)
	{
		const BaseChair&	cd = *itrCL;

		if( cd.curve.empty() ) 
		{
			TRACE0("Warning: empty chair curve\n");
			continue;
		}

		Chairs.push_back( MyChair::Chair() );

		MyChair::Chair&				cc = Chairs.back();
		cc.id			= cd.index;
		cc.curve		= cd.curve;
		cc.subFrameNo		= cd.cameraNo;
		//		cc.center		= cd.center;
		cc.center		= Utility::GetMassCenter( cc.curve );
		//	AssignIndex<const Chair*, int>( IndexMap, itrCL->pLeftChair, cd.nLeft );
	}	
	return true;
}

/////////////////////////////////////////////////////////////////////

void	ChairSaver::Save( LPCTSTR szFileName, const MyChair::ChairSet& Chairs, const ChairLinkList_t& ChairLinks, const IChairSerializer::PositionMap_t& PosMap  )
{
	std::vector<BaseChair> BaseChairArr;
	bool res = ConvertToBaseChair( Chairs, ChairLinks, BaseChairArr );
	if(! res )
		throw ChairSerializerException("Error occuried while converting");
	IChairSerializer* pChairSer;
	DWORD dwRes = CreateInterface( CHAIR_SERIALIZER_INTERFACE_0, (void**)&pChairSer );
	// FIXME : to resource or smth
	if( dwRes != 0)
		throw ChairSerializerException("Invalid or corrupted file");
	dwRes = pChairSer->SaveGaugeHallToFile( szFileName, BaseChairArr, PosMap );
	if( dwRes != 0)
		throw ChairSerializerException("Invalid or corrupted file");
}

void	ChairSaver::SaveToString( std::tstring& strDocument, const MyChair::ChairSet& Chairs, const ChairLinkList_t& ChairLinks, const IChairSerializer::PositionMap_t& PosMap  ) 
{
	std::vector<BaseChair> BaseChairArr;
	bool res = ConvertToBaseChair( Chairs, ChairLinks, BaseChairArr );
	if(! res )
		throw ChairSerializerException("Error occuried while converting");
	IChairSerializer* pChairSer;
	DWORD dwRes = CreateInterface( CHAIR_SERIALIZER_INTERFACE_0, (void**)&pChairSer );
	// FIXME : to resource or smth
	if( dwRes != 0)
		throw ChairSerializerException("Invalid or corrupted file");
	dwRes = pChairSer->SaveGaugeHallToString( strDocument, BaseChairArr, PosMap );
	// FIXME : to resource or smth
	if( dwRes != 0)
		throw ChairSerializerException("Invalid or corrupted file");
}

void	ChairSaver::SaveToBinary( std::vector<BYTE>& Data, const MyChair::ChairSet& Chairs, const ChairLinkList_t& ChairLinks, const IChairSerializer::PositionMap_t& PosMap  ) 
{
	std::vector<BaseChair> BaseChairArr;
	bool res = ConvertToBaseChair( Chairs, ChairLinks, BaseChairArr );
	if(! res )
		throw ChairSerializerException("Error occuried while converting");
	IChairSerializer* pChairSer;
	DWORD dwRes = CreateInterface( CHAIR_SERIALIZER_INTERFACE_0, (void**)&pChairSer );
	if( dwRes != S_OK )
		throw ChairSerializerException("Error occuried while converting");
	dwRes = pChairSer->SaveGaugeHallToBinaryData( Data, BaseChairArr, PosMap );
	pChairSer->Release();
	if( dwRes != S_OK)
		throw ChairSerializerException("Invalid or corrupted file");
}

///// load ////////////////



void	ChairLoader::Load( LPCTSTR szFileName, MyChair::ChairSet& Chairs, ChairLinkList_t& ChairLinks, IChairSerializer::PositionMap_t& PosMap  )  
{
	UNREFERENCED_PARAMETER(PosMap);
	std::vector<BaseChair> BaseChairArr;
	DWORD dwRes = LoadGaugeHallFromFile( szFileName, BaseChairArr );
	// FIXME : to resource or smth
	if( dwRes != 0)
		throw ChairSerializerException("Invalid or corrupted file");
	bool res = ConvertFromBaseChair( Chairs, ChairLinks, BaseChairArr );
	if(! res )
		throw ChairSerializerException("Error occuried while converting");	
}

void	ChairLoader::LoadFromString( LPCTSTR szDocument, MyChair::ChairSet& Chairs, ChairLinkList_t& ChairLinks, IChairSerializer::PositionMap_t& PosMap  )  
{
	UNREFERENCED_PARAMETER(PosMap);
	std::vector<BaseChair> BaseChairArr;
	DWORD dwRes = LoadGaugeHallFromString( szDocument, BaseChairArr );
	// FIXME : to resource or smth
	if( dwRes != 0)
		throw ChairSerializerException("Invalid or corrupted file");
	bool res = ConvertFromBaseChair( Chairs, ChairLinks, BaseChairArr );
	if(! res )
		throw ChairSerializerException("Error occuried while converting");
}

void	ChairLoader::LoadFromBinary(const std::vector<BYTE>& Data, MyChair::ChairSet& Chairs, ChairLinkList_t& ChairLinks, IChairSerializer::PositionMap_t& PosMap  )  
{
	std::vector<BaseChair> BaseChairArr;
	IChairSerializer* pChairSer;
	DWORD dwRes = CreateInterface( CHAIR_SERIALIZER_INTERFACE_0, (void**)&pChairSer );
	// FIXME : to resource or smth
	if( dwRes != 0)
		throw ChairSerializerException("Invalid or corrupted file");
	dwRes = pChairSer->LoadGaugeHallFromBinaryData( &Data[0], Data.size(), BaseChairArr, PosMap );
	pChairSer->Release();
	bool res = ConvertFromBaseChair( Chairs, ChairLinks, BaseChairArr );
	if(! res )
		throw ChairSerializerException("Error occuried while converting");
}