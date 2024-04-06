/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: BorderMesh.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-13
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __BORDERMESH_HPP__
#define __BORDERMESH_HPP__

class CMesh;

class CBorderMesh : public CMesh
{
public:
	CBorderMesh():
	  m_bCreated(false){};

	void Update( CRect* pRect, float fBorderWidth = 3.0f );

private:
	bool m_bCreated;
	MeshVertexArray m_VerticesSource;
	MeshIndexArray m_IndicesSource;

};







#endif //__BORDERMESH_HPP__
