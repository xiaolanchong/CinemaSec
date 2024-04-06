// ContentSerializer.cpp: implementation of the ContentSerializer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
//#include "csclient.h"
#include "ContentSerializer.h"
//#include <string>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TAG_NAME(x, y)	static LPCTSTR x  =_T(y);

TAG_NAME( szRootElem	,	"ChairContent"	)
TAG_NAME( szFormatAttr	,	"FormatVersion"	)

// ignore this attributes
TAG_NAME( szAddInfo		,	"AdditionalInfo" )
TAG_NAME( szVideoElem   ,	"VideoFile"		)
TAG_NAME( szGaugeElem   ,	"GaugeFile"		)
TAG_NAME( szBgFile		,	"BackgroundFile")

TAG_NAME( szNameAttr  ,		"Name"			)
TAG_NAME( szContentElem ,	"Content"		)
TAG_NAME( szIdAttr		,	"Id"			)
TAG_NAME( szXAttr		,	"X"				)
TAG_NAME( szYAttr		,	"Y"				)
TAG_NAME( szRangeListElem,	"RangeList"		)
TAG_NAME( szRangeElem,		"Range"			)
TAG_NAME( szStartAttr,		"Start"			)
TAG_NAME( szEndAttr,		"End"			)
TAG_NAME( szContentAttr,		"ContentValue"	)
TAG_NAME( szZoneElem,		"Zone"	)

const WORD c_wCurrentFormat = 3;

static	LPCTSTR szContent_Empty		= _T("Empty");
static	LPCTSTR szContent_Unknown	= _T("Unknown");
static	LPCTSTR szContent_Human		= _T("Human");
static	LPCTSTR szContent_Clothes		= _T("Clothes");
//NOTENOTE: can't be used in serializations if everithing is ok 
static	LPCTSTR szContent_Undefine	= _T("Undefine");

LPCTSTR			ContentSerializer::ContentToName( Range::CONTENT cnt )THROW_HEADER (ContentInvalidValue)
{
	switch( cnt )
	{
	case Range::human :		return szContent_Human;
	case Range::clothes :	return szContent_Clothes;
	case Range::unknown :	return szContent_Unknown;
	case Range::empty :		return szContent_Empty;
	case Range::undefined : ASSERT(FALSE);
	}
	throw ContentInvalidValue("ToName:Unknown content value");
#if _MSC_VER < 1300
	return 0;
#endif
}

Range::CONTENT	ContentSerializer::NameToContent( CString cnt )THROW_HEADER (ContentInvalidValue)
{
	if(		cnt == szContent_Human ) return Range::human;
	else if( cnt == szContent_Clothes ) return Range::clothes;
	else if( cnt == szContent_Unknown ) return Range::unknown;
	else if( cnt == szContent_Empty ) return Range::empty;
	else
	{
		throw ContentInvalidValue("ToContent:Unknown content name");
		#if _MSC_VER < 1300
		return Range::undefined;
		#endif
	}
}

using namespace XmlLite;

void	ContentSerializer::SaveRangeList(	XmlLite::XMLElement& el,	const RangeList_t& rl )
{
	RangeList_t::const_iterator it = rl.begin();
	for(; it != rl.end(); ++it)
	{
		XMLElement elRange( el, szRangeElem );
		SaveRange( elRange, *it );
		el << elRange;
	}
}

void	ContentSerializer::SaveRange(		XmlLite::XMLElement& el,	const Range& r )
{
	el << XmlLite::XMLAttribute( el, szStartAttr,	r.m_nStart );
	el << XmlLite::XMLAttribute( el, szEndAttr,		r.m_nEnd );
	LPCTSTR szName = ContentToName( r.m_nContent );
	el << XmlLite::XMLAttribute( el, szContentAttr,	szName );
}

void	ContentSerializer::LoadRangeList(	WORD wVersion, 	XMLElement& el,	RangeList_t& rl )
{
	XMLElement::iterator it = el.begin();
	for(; it != el.end(); ++it)
	{
		Range r(0, 1, Range::unknown);
		XMLElement elRange(*it);
		LoadRange( wVersion, elRange, r);
		rl.push_back( r );
	}
}

void	ContentSerializer::LoadRange(	WORD wVersion, XMLElement& el,	Range& r )
{
	UNREFERENCED_PARAMETER(wVersion);
	int nValue;
	el.GetAttr( szStartAttr, nValue ); r.m_nStart	= nValue;
	el.GetAttr( szEndAttr, nValue ); r.m_nEnd		= nValue;
	std::tstring str;
	el.GetAttr( szContentAttr, str ); r.m_nContent = NameToContent( str.c_str() );
}

//////// V1 /////////////

void	ContentLoaderV1::LoadContent(	WORD wVersion, XMLElement& el,	TrainListV1_t& ctl )
{
	XMLElement::iterator it = el.begin();
	for(; it != el.end(); ++it)
	{
		int nID;
		XMLElement elChair(*it);
		elChair.GetAttr( szIdAttr, nID );
		LoadRangeList( wVersion, elChair, ctl[ nID ] );
	}
}

