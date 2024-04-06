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
#include "Configuration.hpp"
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
	this->active = false;
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

CNodePtr Vertex::GetChild( int n )
{
	return static_cast<CNodePtr>(NULL);
}