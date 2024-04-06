/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: RectangleMesh.hpp
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
#ifndef __RECTANGLEMESH_HPP__
#define __RECTANGLEMESH_HPP__

class CRectangleMesh : public CMesh
{
public:
	CRectangleMesh();

public:
	  void Update( CRect* pRect );

private:
	bool m_bCreated;
	MeshVertexArray m_VerticesSource;
	MeshIndexArray m_IndicesSource;
};




#endif //__RECTANGLEMESH_HPP__
