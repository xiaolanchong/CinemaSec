/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Types.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-18
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __TYPES_HPP__
#define __TYPES_HPP__

enum NodeType
{
	SceneNode = 0,
	CameraNode,
	ElementNode,
	GeometryNode,
	VertexNode,
	Undefined = 0xffffffff
};



#endif //__TYPES_HPP__
