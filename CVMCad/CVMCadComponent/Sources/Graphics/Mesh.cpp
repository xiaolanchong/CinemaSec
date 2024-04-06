/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Mesh.cpp
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
#include "Model.hpp"
#include "GraphicsDevice.hpp"

#include "Mesh.hpp"

CMesh::CMesh()
{
	m_pVB = NULL;
	m_pIB = NULL;

	m_nIndicesNum = 0;
	m_nVerticesNum = 0;
	m_nIndicesMaxNum = 0;
	m_nVerticesMaxNum = 0;
	m_nPrimitivesNum = 0;
}

CMesh::~CMesh()
{
	if( m_pVB != NULL )
	{
		m_pVB->Release();
		m_pVB = NULL;
	}

	if( m_pIB != NULL )
	{
		m_pIB->Release();
		m_pIB = NULL;
	}
}


void CMesh::Initialize()
{
	if( m_pVB != NULL )
	{
		m_pVB->Release();
		m_pVB = NULL;
	}

	if( m_pIB != NULL )
	{
		m_pIB->Release();
		m_pIB = NULL;
	}

	IDirect3DDevice9* pDevice = CGraphicsDevice::GetInstance()->GetDirect3DDevice();

	if( m_nVerticesMaxNum > 0 )
	{
		//Create VB for mesh
		if( FAILED( pDevice->CreateVertexBuffer( m_nVerticesMaxNum * sizeof(MeshVertex),
			D3DUSAGE_DYNAMIC /*Usage*/, D3D_VERTEX_FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL ) ) )
			throw CBaseException<CGraphicsDevice>( L"CMesh", L"Initialize()", L"Can't create vertex buffer for the mesh", L"" );
	}

	if( m_nIndicesMaxNum > 0 )
	{
		//Create IB for mesh
		if( FAILED( pDevice->CreateIndexBuffer( m_nIndicesMaxNum * sizeof(MeshIndex),
			D3DUSAGE_DYNAMIC /*Usage*/, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &m_pIB, NULL ) ) )
			throw CBaseException<CGraphicsDevice>( L"CMesh", L"Initialize()", L"Can't create index buffer for the mesh", L"" );
	}
}
void CMesh::Update( MeshVertexArray* pVerticesSource, MeshIndexArray* pIndicesSource, 
				   bool bUpdateVS, bool bUpdateIS )
{
	m_nVerticesNum = (unsigned int) pVerticesSource->size();
	if( pIndicesSource != NULL )
	{
		m_nIndicesNum = (unsigned int) pIndicesSource->size();
		m_nPrimitivesNum = (unsigned int)(m_nIndicesNum / 3);
	}
	else
	{
		m_nIndicesNum = 0;
		m_nPrimitivesNum = 0;
	}

	//If in VB or IB not enough room for new vertices, recreate them
	if( m_nVerticesNum >= m_nVerticesMaxNum || m_nIndicesNum >= m_nIndicesMaxNum )
	{
		//Set new vertices 
		m_nVerticesMaxNum = m_nVerticesNum + 25;
		if( pIndicesSource != NULL )
			m_nIndicesMaxNum = m_nIndicesNum + 50;
		else
			m_nIndicesMaxNum = 0;

		Initialize();
	}

	void* pVertices;
	void* pIndices;

	if( bUpdateVS )
	{
		//Populate vertex buffer
		if( FAILED( m_pVB->Lock( 0, sizeof(MeshVertex)*m_nVerticesNum, (void**)&pVertices, D3DLOCK_DISCARD ) ) )
			throw CBaseException<CGraphicsDevice>( L"CMesh", L"Update()",L"Can't lock vertex buffer(mesh)", L"" );

		memcpy( pVertices, (void*)(&(*pVerticesSource)[0]), sizeof(MeshVertex)*m_nVerticesNum );

		m_pVB->Unlock();
	}

	if( bUpdateIS )
	{
		//Populate index buffer
		if( FAILED( m_pIB->Lock( 0, sizeof(MeshIndex)*m_nIndicesNum, (void**)&pIndices, D3DLOCK_DISCARD ) ) )
			throw CBaseException<CGraphicsDevice>( L"CMesh", L"Update()", L"Can't lock vertex buffer(mesh)", L"" );

		memcpy( pIndices, (void*)(&(*pIndicesSource)[0]), sizeof(MeshIndex)*m_nIndicesNum );

		m_pIB->Unlock();
	}

}


