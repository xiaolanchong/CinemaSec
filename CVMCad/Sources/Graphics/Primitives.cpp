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
#include "Configuration.hpp"
#include "Primitives.hpp"

Vertex::Vertex():
	x(0.0f), y(0.0f), z(0.0f),rhw(1.0f),tu(0.0f), tv(0.0f),color(0xff0000ff)
{
	CApplicationConfig::GetInstance()->GetUnSelectedPointColor( selected );
}

Vertex::Vertex( LONG _x, LONG _y, DWORD _color ):
	z(0.0f),rhw(1.0f),tu(0.0f), tv(0.0f), color(_color)
{
	x = (float)_x;
	y = (float)_y;
	selected = 0;
	CApplicationConfig::GetInstance()->GetUnSelectedPointColor( selected );

}

Vertex::Vertex( float _x, float _y, DWORD _color ):
	x(_x),y(_y),z(0.0f),rhw(1.0f),tu(0.0f), tv(0.0f),color(_color)
{
	selected = 0;
	CApplicationConfig::GetInstance()->GetUnSelectedPointColor( selected );
}


Vertex::~Vertex()
{

}

