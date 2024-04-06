/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: BaseException.hpp
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
#ifndef __BASEEXCEPTION_HPP__
#define __BASEEXCEPTION_HPP__

using namespace std;

template<typename T>
class CBaseException
{

public:
	CBaseException( const wstring& sClassName, const wstring& sMethodName, const wstring& sCause, const wstring& sSolution ):
	  m_sClassName(sClassName), m_sMethodName(sMethodName), m_sCause(sCause), m_sSolution(sSolution){};
	virtual ~CBaseException(){};

public:
	virtual void DisplayErrorMessage()
	{
		//::MessageBoxEx( NULL, m_sCause.c_str(), L"CBaseException raised", MB_ICONERROR|MB_OK,0 );
	}

protected:
	wstring m_sClassName;
	wstring m_sMethodName;
	wstring m_sCause;
	wstring m_sSolution;
};



#endif //__BASEEXCEPTION_HPP__
