/*
*	CSBackup 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Exception.hpp
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
#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__

template< typename T >
class CException
{
public:
	CException( const std::wstring& pCause ):m_sCause(pCause){;}
public:
	std::wstring& Cause(){ return m_sCause; }

private:
	std::wstring m_sCause;
};

#endif //__EXCEPTION_HPP__
