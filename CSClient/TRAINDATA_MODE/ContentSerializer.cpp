// ContentSerializer.cpp: implementation of the ContentSerializer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "csclient.h"
#include "ContentSerializer.h"
#include "../../CSChair/interface.h" 
//#include <string>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

inline void ThrowContentException(BOOL bResult)
{
	if( !bResult ) throw ContentSerializerException("Invalid or corrupted file");
}

void ContentSerializer::Convert(		const MyChair::ChairSet& chSet, BaseChairArr_t& BaseChairs  )
{
	size_t nSize = chSet.size();
	BaseChairs.resize( nSize );
	MyChair::ChairSet::const_iterator it = chSet.begin();
	for( size_t i = 0; it != chSet.end(); ++it, ++i)
	{
		BaseChairs[i].cameraNo	= it->subFrameNo;
		BaseChairs[i].curve		= it->curve;
		BaseChairs[i].index		= it->id;
	}
}

void ContentSerializer::Convert(	const BaseChairArr_t& chSet,std::vector<RangeList_t>&	RangeListArr ,  const ChairTrainList_t& TrainList )
{
	RangeListArr.resize( chSet.size() );
	BaseChairArr_t::const_iterator it = chSet.begin();
	for( size_t i = 0; it != chSet.end(); ++it, ++i)
	{
		ChairTrainList_t::const_iterator itChTL = TrainList.find( it->index );
		if( itChTL != TrainList.end() && !it->curve.empty() && !itChTL->second.empty())
		{
			Vec2f cnt = vislib::MeanPoint( it->curve.begin(), it->curve.end() );
			RangeListArr[i] = itChTL->second;
		}
	}
}

void ContentSerializer::Convert(	const BaseChairArr_t& chSet, ChairTrainList_t& TrainList,	const std::vector<RangeList_t>&	RangeListArr )
{
	ASSERT( chSet.size() == RangeListArr.size() );
	for( size_t i = 0; i < chSet.size(); ++i)
	{
	//	ChairTrainList_t::const_iterator itChTL = TrainList.find( it->id );
		if( !RangeListArr[i].empty())
		{
			int nIndex = chSet[i].index;
			TrainList[ nIndex ] = RangeListArr[i];
		}
	}
}

void ContentLoader ::Load(	LPCTSTR szFileName,
						  const MyChair::ChairSet& chset,
						  ChairTrainList_t& ctl, 
						  CString& strVideoFile ,
						  CString& strGaugeFile)  const
{
	UNREFERENCED_PARAMETER(strVideoFile);
	UNREFERENCED_PARAMETER(strGaugeFile);

	BaseChairArr_t				BaseArr;
	std::vector<RangeList_t>	RangeArr;
	Convert( chset, BaseArr );
	

	IChairContentEx *pInt;
	DWORD dwRes = CreateInterface( CHAIR_CONTENT_INTERFACE_EX, (void**)&pInt );
	ASSERT(!dwRes);
	std::wstring sVideo, sChair, sBackground;
	dwRes = pInt->Load( szFileName, BaseArr, RangeArr, sVideo, sChair, sBackground  );
	strVideoFile = sVideo.c_str();
	strGaugeFile = sChair.c_str();
	pInt->Release();
	if( dwRes != S_OK) 
	{
		ctl.clear();
		throw ContentSerializerInvalidFile("Error");
	}
	Convert( BaseArr, ctl, RangeArr  );
} 

void ContentSaver ::Save(	
						 LPCTSTR szFileName, 
						 const MyChair::ChairSet& chset,
						 const ChairTrainList_t& ctl, 
						 CString strVideoFile,
						 CString strGaugeFile)
{
	UNREFERENCED_PARAMETER(strVideoFile);
	UNREFERENCED_PARAMETER(strGaugeFile);

	BaseChairArr_t				BaseArr;
	std::vector<RangeList_t>	RangeArr;
	Convert( chset, BaseArr );
	Convert( BaseArr,RangeArr, ctl   );

	IChairContentEx *pInt;
	DWORD dwRes = CreateInterface( CHAIR_CONTENT_INTERFACE_EX, (void**)&pInt );
	ASSERT(!dwRes);
	dwRes = pInt->Save( szFileName, BaseArr, RangeArr, strVideoFile, strGaugeFile, L"" );
	pInt->Release();
	if( dwRes != S_OK) 
	{
		throw ContentSerializerInvalidFile("Error");
	}
}











