/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Primitives.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-10
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Exceptions.hpp"
#include "Types.hpp"
#include "Node.hpp"
#include "Primitives.hpp"

Vertex::Vertex( float x, float y, float tu, float tv, uint color ) 
{
	m_sType = L"Point";

	this->x = x;
	this->y = y;
	this->tu = tu;
	this->tv = tv;
	this->color = color;
	this->selected = false;
}

//Vertex::Vertex( const Vertex& v )
//{
//	m_sType = L"Point";
//
//	this->x = v.x;
//	this->y = v.y;
//	this->tu = v.tu;
//	this->tv = v.tv;
//	this->color = v.color;
//	this->selected = false;
//}

bool Vertex::operator==( const Vertex& rhs )
{
	if( rhs.x == x
		&& rhs.y == y
		&& rhs.tu == tu 
		&& rhs.tv == tv
		&& rhs.color == color )
		return true;

	return false;
}

bool Triangle::PointInTriangle( const Vertex& v, const Vertex& p0, const Vertex& p1, const Vertex& p2 )
{
	Vertex a;
	Vertex b;

	a.x = v.x - p0.x;
	a.y = v.y - p0.y;

	b.x = p1.x - v.x;
	b.y = p1.y - v.y;

	double sa0 = ( a.x * b.y ) - ( a.y * b.x );


	a.x = v.x - p1.x;
	a.y = v.y - p1.y;

	b.x = p2.x - v.x;
	b.y = p2.y - v.y;

	double sa1 = ( a.x * b.y ) - ( a.y * b.x );

	a.x = v.x - p2.x;
	a.y = v.y - p2.y;

	b.x = p0.x - v.x;
	b.y = p0.y - v.y;

	double sa2 = ( a.x * b.y ) - ( a.y * b.x );


	if( sa0 <= 0.0f && sa1 <= 0.0f && sa2 <= 0.0f )
		return true;
    
	return false;
}

CNodePtr Vertex::GetChild( int n )
{
	return static_cast<CNodePtr>(NULL);
}