// ChairSerializer.cpp: implementation of the ChairSerializer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChairSerializerImpl.h"
//#include ""
#include "ChairLink.h"
#include <strsafe.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TAG_NAME(x, y)	static LPCTSTR x  =_T(y);

TAG_NAME( szRootElem	,"ChairGraph"	)
TAG_NAME( szChairElem   ,"Chair"		)
TAG_NAME( szZoneCurve   ,"ZoneCurve"	)
TAG_NAME( szPoint		, "Point"		)
TAG_NAME( szCoordX		, "x"			)
TAG_NAME( szCoordY		, "y"			)
TAG_NAME( szId			, "Id"			)
TAG_NAME( szZoneNumber	, "ZoneNumber"	)	
TAG_NAME( szFormatVersion,"FormatVersion")
TAG_NAME( szChairRoot	, "Chairs"		)
TAG_NAME( szLinkRoot	, "Links"		)
TAG_NAME( szLinkElem	, "ChairLink"	)
TAG_NAME( szFirstAttr	, "FirstId"		)
TAG_NAME( szSecondAttr	, "SecondId"	)
TAG_NAME( szFirstPos	, "FirstPos"	)

TAG_NAME( Elem_Camera	, "Camera"	)
TAG_NAME( Attr_X	,	"x"	)
TAG_NAME( Attr_Y	,	"y"	)

const WORD	c_wFormatVersion_1 = 1;
const WORD	c_wFormatVersion_2 = 2;

ChairSerializer::ChairSerializer()
{

}

ChairSerializer::~ChairSerializer()
{

}

inline Vec2i GetMassCenter( const Vec2fArr& curve )
{
	//
	Vec2i MassCenter(0, 0);
	for(DWORD i=0; i < curve.size(); ++i)
	{
		MassCenter.x += int(curve[i].x);
		MassCenter.y += int(curve[i].y);
	}
	MassCenter.x /= (int)(curve.size());
	MassCenter.y /= (int)(curve.size());
	return MassCenter;
	//
}


void ChairSerializer::ConvertToBaseChair ( const std::vector<ChairDisk_t>& ChairOnDisk, BaseChairArr_t& ChairList) 
{
	std::vector<ChairDisk_t>::const_iterator itrCL = ChairOnDisk.begin();

	int nIndex = 0;
	ChairList.clear();

	for(nIndex = 0;itrCL != ChairOnDisk.end(); ++itrCL, ++nIndex)
	{
		const ChairDisk_t&	cd = *itrCL;

		if( cd.curve.empty() ) 
		{
			TRACE0("Warning: empty chair curve\n");
			continue;
		}

		ChairList.push_back( BaseChair() );
		
		BaseChair&		cc	= ChairList.back();
		cc.index			= cd.id;
		cc.curve			= cd.curve;
		cc.cameraNo		= cd.subFrameNo;
	//	AssignIndex<const Chair*, int>( IndexMap, itrCL->pLeftChair, cd.nLeft );
	}	
}

template <typename PointerType, typename IndexType>  
void AssignIndex(	const std::map<PointerType, IndexType>& IndexMap, 
					PointerType pObject,
					IndexType& Index )
{
		if( pObject )
		{
			std::map<PointerType, IndexType>::const_iterator itrIndex;
			itrIndex = IndexMap.find( pObject );
			ASSERT( itrIndex != IndexMap.end() );
			Index = itrIndex->second;
		}	
		else 
			Index = -1;
}



void ChairSerializer::ConvertFromBaseChair ( const BaseChairArr_t& ChairList, std::vector<ChairDisk_t>& ChairOnDisk) 
{
	BaseChairArr_t::const_iterator itrCL = ChairList.begin();
	std::map<const BaseChair*, int> IndexMap;

	int nIndex = 0;
	for( nIndex = 0;itrCL != ChairList.end(); ++itrCL, ++nIndex)
	{
		IndexMap.insert( std::make_pair( &(*itrCL), nIndex ) );
	}
	itrCL = ChairList.begin();
	ChairOnDisk.clear();
#if 0
	ChairOnDisk.resize(nIndex);
#else
	ChairOnDisk.reserve(nIndex);
#endif

	for(nIndex = 0;itrCL != ChairList.end(); ++itrCL, ++nIndex)
	{
	
		if( !itrCL->curve.empty() )
		{
			ChairOnDisk.push_back( ChairDisk_t() );
			ChairDisk_t& cd = ChairOnDisk.back();
			cd.center		= GetMassCenter( itrCL->curve );
			cd.id			= itrCL->index;
			cd.curve		= itrCL->curve;
			cd.subFrameNo	= itrCL->cameraNo;
	#ifdef SAVE_LINKS	
			AssignIndex<const BaseChair*, int>( IndexMap, itrCL->pLeftChair, cd.nLeft );
	#endif
		}
		else
		{
			OutputDebugString(_T("[ChairSerializer::ConvertFromBaseChair] empty curve\n"));
		}
	}
}

