/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CommandExecutor.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-19
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __COMMANDEXECUTOR_HPP__
#define __COMMANDEXECUTOR_HPP__

class ICommandExecutor
{
public:
	virtual void Enable( uint nID, bool bEnable ) = 0;
	virtual void Check( uint nID, bool bCheck ) = 0;
};


#endif //__COMMANDEXECUTOR_HPP__
