/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: VertexToolSet.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-31
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __VERTEXTOOLSET_HPP__
#define __VERTEXTOOLSET_HPP__


struct CVertexToolSet
{
	enum ToolOptions
	{
		HorizontalAlign,
		VerticalAlign,
		HorizontalMirror,
		VerticalMirror
	};

	static void Align( VertexPtrArray* pVertices, float v, ToolOptions o );
	static void Align( VertexPtrArray* pVertices, ToolOptions o );
	static void Mirror( VertexPtrArray* pVertices, float v, ToolOptions o );
	static void Translate( VertexPtrArray* pVertices, float dx, float dy );
	static void TCTranslate( VertexPtrArray* pVertices, float dx, float dy );
	static void Rotate( VertexPtrArray* pVertices, float a, float x = 0.0f, float y = 0.0f  );
	static void Scale( VertexPtrArray* pVertices, float dx, float dy, float x, float y );
	static void HitTest( VertexPtrArray* pVertices, Vertex& topLeft, Vertex& bottomRight );
	static Vertex* HitTestOnSelected( VertexPtrArray* pVertices, Vertex& point );
	static bool TCHitTest( VertexPtrArray* pVertices, Vertex& point, float fPointRadius );
	static void GetBounds( VertexPtrArray* pVertices, Vertex& topLeft, Vertex& bottomRight );
	static void DeselectAll( VertexPtrArray* pVertices );
};


#endif //__VERTEXTOOLSET_HPP__