void ChairSerializer::AddLinkListToBaseChair ( const ChairLinkDiskArray_t& LinkOnDisk, BaseChairArr_t& LinkList)
{
	UNREFERENCED_PARAMETER(LinkOnDisk);
	UNREFERENCED_PARAMETER(LinkList);
}

void ChairSerializer::CreateLinkListFromBaseChair ( const BaseChairArr_t& LinkList,	 ChairLinkDiskArray_t& LinkOnDisk) 
{
#ifdef CREATE_LINK
	BaseChairArr_t::const_iterator it = LinkList.begin();
	LinkOnDisk.clear();
	for(; it != LinkList.end(); ++it)
	{
		LinkOnDisk.push_back( ChairLinkDisk () );
		LinkOnDisk.back().m_nFirstId	= it->m_itrFirst->id;
		LinkOnDisk.back().m_nSecondId	= it->m_itrSecond->id;
		LinkOnDisk.back().m_posFirst	= it->m_posFirst;
	}
#endif
}

inline void ThrowChairSerializerException(BOOL bResult)
{
	if( !bResult ) throw ChairSerializerException("Invalid or corrupted file");
}

void ChairSerializer::SaveChair(XmlLite::XMLElement& elChair, const ChairDisk_t& ch)
{
	using namespace XmlLite;
	elChair << XmlLite::XMLAttribute( elChair, szId, ch.id )
			<< XmlLite::XMLAttribute( elChair, szZoneNumber, ch.subFrameNo);
	XMLElement elArr( elChair, szZoneCurve);
	SaveArray(elArr, ch.curve);
	elChair << elArr;
}

void	ChairSerializer::LoadChair(XmlLite::XMLElement& elChair, ChairDisk_t& ch)
{
	using namespace XmlLite;
	ThrowChairSerializerException( elChair.Name() == szChairElem );
	elChair.GetAttr( szId, ch.id );
	elChair.GetAttr( szZoneNumber, ch.subFrameNo );
	
	XMLElement elArr( *elChair.begin());
	ThrowChairSerializerException( elArr.Name() == szZoneCurve );
	LoadArray(elArr, ch.curve);
}

void ChairSerializer::SaveLink(XmlLite::XMLElement& elLink, const ChairLinkDisk& cld  )
{
	using namespace XmlLite;
	elLink	<< XmlLite::XMLAttribute( elLink, szFirstAttr, cld.m_nFirstId )
			<< XmlLite::XMLAttribute( elLink, szSecondAttr, cld.m_nSecondId )
			<< XmlLite::XMLAttribute( elLink, szFirstPos, (WORD)cld.m_posFirst );
}

void ChairSerializer::LoadLink(XmlLite::XMLElement& elLink, ChairLinkDisk& cld  )
{
	UNREFERENCED_PARAMETER(cld);
	UNREFERENCED_PARAMETER(elLink);
}

void	ChairSerializer::SaveArray(XmlLite::XMLElement& elArray, const Vec2fArr& ch)
{
	using namespace XmlLite;
	for(DWORD i=0; i < ch.size(); ++i)
	{
		XMLElement elPoint(elArray, szPoint);

		// NOTE : we have some problem on float & double value
		// since local settings may be changed
		// fractal delimiter . or , we use only dot explicitly
		TCHAR Buf[MAX_PATH];
		_sntprintf(Buf, MAX_PATH, _T("%f"), ch[i].x );
		elPoint	<< XmlLite::XMLAttribute( elPoint,  szCoordX, Buf ) ;
		_sntprintf(Buf, MAX_PATH, _T("%f"), ch[i].y );
		elPoint	<< XmlLite::XMLAttribute( elPoint,  szCoordY, Buf ); 
		elArray << elPoint;	
	}
}

void	ChairSerializer::LoadArray(XmlLite::XMLElement& elArray, Vec2fArr& ch)
{
	using namespace XmlLite;
	XMLElement::iterator itr = elArray.begin();
	for(; itr != elArray.end(); ++itr)
	{
		XMLElement elItem( *itr );
		Vec2f vec;
		ThrowChairSerializerException( elItem.Name() == szPoint );
		// NOTE : we have some problem on float & double value
		// since local settings may be changed
		// fractal delimiter . or , we use only dot explicitly
		std::tstring d;
		elItem.GetAttr( szCoordX, d );	
		vec.x =  float ( _tcstod( d.c_str(), 0) );
		elItem.GetAttr( szCoordY, d ); 
		vec.y =  float( _tcstod( d.c_str(), 0) );
		ch.push_back( vec );
	}	
}

