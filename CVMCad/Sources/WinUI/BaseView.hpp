/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: BaseView.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-09
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __BASEVIEW_HPP__
#define __BASEVIEW_HPP__

class IBaseView
{
public:
	virtual ~IBaseView() = 0 {};

public:
	virtual void OnUpdate( ) = 0;

};




#endif //__BASEVIEW_HPP__
