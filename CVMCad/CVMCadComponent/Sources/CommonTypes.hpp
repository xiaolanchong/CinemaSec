/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CommonTypes.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-15
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __COMMONTYPES_HPP__
#define __COMMONTYPES_HPP__

inline void EVAL_HR( HRESULT _hr ) 
{ if FAILED(_hr) throw(_hr); }


#define WIDTHBYTES(bits) \
	((DWORD)(((bits)+31) & (~31)) / 8)

//////////////////////////////////////////////////////////////////////////
//POD types
//////////////////////////////////////////////////////////////////////////
typedef unsigned int uint;



//////////////////////////////////////////////////////////////////////////
//Destroyer template
//////////////////////////////////////////////////////////////////////////
template<typename T>
class CSingletonDestroyer
{
	T* ptr;
public:
	CSingletonDestroyer():
	  ptr(NULL){}

	~CSingletonDestroyer()
	{
		if( ptr != NULL )
			delete ptr;
	}

	void SetDestroyable( T* p )
	{
		ptr = p;
	}

	T* GetDestroyable()
	{
		return ptr;
	}
};

//////////////////////////////////////////////////////////////////////////
//Convert class template
//////////////////////////////////////////////////////////////////////////
struct Convert
{
	template<typename T>
	static void FromType( T value, std::wstring& sOut )
	{
		std::wostringstream os;
		os<<value;
		sOut = os.str();
	}

	template<typename T>
	static T ToType( std::wstring& sStrValue )
	{		
		T value;
		std::wistringstream is(sStrValue);
		is>>value;
		return value;
	}
};


//////////////////////////////////////////////////////////////////////////
#endif //__COMMONTYPES_HPP__
