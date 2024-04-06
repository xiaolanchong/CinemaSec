//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Классы для загрузки обводки без конкретного алгоритма
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 15.07.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ChairCommonClass.h"
#include "ChairSerializerImpl.h"

void	ChairLoader::Validate( BaseChairArr_t& Chairs ) const
{
	std::set<int> IdSet;
	std::vector<size_t> DuplicatedIndex;
	DuplicatedIndex.reserve( 1024 );
	int nLastID = 0;
	for( size_t i = 0; i < Chairs.size(); ++i )
	{
		std::pair< std::set<int>::iterator, bool > p = IdSet.insert( Chairs[i].index );
		//ASSERT( p.second );
		if( !p.second )
		{
			TRACE2( "<-- Duplicated index=%d cam=%d\n", Chairs[i].index, Chairs[i].cameraNo );
			DuplicatedIndex.push_back( i );
		}
	}
	if( IdSet.rend() != IdSet.rbegin() )
	{
		nLastID = *IdSet.rbegin();
	}
	for( size_t i =0; i < DuplicatedIndex.size(); ++i )
	{
		int nOld = Chairs[i].index;
		Chairs[i].index = ++nLastID;
		TRACE2( "<-- Set for %d index=%d\n", nOld, Chairs[i].index );
	}
}

void	ChairLoader::Load( LPCTSTR szFileName, BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap )  
{
	try
	{
		// start try
		VARIANT_BOOL bSuccess;
		HRESULT hr = m_xmlDocument.m_pDoc->load( CComVariant(szFileName), &bSuccess );
		if( FAILED(hr) )
			throw ChairSerializerException("Invalid or corrupted file");
		LoadFromDocument( Chairs, PosMap );
		// end try
	}
	catch(XMLException)
	{
		// FIXME : to resource or smth
		throw ChairSerializerException("Invalid or corrupted file");
	}	
}

void	ChairLoader::LoadFromString( LPCTSTR szDocument, BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap )  
{
	try
	{
		VARIANT_BOOL bSuccess;
		HRESULT hr = m_xmlDocument.m_pDoc->loadXML( CComBSTR(szDocument), &bSuccess );
		if( FAILED(hr) )
			throw ChairSerializerException("Invalid or corrupted file");
		LoadFromDocument( Chairs, PosMap );
	}
	catch(XMLException)
	{
		// FIXME : to resource or smth
		throw ChairSerializerException("Invalid or corrupted file");
	}	
}

void	ChairLoader::LoadFromBinArray( const void* pData, size_t nSize, BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap )
{
	try
	{
		m_xmlDocument.LoadFromBinary( pData, nSize );
		LoadFromDocument( Chairs, PosMap );
	}
	catch(XMLException)
	{
		// FIXME : to resource or smth
		throw ChairSerializerException("Invalid or corrupted file");
	}	
}

void	ChairLoader::LoadFromDocument(BaseChairArr_t& Chairs, IChairSerializer::PositionMap_t& PosMap)
{
	std::auto_ptr<ChairLoaderVersionAbs > ldr = ChairLoaderVersionAbs::GetLoader( m_xmlDocument );
	ldr->LoadFromDocument( m_xmlDocument, Chairs, PosMap);
	Validate( Chairs );
}

///////////////////

void	ChairSaver::SaveToString( std::tstring& strDocument, const BaseChairArr_t& Chairs, const IChairSerializer::PositionMap_t& PosMap ) 
{
	try
	{
		// start try
		SaveToDocument( Chairs, PosMap  );
		BSTR bstr;
		HRESULT hr = m_xmlDocument.m_pDoc->get_xml(&bstr);
		CComBSTR str(bstr);
		// FIXME : to resource or smth
		if( FAILED(hr)) throw ChairSerializerException("Failed to save file");
		strDocument = str;
		// end try
	}
	catch(XMLException)
	{
		// FIXME : to resource or smth
		throw ChairSerializerException("Invalid or corrupted file");
	}
}

void	ChairSaver::SaveToBinArray( std::vector<BYTE>& Data, const BaseChairArr_t& Chairs, const IChairSerializer::PositionMap_t& PosMap )
{
	try
	{
		// start try
		SaveToDocument( Chairs, PosMap  );
		m_xmlDocument.SaveToBinary( Data ) ;
		// end try
	}
	catch(XMLException)
	{
		// FIXME : to resource or smth
		throw ChairSerializerException("Invalid or corrupted file");
	}
}

void	ChairSaver::Save( LPCTSTR szFileName, const BaseChairArr_t& Chairs, const IChairSerializer::PositionMap_t& PosMap  )
{
	try
	{
		// start try
		SaveToDocument( Chairs, PosMap  );
		HRESULT hr = m_xmlDocument.m_pDoc->save(CComVariant(szFileName));
		// FIXME : to resource or smth
		if( FAILED(hr)) throw ChairSerializerException("Failed to save file");
		// end try
	}
	catch(XMLException)
	{
		// FIXME : to resource or smth
		throw ChairSerializerException("Invalid or corrupted file");
	}	
}

void	ChairSaver::SaveToDocument(const BaseChairArr_t& Chairs, const IChairSerializer::PositionMap_t& PosMap)
{
#if 0
	ChairSaverVersion1 vs;
#else
	ChairSaverVersion2 vs;
#endif
	vs.SaveToDocument( m_xmlDocument, Chairs, PosMap );
}