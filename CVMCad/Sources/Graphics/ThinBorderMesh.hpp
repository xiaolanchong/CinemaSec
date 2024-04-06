/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ThinBorderMesh.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-16
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __THINBORDERMESH_HPP__
#define __THINBORDERMESH_HPP__

class CMesh;

class CThinBorderMesh : public CMesh
{
public:
	  void Update( CRect* pRect );
	  void Draw();

private:
	MeshVertexArray m_VerticesSource;
};






#endif //__THINBORDERMESH_HPP__
