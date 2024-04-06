/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: SceneToolSet.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-29
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: Scene component helper tools
*
*/

#include "Common.hpp"
#include "Configuration.hpp"
#include "Exceptions.hpp"
#include "Model.hpp"
#include "SceneToolSet.hpp"


void CSceneToolSet::PopulateTree( std::vector<MSXML2::IXMLDOMNodePtr>& tree, MSXML2::IXMLDOMNodePtr pRoot  )
{
	MSXML2::IXMLDOMNodeListPtr pXMLNodeList;
	MSXML2::IXMLDOMNodePtr pXMLNode;

	pXMLNodeList = pRoot->childNodes;
	for( long i = 0; i < pXMLNodeList->length; ++i )
	{
		pXMLNode = pXMLNodeList->item[i];
		tree.push_back( pXMLNode );
		CSceneToolSet::PopulateTree( tree, pXMLNode );
	}
}

void CSceneToolSet::PopulateTree( CNodePtrArray& tree, CNode* pRoot  )
{
	MSXML2::IXMLDOMNodeListPtr pXMLNodeList;
	MSXML2::IXMLDOMNodePtr pXMLNode;

	CNode* pNode = NULL;
	for( long i = 0; pNode = pRoot->GetChild(i); ++i )
	{
		tree.push_back( pNode );
		CSceneToolSet::PopulateTree( tree, pNode );
	}
}


void CSceneToolSet::LoadScene( const void* pMemory, size_t size, CScene** ppSceneComponent )
{

	wstring s;
	s.reserve( size + sizeof(wchar_t) );
	memcpy( &s[0], pMemory, size );
	s.push_back( L'\0' );

	LoadScene( s, ppSceneComponent );
}

