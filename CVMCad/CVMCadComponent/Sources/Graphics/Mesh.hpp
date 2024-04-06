/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Mesh.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-15
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __MESH_HPP__
#define __MESH_HPP__

class CMesh
{
public:
	CMesh();
	virtual ~CMesh();

public:
	void Update( VertexArray* pVerticesSource, IndexArray* pIndicesSource );
	void Update( MeshVertexArray* pVerticesSource, MeshIndexArray* pIndicesSource, bool bUpdateVS = true, bool bUpdateIS = true );
	virtual void Draw();

protected:
	virtual void Initialize();

protected:
	MeshVertexArray m_TempVertices;
	MeshIndexArray m_TempIndices;

	uint m_nVerticesNum;
	uint m_nIndicesNum;
	uint m_nVerticesMaxNum;
	uint m_nIndicesMaxNum;
	uint m_nPrimitivesNum;



	IDirect3DVertexBuffer9*	m_pVB;
	IDirect3DIndexBuffer9*	m_pIB;
};



#endif //__MESH_HPP__
