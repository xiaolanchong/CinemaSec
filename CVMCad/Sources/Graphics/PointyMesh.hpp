/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: PointyMesh.hpp
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
#ifndef __POINTYMESH_HPP__
#define __POINTYMESH_HPP__

class CMesh;

class CPointyMesh : public CMesh
{
public:
	CPointyMesh();
	virtual ~CPointyMesh();
	void Update( VertexPtrArray* pVerticesSource, uint SelectedColor = 0xffff0000, uint UnSelectedColor = 0xff0000ff );
	void Update( VertexArray* pVerticesSource, uint SelectedColor = 0xffff0000, uint UnSelectedColor = 0xff0000ff );
	void Draw();

private:
	MeshVertexArray m_VerticesSource;
	MeshIndexArray m_IndicesSource;

};




#endif //__POINTYMESH_HPP__