void CSceneToolSet::LoadScene( const wstring& sFileName, CScene** ppSceneComponent )
{
	CScene* pActiveScene = NULL;

	try
	{

		wstring sNodeName;
		wstring sAttrVal;
		_variant_t varLoadResult((bool)FALSE);
		_variant_t varAttrVal;

		MSXML2::IXMLDOMNodeListPtr pXMLNodeList = NULL;
		MSXML2::IXMLDOMNodePtr pXMLParentNode = NULL;
		MSXML2::IXMLDOMNodePtr pXMLChildNode = NULL;
		MSXML2::IXMLDOMNodePtr pXMLNode = NULL;
		MSXML2::IXMLDOMElementPtr pXMLRootElement = NULL;
		MSXML2::IXMLDOMNamedNodeMapPtr pXMLNamedNodeMap = NULL;
		MSXML2::IXMLDOMDocumentPtr pXMLDoc = NULL;

		CCamera* pLastCamera;
		CElement* pLastElement;


		//Create MSXML DOM object
		EVAL_HR(pXMLDoc.CreateInstance("Msxml2.DOMDocument"));

		varLoadResult = pXMLDoc->load( sFileName.c_str() );
		if( (bool)varLoadResult == FALSE )
			throw CBaseException<CSceneToolSet>( L"CSceneToolSet", L"LoadScene()", L"Can't load XML document", L"" );

		list<MSXML2::IXMLDOMNodePtr> nodes;

		if( FAILED(pXMLDoc->get_documentElement( &pXMLRootElement )) )
			throw CBaseException<CSceneToolSet>( L"CSceneToolSet", L"LoadScene()", L"XML document is not valid", L"" );

		std::vector<MSXML2::IXMLDOMNodePtr> tree;
		PopulateTree( tree, pXMLDoc );

		for( std::vector<MSXML2::IXMLDOMNodePtr>::iterator it = tree.begin(); it != tree.end(); ++it )
		{
			pXMLParentNode = (MSXML2::IXMLDOMNodePtr)(*it);
			pXMLNodeList = pXMLParentNode->childNodes;

			sNodeName = _bstr_t( pXMLParentNode->nodeName );

			if( sNodeName.compare( L"Scene" ) == 0 )
			{
				pActiveScene = new CScene();
				pXMLNamedNodeMap = pXMLParentNode->attributes;
			}
			else if( sNodeName.compare( L"Camera") == 0 )
			{
				pLastCamera = pActiveScene->AddCamera();
				pXMLNamedNodeMap = pXMLParentNode->attributes;

				int idx = 0;
				int idy = 0;
				
				if( pXMLNode = pXMLNamedNodeMap->getNamedItem(L"idx") )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					idx = Convert::ToType<int>( sAttrVal );
				}
				if( pXMLNode = pXMLNamedNodeMap->getNamedItem(L"idy") )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					idy = Convert::ToType<int>( sAttrVal );
				}

				pLastCamera->SetId( idx, idy );


			}
			else if( sNodeName.compare( L"Element" )  == 0 )
			{
				pLastElement = pLastCamera->AddElement();
			}

			else if( sNodeName.compare( L"Vertex" ) == 0 )
			{
				pXMLNamedNodeMap = pXMLParentNode->attributes;
				Vertex v;

				//x
				if( pXMLNode = pXMLNamedNodeMap->getNamedItem( L"x" ) )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					v.x = Convert::ToType<float>( sAttrVal );
				}

				//y
				if( pXMLNode = pXMLNamedNodeMap->getNamedItem( L"y" ) )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					v.y = Convert::ToType<float>( sAttrVal );
				}

				//tu
				if( pXMLNode = pXMLNamedNodeMap->getNamedItem( L"tu" ) )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					v.tu = Convert::ToType<float>( sAttrVal );
				}

				//tv
				if( pXMLNode = pXMLNamedNodeMap->getNamedItem( L"tv" ) )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					v.tv = Convert::ToType<float>( sAttrVal );
				}

				//color
				if ( pXMLNode = pXMLNamedNodeMap->getNamedItem( L"color" ) )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					v.color = Convert::ToType<uint>( sAttrVal );
				}


				
				VertexArray* pVertices;
				pLastElement->GetVertices( &pVertices );
				pVertices->push_back( v );

			}
			else if( sNodeName.compare( L"Index" ) == 0 )
			{
				pXMLNamedNodeMap = pXMLParentNode->attributes;
				//index
				if( pXMLNode = pXMLNamedNodeMap->getNamedItem( L"value" ) )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					Index i = Convert::ToType<uint>( sAttrVal );

					IndexArray* pIndices;
					pLastElement->GetIndices( &pIndices );
					pIndices->push_back( i );
				}

			}

		}

		*ppSceneComponent = pActiveScene;

	}
	catch( ... )
	{
		if( pActiveScene != NULL )
		{
			delete pActiveScene;
			pActiveScene = NULL;
		}

		//rethrow
	}

}

