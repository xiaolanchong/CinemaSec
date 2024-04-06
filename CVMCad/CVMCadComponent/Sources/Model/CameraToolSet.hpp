/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CameraToolSet.hpp
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
#ifndef __CAMERATOOLSET_HPP__
#define __CAMERATOOLSET_HPP__

struct CCameraToolSet
{
	static bool FindActive( CCamera** ppCamera );
	static void DeactivateAll();
	static void ApplyNormalMapping( CCamera* pCamera );

};


#endif //__CAMERATOOLSET_HPP__
