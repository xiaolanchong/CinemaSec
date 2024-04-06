/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: PointyMesh.cpp
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
#include "Exceptions.hpp"
#include "Configuration.hpp"
#include "Model.hpp"
#include "GraphicsDevice.hpp"
#include "Mesh.hpp"
#include "PointyMesh.hpp"

CPointyMesh::CPointyMesh()
{

}

CPointyMesh::~CPointyMesh()
{
	
}

void CPointyMesh::Update( VertexPtrArray* pVerticesSource, uint SelectedColor, uint UnSelectedColor )
{
	float fPointRadius = 3.0f;

	bool bUpdateIS = true;

	if( pVerticesSource->size() != m_VerticesSource.size() )
		bUpdateIS = true;
	else
		bUpdateIS = false;

	// 1 vertex source = 4 point's vertices
	if( 4 * m_VerticesSource.size() != pVerticesSource->size() )
		m_VerticesSource.resize( 4 * pVerticesSource->size() );

	Vertex* v;
	MeshVertex v0,v1,v2,v3;

	for(  size_t i = 0, j = 0; i < pVerticesSource->size(); ++i, j+=4 )
	{
		v = (*pVerticesSource)[i];

		m_VerticesSource[j].x = v->x - fPointRadius; //BottomLeft
		m_VerticesSource[j].y = v->y + fPointRadius; //BottomLeft

		m_VerticesSource[j+1].x = v->x - fPointRadius;	//TopLeft
		m_VerticesSource[j+1].y = v->y - fPointRadius;	//TopLeft

		m_VerticesSource[j+2].x = v->x + fPointRadius;	//TopRight
		m_VerticesSource[j+2].y = v->y - fPointRadius;	//TopRight

		m_VerticesSource[j+3].x = v->x + fPointRadius;	//BottomRight
		m_VerticesSource[j+3].y = v->y + fPointRadius;	//BottomRight

		if( v->active )
		{
			m_VerticesSource[j].color =
				m_VerticesSource[j+1].color =
				m_VerticesSource[j+2].color =
				m_VerticesSource[j+3].color =
				v->selected ? SelectedColor : UnSelectedColor;
		}
		else
		{

			m_VerticesSource[j].color =
				m_VerticesSource[j+1].color =
				m_VerticesSource[j+2].color =
				m_VerticesSource[j+3].color =
				v->selected ? SelectedColor : UnSelectedColor;

		}

	}

	if( bUpdateIS )
	{
		MeshIndex index = 0;
		m_IndicesSource.resize(0);

		for( size_t i = 0, j = 0; i < pVerticesSource->size(); ++i )
		{
			//First Triangle
			m_IndicesSource.push_back( index   );
			m_IndicesSource.push_back( index+1 );
			m_IndicesSource.push_back( index+2 );

			//Second Triangle
			m_IndicesSource.push_back( index   );
			m_IndicesSource.push_back( index+2 );
			m_IndicesSource.push_back( index+3 );

			index += 4;
		}
	}

	CMesh::Update( &m_VerticesSource, &m_IndicesSource, true, bUpdateIS );
}

void CPointyMesh::Update( VertexArray* pVerticesSource, uint SelectedColor, uint UnSelectedColor )
{
	float fPointRadius = 3.0f;

	bool bUpdateIS = true;

	if( pVerticesSource->size() != m_VerticesSource.size() )
		bUpdateIS = true;
	else
		bUpdateIS = false;

	// 1 source vertex = 4 point's vertices
	if( 4 * m_VerticesSource.size() != pVerticesSource->size() )
		m_VerticesSource.resize( 4 * pVerticesSource->size() );

	MeshVertex v0,v1,v2,v3;

	for(  size_t i = 0, j = 0; i < pVerticesSource->size(); ++i, j+=4 )
	{
		m_VerticesSource[j].x = (*pVerticesSource)[i].x - fPointRadius; //BottomLeft
		m_VerticesSource[j].y = (*pVerticesSource)[i].y + fPointRadius; //BottomLeft

		m_VerticesSource[j+1].x = (*pVerticesSource)[i].x - fPointRadius;	//TopLeft
		m_VerticesSource[j+1].y = (*pVerticesSource)[i].y - fPointRadius;	//TopLeft

		m_VerticesSource[j+2].x = (*pVerticesSource)[i].x + fPointRadius;	//TopRight
		m_VerticesSource[j+2].y = (*pVerticesSource)[i].y - fPointRadius;	//TopRight

		m_VerticesSource[j+3].x = (*pVerticesSource)[i].x + fPointRadius;	//BottomRight
		m_VerticesSource[j+3].y = (*pVerticesSource)[i].y + fPointRadius;	//BottomRight

		if( (*pVerticesSource)[i].active )
		{
			m_VerticesSource[j].color = 
			m_VerticesSource[j+1].color =
			m_VerticesSource[j+2].color =
			m_VerticesSource[j+3].color =
				(*pVerticesSource)[i].selected ? 0xffff00aa : 0xff00ff00;
		}
		else
		{

			m_VerticesSource[j].color =
				m_VerticesSource[j+1].color =
				m_VerticesSource[j+2].color =
				m_VerticesSource[j+3].color =
				(*pVerticesSource)[i].selected ? SelectedColor : UnSelectedColor;

		}

	}

	if( bUpdateIS )
	{
		MeshIndex index = 0;
		m_IndicesSource.resize(0);

		for( size_t i = 0, j = 0; i < pVerticesSource->size(); ++i )
		{
			//First Triangle
			m_IndicesSource.push_back( index   );
			m_IndicesSource.push_back( index+1 );
			m_IndicesSource.push_back( index+2 );

			//Second Triangle
			m_IndicesSource.push_back( index   );
			m_IndicesSource.push_back( index+2 );
			m_IndicesSource.push_back( index+3 );
			index += 4;
		}
	}

	CMesh::Update( &m_VerticesSource, &m_IndicesSource, true, bUpdateIS );
}

void CPointyMesh::Draw()
{
	CMesh::Draw();
}