void CSceneToolSet::SaveScene( const wstring& sFileName, CScene* pSceneComponent )
{
	try
	{
		//If scene doesn't exist return
		if( pSceneComponent == NULL )
			return;

		_variant_t varAttrVal;
		MSXML2::IXMLDOMDocumentPtr pXMLDoc = NULL;

		//Create MSXML DOM object
		EVAL_HR(pXMLDoc.CreateInstance("Msxml2.DOMDocument"));


		//XML processing instruction
		MSXML2::IXMLDOMProcessingInstructionPtr pPI = pXMLDoc->createProcessingInstruction
			("xml", "version='1.0' encoding='UTF-8'");


		_variant_t vNullVal;
		vNullVal.vt = VT_NULL;
		pXMLDoc->insertBefore(pPI, vNullVal);

		//File ID node <cmvcad_file version="1.0"/>
		MSXML2::IXMLDOMElementPtr pRootElement = pXMLDoc->createElement( _T("cvmcad_file") );
		pXMLDoc->appendChild( pRootElement );

		varAttrVal = _T("1.0");
		pRootElement->setAttribute( _T("version"), varAttrVal  );

		wstring  sAttrVal;
		CScene* pScene;
		CNodePtr pParentNode;
		CCamera* pCamera;
		CElement* pElement;

		CNodePtrArray nodes;

		MSXML2::IXMLDOMElementPtr pXMLParent = NULL;
		MSXML2::IXMLDOMElementPtr pXMLScene = NULL;
		MSXML2::IXMLDOMElementPtr pXMLCamera = NULL;
		MSXML2::IXMLDOMElementPtr pXMLElement = NULL;


		nodes.push_back( pSceneComponent );

		//Push root element ( Active Scene )
		CSceneToolSet::PopulateTree( nodes, pSceneComponent );

		for( CNodePtrArrayIt it = nodes.begin() ; it != nodes.end(); ++it )
		{
			pParentNode = (*it);

			//CScene
			if( pScene = dynamic_cast<CScene*>(pParentNode) )
			{
				pXMLScene = pXMLDoc->createElement( _T("Scene") );
				pRootElement->appendChild( pXMLScene );
			}

			//CCamera
			else if( pCamera = dynamic_cast<CCamera*>(pParentNode) )
			{
				pXMLCamera = pXMLDoc->createElement( _T("Camera") );
				pXMLScene->appendChild( pXMLCamera );

				std::pair<int,int> val = pCamera->GetId();

				Convert::FromType( val.first, sAttrVal );
				varAttrVal = sAttrVal.c_str();
				pXMLCamera->setAttribute( L"idx", varAttrVal );

				Convert::FromType( val.second, sAttrVal );
				varAttrVal = sAttrVal.c_str();
				pXMLCamera->setAttribute( L"idy", varAttrVal );
			}

			//CElement
			else if( pElement = dynamic_cast<CElement*>(pParentNode) )
			{
				pXMLElement = pXMLDoc->createElement( _T("Element") );
				pXMLCamera->appendChild( pXMLElement );

				VertexArray* pVertices;
				IndexArray* pIndices;

				pElement->GetVertices( &pVertices );
				pElement->GetIndices( &pIndices );

				//<Vertices>
				if( !pVertices->empty() )
				{
					MSXML2::IXMLDOMElementPtr pXMLVertices = pXMLDoc->createElement( _T("Vertices") );
					pXMLElement->appendChild( pXMLVertices );

					for( VertexArrayIt it = pVertices->begin(); it != pVertices->end(); ++it )
					{
						MSXML2::IXMLDOMElementPtr pXMLVertex = pXMLDoc->createElement( _T("Vertex") );
						
						//x
						Convert::FromType( (*it).x, sAttrVal );
						varAttrVal = sAttrVal.c_str();
						pXMLVertex->setAttribute( "x", varAttrVal );

						//y
						Convert::FromType( (*it).y, sAttrVal );
						varAttrVal = sAttrVal.c_str();
						pXMLVertex->setAttribute( "y", varAttrVal );

						//tu
						Convert::FromType( (*it).tu, sAttrVal );
						varAttrVal = sAttrVal.c_str();
						pXMLVertex->setAttribute( "tu", varAttrVal );

						//tv
						Convert::FromType( (*it).tv, sAttrVal );
						varAttrVal = sAttrVal.c_str();
						pXMLVertex->setAttribute( "tv", varAttrVal );

						//color
						Convert::FromType( (*it).color, sAttrVal );
						varAttrVal = sAttrVal.c_str();
						pXMLVertex->setAttribute( "color", varAttrVal );

						pXMLVertices->appendChild( pXMLVertex );
					}
				}

				//<Indices>
				if( !pIndices->empty() )
				{
					MSXML2::IXMLDOMElementPtr pXMLIndices = pXMLDoc->createElement( _T("Indices") );
					pXMLElement->appendChild( pXMLIndices );

					for( IndexArrayIt it = pIndices->begin(); it != pIndices->end(); ++it )
					{
						MSXML2::IXMLDOMElementPtr pXMLIndex = pXMLDoc->createElement( _T("Index") );

						//index
						uint val = (*it);
						Convert::FromType( val, sAttrVal );
						varAttrVal = sAttrVal.c_str();
						pXMLIndex->setAttribute( L"value", varAttrVal );
						pXMLIndices->appendChild( pXMLIndex );
					}
				}
			}

		}

		pXMLDoc->save( sFileName.c_str() );
	}

	catch(...)
	{
		//FIX
		//rethrow
	}


}