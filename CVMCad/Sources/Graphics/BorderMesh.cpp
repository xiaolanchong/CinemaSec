/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: BorderMesh.cpp
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
#include "BorderMesh.hpp"

void CBorderMesh::Update( CRect* pRect, float fBorderWidth  )
{
	m_VerticesSource.clear();

	//Outer vertices
	MeshVertex vTopLeft( (float)pRect->left, (float)pRect->top );
	MeshVertex vTopRight( (float)pRect->right, (float)pRect->top );
	MeshVertex vBottomRight( (float)pRect->right, (float)pRect->bottom );
	MeshVertex vBottomLeft( (float)pRect->left, (float)pRect->bottom );

	m_VerticesSource.push_back(vTopLeft);
	m_VerticesSource.push_back(vTopRight);
	m_VerticesSource.push_back(vBottomRight);
	m_VerticesSource.push_back(vBottomLeft);

	//Inner rect
	m_VerticesSource.push_back( MeshVertex(vTopLeft.x + fBorderWidth, vTopLeft.y)  );
	m_VerticesSource.push_back( MeshVertex(vTopRight.x - fBorderWidth, vTopLeft.y) );

	m_VerticesSource.push_back( MeshVertex(vBottomRight.x - fBorderWidth, vBottomRight.y) );
	m_VerticesSource.push_back( MeshVertex(vBottomRight.x + fBorderWidth, vBottomRight.y) );

	//Best inner rect
	m_VerticesSource.push_back( MeshVertex(vTopLeft.x + fBorderWidth, vTopLeft.y + fBorderWidth)  );
	m_VerticesSource.push_back( MeshVertex(vTopRight.x - fBorderWidth, vTopLeft.y - fBorderWidth) );
	m_VerticesSource.push_back( MeshVertex(vBottomRight.x - fBorderWidth, vBottomRight.y - fBorderWidth) );
	m_VerticesSource.push_back( MeshVertex(vBottomRight.x + fBorderWidth, vBottomRight.y + fBorderWidth) );

	bool bUpdateIS = m_bCreated?false:true;

	if( m_bCreated == false )
	{
		m_IndicesSource.push_back( 0 );
		m_IndicesSource.push_back( 4 );
		m_IndicesSource.push_back( 3 );

		m_IndicesSource.push_back( 7 );
		m_IndicesSource.push_back( 3 );
		m_IndicesSource.push_back( 4 );

		m_IndicesSource.push_back( 4 );
		m_IndicesSource.push_back( 5 );
		m_IndicesSource.push_back( 8 );

		m_IndicesSource.push_back( 8 );
		m_IndicesSource.push_back( 5 );
		m_IndicesSource.push_back( 9 );

		m_IndicesSource.push_back( 11 );
		m_IndicesSource.push_back( 10 );
		m_IndicesSource.push_back( 7 );

		m_IndicesSource.push_back( 7 );
		m_IndicesSource.push_back( 10 );
		m_IndicesSource.push_back( 6 );

		m_IndicesSource.push_back( 6 );
		m_IndicesSource.push_back( 5 );
		m_IndicesSource.push_back( 1 );

		m_IndicesSource.push_back( 6 );
		m_IndicesSource.push_back( 1 );
		m_IndicesSource.push_back( 2 );
		m_bCreated = true;
	}

	CMesh::Update( &m_VerticesSource, &m_IndicesSource, true, bUpdateIS );

}