///////////////////////////// V2 

void	ContentLoaderV2::LoadContent(	WORD wVersion, XMLElement& el,	ZoneTrainListV2_t& ctl )
{
	XMLElement::iterator it = el.begin();
	for(; it != el.end(); ++it)
	{
		int nID;
		XMLElement elZone(*it);
		elZone.GetAttr( szIdAttr, nID );
		//if( ctl.size() <=  DWORD(nID)) ctl nID + 1);
		LoadZone( wVersion, elZone, /*nID,*/ ctl[ nID ] );
	}
}

void	ContentLoaderV2::LoadZone( WORD wVersion, XmlLite::XMLElement& el,	TrainListV2_t& ctl )
{
	XMLElement::iterator it = el.begin();
	for(; it != el.end(); ++it)
	{
		std::tstring strX, strY;
		XMLElement elChair(*it);
		elChair.GetAttr( szXAttr, strX );
		elChair.GetAttr( szYAttr, strY );
		float x = (float)_tcstod( strX.c_str(), 0 );
		float y = (float)_tcstod( strY.c_str(), 0 );
		ctl.push_back( std::make_pair( Vec2f( x, y ), RangeList_t() ) );
		LoadRangeList( wVersion, elChair, ctl.back().second );
	}
}




void ContentSerializer::Convert( const BaseChairArr_t& chSet, const ChairTrainList_t& chTL,	ZoneTrainListV2_t& chIL )
{
	ASSERT( chSet.size() == chTL.size() );
	for( size_t i = 0 ; i < chSet.size(); ++i)
	{
		BaseChairArr_t::const_iterator it = chSet.begin() + i;
		ChairTrainList_t::const_iterator itChTL = chTL.begin() + i;
		if( itChTL != chTL.end() && !chSet[i].curve.empty() && !itChTL->empty())
		{
			Vec2f cnt = vislib::MeanPoint( it->curve.begin(), it->curve.end() );
//			if( chIL.size() <= DWORD(it->cameraNo) ) chIL.resize(it->subFrameNo+1);
			chIL[it->cameraNo ].push_back( std::make_pair( Vec2f( cnt.x, cnt.y ),  *itChTL ) );
		}
	}
	
}

void ContentSerializer::Convert( const BaseChairArr_t& chSet, const ZoneTrainListV2_t& chIL, ChairTrainList_t& chTL)
{
	chTL.resize( chSet.size() );
	BaseChairArr_t::const_iterator it = chSet.begin();
	for(size_t nIndex = 0 ; it != chSet.end(); ++it, ++nIndex)
	{
#ifdef USE_ZONE_NUMBER
		ZoneTrainListV2_t::const_iterator itchIL = chIL.find( it->cameraNo );
#else
		ZoneTrainListV2_t::const_iterator itchIL = chIL.begin( );
#endif
		if( itchIL == chIL.end() ) continue;
		const TrainListV2_t& ChIList = itchIL->second;
		
		for( DWORD i = 0; i < ChIList.size(); ++i)
		{
			 if( vislib::CurveAndPointRelation( it->curve.begin(), it->curve.end(), 
														ChIList[i].first.x, ChIList[i].first.y ) > 0 ) 
			 {
				chTL[ nIndex ] = ChIList[i].second;;
			 }
		}
	}
}

void ContentSerializer::ConvertV1( const BaseChairArr_t& chSet, const ChairTrainList_t& chTL,	TrainListV1_t& TrainList )
{
	ASSERT( chSet.size() == chTL.size() );
	for( size_t i = 0 ; i < chTL.size(); ++i)
	{
		if( !chTL[i].empty() )
		{
			//chTL[ i ] = itchIL->second
			int nIndex = chSet[i].index;
			TrainList[ nIndex ] = chTL[i];
		}
	}
}

void ContentSerializer::ConvertV1( const BaseChairArr_t& chSet, const TrainListV1_t& TrainList, ChairTrainList_t& chTL )
{
	chTL.resize( chSet.size() );
	for( size_t i = 0 ; i < chSet.size(); ++i)
	{
		TrainListV1_t::const_iterator itchIL = TrainList.find( chSet[i].index );
		if( itchIL != TrainList.end() )
		{
			chTL[ i ] = itchIL->second;
		}
	}
}

inline void ThrowContentException(BOOL bResult)
{
	if( !bResult ) throw ContentSerializerException("Invalid or corrupted file");
}

///// save //////////////

