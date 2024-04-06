/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Primitives.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-10
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __PRIMITIVES_HPP__
#define __PRIMITIVES_HPP__

#define D3D_VERTEX_FVF (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1)

typedef unsigned short Index;


#pragma pack(push)
#pragma pack(1)

struct Vertex
{
	float x;
	float y;
	float z;
	float rhw;

	D3DCOLOR color;
	D3DCOLOR selected;

	float tu;
	float tv;

public:
	Vertex();
	Vertex( float _x, float _y, DWORD _color = 0xff0000ff );
	Vertex( LONG _x, LONG _y, DWORD _color = 0xff0000ff );
	~Vertex();

};

//////////////////////////////////////////////////////////////////////////

struct Edge
{
	int m_vertices[2];
};

//////////////////////////////////////////////////////////////////////////
struct Triangle
{
	int m_vertices[3];  // a triangle is just 3 indices into a point array
	float radius;
	Vertex m_centre; // cached data for circumcircle calculations

};
#pragma pack(pop)

#endif //__PRIM