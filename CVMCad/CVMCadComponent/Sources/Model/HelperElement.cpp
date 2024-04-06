/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: HelperElement.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-22
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"

#include "Exceptions.hpp"
#include "Types.hpp"
#include "Node.hpp"
#include "Primitives.hpp"
#include "HelperElement.hpp"


CHelperElement::CHelperElement()
{
	//m_pGeometry = new CGeometry();

	//m_pGeometry->vertices.push_back( Vertex( 0.0f,0.0f,0.0f, 0.0f ) );
	//m_pGeometry->vertices.push_back( Vertex( 50.0f,0.0f,1.0f, 0.0f ) );
	//m_pGeometry->vertices.push_back( Vertex( 50.0f,50.0f,1.0f, 1.0f ) );
	//m_pGeometry->vertices.push_back( Vertex( 0.0f,50.0f,0.0f, 1.0f ) );

	//m_pGeometry->indices.push_back( 0 );
	//m_pGeometry->indices.push_back( 1 );
	//m_pGeometry->indices.push_back( 3 );
	//m_pGeometry->indices.push_back( 3 );
	//m_pGeometry->indices.push_back( 1 );
	//m_pGeometry->indices.push_back( 2 );

	m_sType = L"HelperElement";
}

CHelperElement::~CHelperElement()
{
//	delete m_pGeometry;
}


CNodePtr CHelperElement::GetChild( int n )
{
	return NULL;
}

void CHelperElement::SetPivot( float x, float y )
{
	m_fPivotX = x;
	m_fPivotX = y;
}

void CHelperElement::GetPivot( float& x, float& y )
{
	x = m_fPivotX;
	y = m_fPivotY;
}