/*
*	CSBackup 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CSBackup.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-07-29
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __CSBACKUP_H__
#define __CSBACKUP_H__

#ifdef CSBACKUP_EXPORTS
#define CSBACKUP_API __declspec(dllexport)
#else
#define CSBACKUP_API __declspec(dllimport)
#endif


struct IDebugOutput;

//ICSBackup interface
struct ICSBackUp
{
	virtual HRESULT Release() = 0;
};

extern "C"
{
	HRESULT CSBACKUP_API CreateCSBackUp(ICSBackUp** ppBackUp, IDebugOutput* pDebugOut);
}

#endif //__CSBACKUP_H__
