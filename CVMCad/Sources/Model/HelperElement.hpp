/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: HelperElement.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-22
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __HELPERELEMENT_HPP__
#define __HELPERELEMENT_HPP__


using namespace std;
using namespace boost;

class CCamera;
class CElement;

class CHelperElement : public CElement
{
public:
	CHelperElement();
	virtual ~CHelperElement();

public:
	CNodePtr GetChild( int n );

	void AddPoint();
	void AddPoint( const Vertex& v );
	void RemovePoint( Vertex* v );

	void SetPivot( float x, float y );
	void GetPivot( float& x, float& y );



protected:
	float m_fPivotX;
	float m_fPivotY;
};



#endif //__HELPERELEMENT_HPP__