void	ContentSaver::Save( LPCWSTR szFileName, 
						   const BaseChairArr_t& chset,
						   const ChairTrainList_t& ctl,
						   const std::wstring& VideoFile,
						   const std::wstring& ChairFile,
						   const std::wstring& BgFile
							) THROW_HEADER (ContentSerializerException)
{
	try
	{
	// start try

	using namespace XmlLite;

	XMLElement elRoot( m_xmlDocument, szRootElem );
	elRoot << XmlLite::XMLAttribute(elRoot, szFormatAttr, c_wCurrentFormat);
	m_xmlDocument << XMLProcInstr(elRoot, _T("xml"), _T("version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\""));
	m_xmlDocument.SetRoot( elRoot );

	XMLElement elContentElem( elRoot, szContentElem );

	XMLElement elAddInfo ( elRoot, szAddInfo);
	elAddInfo	<< XmlLite::XMLAttribute(elRoot, szVideoElem, VideoFile.c_str())
				<< XmlLite::XMLAttribute(elRoot, szGaugeElem, ChairFile.c_str())
				<< XmlLite::XMLAttribute(elRoot, szBgFile,   BgFile.c_str());
	elRoot << elAddInfo;
	elRoot << elContentElem;
	
	ZoneTrainListV2_t ChIL;
	Convert( chset, ctl, ChIL );
	ZoneTrainListV2_t::const_iterator itZoneList = ChIL.begin();
	for( ; itZoneList != ChIL.end(); ++itZoneList )
	{
		XMLElement elZone( elRoot, szZoneElem );
		elZone << XmlLite::XMLAttribute( elRoot, szIdAttr, int(itZoneList->first) );
		elContentElem << elZone;
		TrainListV2_t::const_iterator it = itZoneList->second.begin();
		for( ; it != itZoneList->second.end(); ++it)
		{
			XMLElement elChair(elZone, szRangeListElem );
			TCHAR Buf[MAX_PATH];
			_sntprintf( Buf, MAX_PATH, _T("%f"), it->first.x );
			elChair << XmlLite::XMLAttribute( elRoot, szXAttr, /*std::tstring*/(Buf) ); 
			_sntprintf( Buf, MAX_PATH, _T("%f"), it->first.y );
			elChair << XmlLite::XMLAttribute( elRoot, szYAttr, /*std::tstring*/(Buf) ); 
			//elChair << XmlLite::XMLAttribute( elRoot, szYAttr, it->second );  
			SaveRangeList( elChair, it->second );
			elZone << elChair;
		}
	}
	HRESULT hr = m_xmlDocument.m_pDoc->save(CComVariant(szFileName));
	// FIXME : to resource or smth
	if( FAILED(hr)) throw ContentSerializerException("Failed to save file");
	// end try
	}
	catch(XMLException)
	{
		// FIXME : to resource or smth
		throw ("Invalid or corrupted file");
	}	
}

///// load ////////////////



void	ContentLoader::Load(	LPCWSTR szFileName, 
								const BaseChairArr_t& chset,
								ChairTrainList_t& ctl,
								std::wstring& VideoFile,
								std::wstring& ChairFile,
								std::wstring& BgFile
								) const THROW_HEADER (ContentSerializerException)
{
	try
	{
	// start try
	VARIANT_BOOL bSuccess;
	HRESULT hr = m_xmlDocument.m_pDoc->load( CComVariant(szFileName), &bSuccess );
	if( FAILED(hr) )
		throw ContentSerializerException("Invalid or corrupted file");
	using namespace XmlLite;
	XMLElement elRoot = m_xmlDocument.GetRoot();
	ThrowContentException( elRoot.Name() == szRootElem );

	WORD wFormatVersion;
	elRoot.GetAttr( szFormatAttr, wFormatVersion );
	std::tstring str;
	XMLElement::iterator it = elRoot.begin();
	if( wFormatVersion < 3)
	{
		XMLElement elVideo( *it );
		try
		{
			elVideo.GetAttr( szNameAttr, VideoFile ); 
		}
		catch(XMLException)
		{
			VideoFile.clear();
		}
		++it;
		XMLElement elGauge( *it );
		try
		{
		
		elGauge.GetAttr( szNameAttr, ChairFile ); 
		}
		catch(XMLException)
		{
			ChairFile.clear();
		}
		++it;
	}
	else
	{
		XMLElement elAddInfo( *it );
		elAddInfo.GetAttr( szVideoElem, VideoFile );
		elAddInfo.GetAttr( szBgFile,	BgFile );
		elAddInfo.GetAttr( szGaugeElem, ChairFile );
		++it;
	}

	
	XMLElement elContent(*it);
	ctl.resize( chset.size() );
	if( wFormatVersion == 1 )
	{
		TrainListV1_t tlV1;
		ContentLoaderV1::LoadContent(wFormatVersion,  elContent, tlV1);
		ConvertV1( chset, tlV1, ctl );
	}
	else if(  wFormatVersion == 2 || wFormatVersion == 3 )
	{
		ZoneTrainListV2_t ztl;
		ContentLoaderV2::LoadContent(wFormatVersion,  elContent, ztl);
		ContentLoaderV2::Convert( chset, ztl, ctl );
	}

	// end try
	}
	catch(XMLException)
	{
		// FIXME : to resource or smth
		ctl.clear();
		ctl.resize( chset.size() );
		throw ContentSerializerException("Invalid or corrupted file");
	}	
}