/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Element.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-15
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Configuration.hpp"
#include "Exceptions.hpp"
#include "Types.hpp"
#include "Node.hpp"
#include "Primitives.hpp"
#include "ElementToolSet.hpp"
#include "Element.hpp"


CElement::CElement()
{
	m_bActive = false;
	m_bWired = false;
	m_sType = L"Element";
}

CElement::CElement( const CElement& element )
{
	m_sType = L"Element";

	//Copy vertices
	std::copy( 
		element.m_Vertices.begin(), 
		element.m_Vertices.end(), 
		m_Vertices.begin() );

	//Copy indices
	std::copy( 
		element.m_Indices.begin(), 
		element.m_Indices.end(), 
		m_Indices.begin() );
}

CElement::~CElement()
{

}

void CElement::SetStatus( bool bActive )
{
	m_bActive = bActive;

	for( size_t i = 0; i < m_Vertices.size(); ++i )
		m_Vertices[i].active = m_bActive;
}
bool CElement::GetStatus()
{
	return m_bActive;
}

void CElement::SetWired( bool bTrue )
{
	m_bWired = bTrue;
}

bool CElement::GetWired()
{
	return m_bWired;
}

CNodePtr CElement::GetChild( int n )
{
	if( n >= (int)m_Vertices.size() )
		return NULL;
	return &m_Vertices[n];
}

void CElement::AddPoint()
{
	Vertex v;
	m_Vertices.push_back( v );
	m_Indices.clear();

	for( size_t i = 0; i < m_Vertices.size(); ++i )
		m_Vertices[i].active = m_bActive;
}

void CElement::AddPoint( const Vertex& v )
{
	m_Vertices.push_back( v );
	m_Indices.clear();

	for( size_t i = 0; i < m_Vertices.size(); ++i )
		m_Vertices[i].active = m_bActive;
}


void CElement::RemovePoint( Vertex* pVertex )
{
	_ASSERT( pVertex != NULL );
	VertexArray::iterator it = 
		find( m_Vertices.begin(), m_Vertices.end(), *pVertex );
	
	if( it != m_Vertices.end() )
	{
		m_Vertices.erase(it);
		m_Indices.clear();
	}
}

void CElement::GetVertices( VertexArray** ppVertices )
{
	*ppVertices = &m_Vertices;
}

void CElement::GetIndices( IndexArray** ppIndices )
{
	*ppIndices = &m_Indices;
}

