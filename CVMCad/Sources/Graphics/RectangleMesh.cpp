/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: RectangleMesh.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-13
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Exceptions.hpp"
#include "Configuration.hpp"
#include "GraphicsDevice.hpp"
#include "Model.hpp"
#include "Mesh.hpp"
#include "RectangleMesh.hpp"



CRectangleMesh::CRectangleMesh()
{
	m_bCreated = false;

	m_VerticesSource.resize( 4 );
	m_IndicesSource.resize( 6 );
}

void CRectangleMesh::Update( CRect* pRect )
{
	m_VerticesSource.clear();


	//Outer vertices
	MeshVertex v0( (float)pRect->left, (float)pRect->top );
	MeshVertex v1( (float)pRect->right, (float)pRect->top );
	MeshVertex v2( (float)pRect->right, (float)pRect->bottom );
	MeshVertex v3( (float)pRect->left, (float)pRect->bottom );

	v0.color = 0xffffffff;
	v0.tu = 0.0f;
	v0.tv = 0.0f;

	v1.color = 0xffffffff;
	v1.tu = 1.0f;
	v1.tv = 0.0f;

	v2.color = 0xffffffff;
	v2.tu = 1.0f;
	v2.tv = 1.0f;

	v3.color = 0xffffffff;
	v3.tu = 0.0f;
	v3.tv = 1.0f;

	m_VerticesSource.push_back( v0 );
	m_VerticesSource.push_back( v1 );
	m_VerticesSource.push_back( v2 );
	m_VerticesSource.push_back( v3 );

	bool bUpdateIS = m_bCreated?false:true;

	if( !m_bCreated )
	{
		m_IndicesSource.push_back( 0 );
		m_IndicesSource.push_back( 1 );
		m_IndicesSource.push_back( 2 );

		m_IndicesSource.push_back( 0 );
		m_IndicesSource.push_back( 2 );
		m_IndicesSource.push_back( 3 );

		m_bCreated = true;
	}
	
	CMesh::Update( &m_VerticesSource, &m_IndicesSource, true, bUpdateIS );

}