void	ChairSerializer::SaveChairForCamera(XmlLite::XMLElement& elChair, const ChairDisk_t& ch)
{
	using namespace XmlLite;
	elChair << XmlLite::XMLAttribute( elChair, szId, ch.id );
	SaveArrayWithCompression(elChair, ch.curve);
}

void	ChairSerializer::LoadChairForCamera(XmlLite::XMLElement& elChair, ChairDisk_t& ch)
{
	using namespace XmlLite;
	ThrowChairSerializerException( elChair.Name() == szChairElem );
	elChair.GetAttr( szId, ch.id );

	LoadArrayWithCompression(elChair, ch.curve);
}

void	ChairSerializer::SaveArrayWithCompression(XmlLite::XMLElement& elArray, const Vec2fArr& ch)
{
	std::tstring sFullArr;
	sFullArr.reserve( 50 * 1024 );
	TCHAR szBuf[10];
	for( size_t i = 0 ; i < ch.size(); ++i )
	{
		StringCchPrintf( szBuf, 10, _T("%.2f "), (double)ch[i].x );
		sFullArr += szBuf;
		StringCchPrintf( szBuf, 10, _T("%.2f "), (double)ch[i].y );
		sFullArr += szBuf;
	}
	elArray << XmlLite::XMLText ( elArray, sFullArr ); 
}

static double GetDouble( const std::tstring& s, std::tstring::size_type& nFirstPos )
{
	if( nFirstPos == std::tstring::npos  )	return 0.0;
	std::tstring::size_type nSecondFind = s.find_first_of( _T(" "), nFirstPos );
	if( nSecondFind == std::tstring::npos  )
	{
		std::tstring x = s.substr(nFirstPos, s.length() - nFirstPos );
		nFirstPos = nSecondFind ;
		return _tcstod( x.c_str(), NULL );
	}
	else
	{
		std::tstring x = s.substr(nFirstPos, nSecondFind - nFirstPos);
		nFirstPos = nSecondFind  + 1;//> s.length() :  + 1;
		return _tcstod( x.c_str(), NULL );
	}
}

void	ChairSerializer::LoadArrayWithCompression(XmlLite::XMLElement& elArray, Vec2fArr& ch)
{
	ch.clear();
	ch.reserve( 10*1024 );
	std::tstring sFullArr = elArray.GetValue();
	std::tstring::size_type nFirstFind	= 0;
	do 
	{
		double x = GetDouble( sFullArr, nFirstFind );
		double y = GetDouble( sFullArr, nFirstFind );
		ch.push_back( Vec2f( float(x), float(y) ) );
	} 
	while( nFirstFind != std::tstring::npos );
}

///// save //////////////

void	ChairSaverVersion1::SaveToDocument(	XmlLite::XMLDocument& xmlDoc,
											const BaseChairArr_t& Chairs )
{
	std::vector<ChairDisk_t> ChairDiskArray;
	ChairLinkDiskArray_t	 LinkDiskArray;	
	ConvertFromBaseChair( Chairs, ChairDiskArray );
	CreateLinkListFromBaseChair( Chairs, LinkDiskArray);
	using namespace XmlLite;

	XMLElement elRoot( xmlDoc, szRootElem );
	xmlDoc << XMLProcInstr(elRoot, _T("xml"), _T("version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\""));
	elRoot << XmlLite::XMLAttribute(elRoot, szFormatVersion, c_wFormatVersion_1);
	xmlDoc.SetRoot( elRoot );

	XMLElement elChairRoot( elRoot, szChairRoot );
	XMLElement elLinkRoot( elRoot, szLinkRoot );

	elRoot << elChairRoot;
	elRoot << elLinkRoot;
	size_t i;
	for( i = 0; i < ChairDiskArray.size(); ++i)
	{
		XMLElement elChair(elChairRoot, szChairElem);
		SaveChair( elChair, ChairDiskArray[i] );
		elChairRoot << elChair;
	}
	for( i = 0; i < LinkDiskArray.size(); ++i)
	{
		XMLElement elLink(elLinkRoot, szLinkElem);
		SaveLink( elLink, LinkDiskArray[i] );
		elLinkRoot << elLink;
	}	
}