void CMesh::Update( VertexArray* pVerticesSource, IndexArray* pIndicesSource )
{
	m_nVerticesNum = (unsigned int) pVerticesSource->size();
	if( pIndicesSource != NULL )
	{
		m_nIndicesNum = (unsigned int) pIndicesSource->size();
		m_nPrimitivesNum = (unsigned int)(m_nIndicesNum / 3);
	}
	else
	{
		m_nIndicesNum = 0;
		m_nPrimitivesNum = 0;
	}

	//If in VB or IB not enough room for new vertices, recreate them
	if( m_nVerticesNum >= m_nVerticesMaxNum || m_nIndicesNum >= m_nIndicesMaxNum )
	{
		//Set new vertices 
		m_nVerticesMaxNum = m_nVerticesNum + 25;
		if( pIndicesSource != NULL )
			m_nIndicesMaxNum = m_nIndicesNum + 50;
		else
			m_nIndicesMaxNum = 0;

		Initialize();
	}

	//Transform Vertex -> MeshVertex
	m_TempVertices.resize( m_nVerticesNum );
	for( size_t i = 0; i < m_nVerticesNum; ++i )
	{
		m_TempVertices[i].x = (*pVerticesSource)[i].x;
		m_TempVertices[i].y = (*pVerticesSource)[i].y;
		m_TempVertices[i].tu = (*pVerticesSource)[i].tu;
		m_TempVertices[i].tv = (*pVerticesSource)[i].tv;
		m_TempVertices[i].color = (*pVerticesSource)[i].color;
	}


	//Transform Index -> MeshIndex
	m_TempIndices.resize( m_nIndicesNum );
	for( size_t i = 0; i < m_nIndicesNum; ++i )
	{
		//FIXME: If Index type != MeshVertex type
		m_TempIndices[i] = (*pIndicesSource)[i];
	}


	void* pVertices;
	void* pIndices;


	if( m_nVerticesNum > 0 )
	{
		//Populate vertex buffer
		if( FAILED( m_pVB->Lock( 0, sizeof(MeshVertex)*m_nVerticesNum, (void**)&pVertices, D3DLOCK_DISCARD ) ) )
			throw CBaseException<CGraphicsDevice>( L"CMesh", L"Update()",L"Can't lock vertex buffer(mesh)", L"" );

		memcpy( pVertices, (void*)(&m_TempVertices[0]), sizeof(MeshVertex)*m_nVerticesNum );

		m_pVB->Unlock();
	}


	if( m_nIndicesNum > 0 )
	{
		//Populate index buffer
		if( FAILED( m_pIB->Lock( 0, sizeof(MeshIndex)*m_nIndicesNum, (void**)&pIndices, D3DLOCK_DISCARD ) ) )
			throw CBaseException<CGraphicsDevice>( L"CMesh", L"Update()", L"Can't lock vertex buffer(mesh)", L"" );

		memcpy( pIndices, (void*)(&m_TempIndices[0]), sizeof(MeshIndex)*m_nIndicesNum );

		m_pIB->Unlock();
	}
	

}

void CMesh::Draw()
{
	IDirect3DDevice9* pDevice = CGraphicsDevice::GetInstance()->GetDirect3DDevice();

	if( (m_nVerticesNum == 0) || ( m_nIndicesNum == 0) || (m_nPrimitivesNum == 0) )
		return;

	pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(MeshVertex) );
	pDevice->SetIndices( m_pIB );

	if( FAILED ( pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 
		m_nVerticesNum, 0, m_nPrimitivesNum ) ) )
		throw CBaseException<CGraphicsDevice>( L"CMesh", L"Draw()", L"Can't draw(mesh)", L"" );
}








