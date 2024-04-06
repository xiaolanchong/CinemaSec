/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ThinBorderMesh.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-16
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
#include "ThinBorderMesh.hpp"

void CThinBorderMesh::Update( CRect* pRect )
{
	m_VerticesSource.clear();

	//Outer vertices
	MeshVertex vTopLeft( (float)pRect->left, (float)pRect->top, 0xff000000 );
	MeshVertex vTopRight( (float)pRect->right, (float)pRect->top, 0xff000000 );
	MeshVertex vBottomRight( (float)pRect->right, (float)pRect->bottom, 0xff000000 );
	MeshVertex vBottomLeft( (float)pRect->left, (float)pRect->bottom, 0xff000000 );

	m_VerticesSource.push_back(vTopLeft);
	m_VerticesSource.push_back(vTopRight);
	m_VerticesSource.push_back(vBottomRight);
	m_VerticesSource.push_back(vBottomLeft);
	m_VerticesSource.push_back(vTopLeft);

	CMesh::Update( &m_VerticesSource, 0, true, false );

}

void CThinBorderMesh::Draw()
{
	IDirect3DDevice9* pDevice = CGraphicsDevice::GetInstance()->GetDirect3DDevice();

	if( m_VerticesSource.size() == 0 )
		return;

	pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(MeshVertex) );
	if( FAILED ( pDevice->DrawPrimitive( D3DPT_LINESTRIP, 0, 4 ) ) )
		throw CBaseException<CGraphicsDevice>( L"CThinBorderMesh", L"Draw()", L"Can't draw primitive", L"" );

}
