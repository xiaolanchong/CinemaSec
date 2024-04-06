/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ElementToolSet.hpp
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
#ifndef __ELEMENTTOOLSET_HPP__
#define __ELEMENTTOOLSET_HPP__

class CElement;

struct CElementToolSet
{
	static bool FindActive( CElement** ppElement );
	static void DeactivateAll();
	static bool Triangulate( CElement* pElement );
	static void ApplyNormalMapping( CElement* pElement );
};


#endif //__ELEMENTTOOLSET_HPP__
