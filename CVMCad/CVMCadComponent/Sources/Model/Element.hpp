/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Element.hpp
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
#ifndef __ELEMENT_HPP__
#define __ELEMENT_HPP__

class CCamera;
class CElement;

typedef std::list<CElement*> CElementPtrList;
typedef std::vector<CElement*> CElementPtrArray;
typedef CElementPtrList::iterator CElementPtrListIt;
typedef CElementPtrArray::iterator CElementPtrArrayIt;


class CElement : public CNode
{
public:
	CElement();
	CElement( const CElement& element );
	virtual ~CElement();

public:
	void AddPoint();
	void AddPoint( const Vertex& v );
	void RemovePoint( Vertex* pVertex );
	CNodePtr GetChild( int n );
	
	void GetVertices( VertexArray** ppVertices );
	void GetIndices( IndexArray** ppIndices );

	void SetStatus( bool bActive );
	bool GetStatus();


protected:
	bool m_bActive;
	VertexArray m_Vertices;
	IndexArray m_Indices;
};



#endif //__ELEMENT_HPP__