void	ChairSaverVersion2::SaveToDocument(XmlLite::XMLDocument& xmlDoc,
										   const BaseChairArr_t& Chairs, 
										   const IChairSerializer::PositionMap_t& PosMap )
{
	std::vector<ChairDisk_t> ChairDiskArray;
	ConvertFromBaseChair( Chairs, ChairDiskArray );
	using namespace XmlLite;

	XMLElement elRoot( xmlDoc, szRootElem );
	xmlDoc << XMLProcInstr(elRoot, _T("xml"), _T("version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\""));
	elRoot << XmlLite::XMLAttribute(elRoot, szFormatVersion, c_wFormatVersion_2);
	xmlDoc.SetRoot( elRoot );

	XMLElement elChairRoot( elRoot, szChairRoot );

	elRoot << elChairRoot;

	std::map<int, ChairDiskArr_t> ChairMap;
	bool res = DivideByZone( ChairMap, ChairDiskArray ) ;

	std::map<int, ChairDiskArr_t>::iterator it = ChairMap.begin();
	for( ; it != ChairMap.end(); ++it)
	{
		const ChairDiskArr_t& ChairMas = it->second;
		XMLElement elCamera(elChairRoot, Elem_Camera);
		elCamera << XmlLite::XMLAttribute( elChairRoot, szId, it->first );
		IChairSerializer::PositionMap_t::const_iterator itPos = PosMap.find( it->first );
		if( itPos != PosMap.end() )
		{
			elCamera << XmlLite::XMLAttribute( elChairRoot, Attr_X, itPos->second.first );
			elCamera << XmlLite::XMLAttribute( elChairRoot, Attr_Y, itPos->second.second );
		}
		for( size_t i = 0; i < ChairMas.size(); ++i )
		{
			XMLElement elChair(elChairRoot, szChairElem);
			SaveChairForCamera( elChair, ChairMas[i] );
			elCamera << elChair;
		}
		elChairRoot << elCamera;
	}	
}

///// load ////////////////


void	ChairLoaderVersion1::LoadFromDocument(XmlLite::XMLDocument& xmlDoc, BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap)
{
	using namespace XmlLite;
	XMLElement elRoot = xmlDoc.GetRoot();
	ThrowChairSerializerException( elRoot.Name() == szRootElem );

	PosMap.clear();
	WORD wFormatVersion;
	elRoot.GetAttr( szFormatVersion, wFormatVersion );

	XMLElement::iterator itr = elRoot.begin();
	std::vector<ChairDisk_t>		cd;
	ChairLinkDiskArray_t	cld;

	XMLElement elChairRoot( *itr );
	XMLElement::iterator itrChair = elChairRoot.begin();
	for(; itrChair != elChairRoot.end(); ++itrChair)
	{
		cd.push_back(ChairDisk_t());
		XMLElement elChair(*itrChair);
		LoadChair( elChair, cd.back() );	
	}

	ConvertToBaseChair(cd, Chairs);
	AddLinkListToBaseChair( cld, Chairs );
}

void	ChairLoaderVersion2::LoadFromDocument(XmlLite::XMLDocument& xmlDoc, BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap)
{
	using namespace XmlLite;
	XMLElement elRoot = xmlDoc.GetRoot();

	XMLElement::iterator itr = elRoot.begin();
	std::vector<ChairDisk_t>		cd;
	ChairLinkDiskArray_t	cld;

	PosMap.clear();
	XMLElement elChairRoot( *itr );
	XMLElement::iterator itrCamera = elChairRoot.begin();
	for( ; itrCamera != elChairRoot.end(); ++itrCamera)
	{
		XMLElement elCamera(*itrCamera);
//		ThrowChairSerializerException( elCamera.Name() == Elem_Camera );
		int nCameraNo;
		std::pair<int, int> pos ;
		elCamera.GetAttr( szId, nCameraNo );
		
		elCamera.GetAttr( Attr_X, pos.first );
		elCamera.GetAttr( Attr_Y, pos.second );
		PosMap.insert( std::make_pair( nCameraNo, pos ) );

		XMLElement::iterator itrChair = elCamera.begin();
		for( ; itrChair != elCamera.end(); ++itrChair )
		{
			XMLElement elChair(*itrChair);
			cd.push_back(ChairDisk_t());
			LoadChairForCamera( elChair, cd.back() );
			cd.back().subFrameNo = nCameraNo;
		}
	}

	ConvertToBaseChair(cd, Chairs);
	AddLinkListToBaseChair( cld, Chairs );
}

std::auto_ptr<ChairLoaderVersionAbs>	ChairLoaderVersionAbs::GetLoader( XmlLite::XMLDocument& xmlDoc )
{
	using namespace XmlLite;
	XMLElement elRoot = xmlDoc.GetRoot();
	ThrowChairSerializerException( elRoot.Name() == szRootElem );

	WORD wFormatVersion;
	elRoot.GetAttr( szFormatVersion, wFormatVersion );

	switch( wFormatVersion )
	{
	case c_wFormatVersion_1 : return std::auto_ptr<ChairLoaderVersionAbs>( new ChairLoaderVersion1 );
	case c_wFormatVersion_2 : return std::auto_ptr<ChairLoaderVersionAbs>( new ChairLoaderVersion2 );
	default:
		ThrowChairSerializerException( FALSE );
	}
	ThrowChairSerializerException( FALSE );
	return  std::auto_ptr<ChairLoaderVersionAbs>();
}