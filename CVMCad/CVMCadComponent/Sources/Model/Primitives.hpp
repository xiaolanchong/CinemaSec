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
//#pragma once
#ifndef __PRIMITIVES_HPP__
#define __PRIMITIVES_HPP__

class CNode;
struct Vertex;

typedef unsigned short Index;

typedef std::list<Vertex*> VertexPtrList;
typedef std::list<Vertex*>::iterator VertexPtrListIt;

typedef std::vector<Vertex*> VertexPtrArray;
typedef std::vector<Vertex*>::iterator VertexPtrArrayIt;

typedef std::vector<Vertex> VertexArray;
typedef std::vector<Vertex>::iterator VertexArrayIt;

typedef std::vector<Index> IndexArray;
typedef std::vector<Index>::iterator IndexArrayIt;

//Model's base primitive
struct Vertex : public CNode
{
	Vertex( float x = 0.0f, float y = 0.0f, float tu = 0.0f, float tv = 0.0f, uint color = 0xff0c0c0c );
//	Vertex( const Vertex& v );
	CNodePtr GetChild( int n );

	bool operator==( const Vertex& rhs );

	//x,y vertex pos
	float x;
	float y;
 
	//vertex texcoords 
	float tu;
	float tv;

	//vertex color
	uint color;

	bool selected;
};


struct Edge
{
	int Vertices[2];
};

struct Triangle
{
	int Vertices[3];  // a triangle is just 3 indices into a point array
	float radius;
	Vertex Centre; // cached data for circumcircle calculations

	static bool PointInTriangle( const Vertex& v, const Vertex& p0, const Vertex& p1, const Vertex& p2 );
};


#endif //__PRIMITIVES_HPP__