/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Properties.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-10
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __PROPERTIES_HPP__
#define __PROPERTIES_HPP__

using namespace std;

class IProperty
{
public:
	virtual ~IProperty() = 0 {};
public:
	virtual void SetName( const wstring& s )=0;
	virtual void GetName( wstring& s )=0;
	virtual void SetValue( const wstring& s )=0;
	virtual void GetValue( wstring& s )=0;
};

template<typename T>
class CProperty : public IProperty
{
public:
	CProperty(){};
	CProperty( const wstring s ) : name(s){};
	~CProperty(){};

	void SetName( const wstring& s )
	{
        name = s;
	}

	void GetName( wstring& s )
	{
		s = name;
	}

	void SetValue( const wstring& s )
	{
		wistringstream is(s);
		is>>value;
	}

	void GetValue( wstring& s )
	{
		wostringstream os;
		os<<value;
		s = os.str();
	}

	T value;

private:
	wstring name;
};

typedef list<IProperty*> CPropertyList;
typedef list<IProperty*>::iterator CPropertyListIt;

struct CPropertyToolSet
{
	static bool GetValue( CPropertyList* pPropertyList, wstring& name, wstring& value )
	{
		CPropertyListIt pos;
		CPropertyListIt begin_pos = pPropertyList->begin();
		CPropertyListIt end_pos = pPropertyList->end();
		wstring pname;

		for( pos = begin_pos; pos != end_pos; ++pos )
		{
			(*pos)->GetName( pname );
			if( pname == name )
			{
				(*pos)->GetValue( value );
				return true;
			}

		}

		return false;
	}

};


#endif //__PROPERTIES_HPP__
