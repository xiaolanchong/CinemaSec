/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Node.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-18
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __NODE_HPP__
#define __NODE_HPP__

using namespace std;
using namespace boost;

class CNode;

typedef CNode* CNodePtr;  
typedef list<CNodePtr> CNodePtrList;
typedef list<CNodePtr>::iterator CNodePtrListIt;

typedef vector<CNodePtr> CNodePtrArray;
typedef vector<CNodePtr>::iterator CNodePtrArrayIt;


class CNode
{
public:
	CNode(){};
	virtual ~CNode(){};

public:
	virtual void Adopt( CNodePtr pNode ){};
	virtual void Orphan( CNodePtr pNode ){};
	virtual CNodePtr GetChild( int n ) = 0;

	virtual void SetName( const wstring& sName );
	virtual void GetName( wstring& sName );
	virtual void GetType( wstring& sType );
protected:
	wstring m_sName;
	wstring m_sType;
};


#endif //__NODE_HPP__
