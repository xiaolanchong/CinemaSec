/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: NodeToolSet.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-31
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __NODETOOLSET_HPP__
#define __NODETOOLSET_HPP__


struct CNodeToolSet
{
	//Collect T-type nodes in the tree 
	template<typename T>
	static void CollectNodes( CNodePtr pParentNode, std::list<T*>& outNodeList )
	{
		if( pParentNode == NULL )
			return;

		CNodePtr pChildNode;
		CNodePtrList nodeList;

		//BUG
		T* node;

		nodeList.push_back( pParentNode );

		for( CNodePtrListIt it = nodeList.begin() ; it != nodeList.end(); ++it )
		{
			for( int i = 0 ; pChildNode = (*it)->GetChild(i); ++i )
			{
				if( node = dynamic_cast<T*>(pChildNode) )
					outNodeList.push_back( node );

				nodeList.push_back( pChildNode );
			}
		}
	}

	template<typename T>
	static void CollectNodes( CNodePtr pParentNode, std::vector<T*>& outNodeArray )
	{
		if( pParentNode == NULL )
			return;

		CNodePtr pChildNode;
		T* node;

		CNodePtrArray nodesArray;

		//BUG
		nodesArray.reserve(1000);

	//	m_nodesArray.resize(0);
		nodesArray.push_back( pParentNode );

		for( CNodePtrArrayIt it = nodesArray.begin() ; it != nodesArray.end(); ++it )
		{
			for( int i = 0 ; pChildNode = (*it)->GetChild(i); ++i )
			{
				if( node = dynamic_cast<T*>(pChildNode) )
				{
					outNodeArray.push_back( node );
				}

				nodesArray.push_back( pChildNode );
			}
		}
	}


	//Find the node parent
	static CNodePtr FindParent( CNodePtr pRootNode, CNodePtr pNode )
	{
		ASSERT( pRootNode != NULL );
		ASSERT( pNode != NULL );
		CNodePtr pChildNode;
		CNodePtrList nodes;

		nodes.clear();
		nodes.push_back( pRootNode );

		for( CNodePtrListIt it = nodes.begin() ; it != nodes.end(); ++it )
		{
			for( int i = 0 ; pChildNode = (*it)->GetChild(i); ++i )
			{
				if( pChildNode == pNode )
					return (*it);
				nodes.push_back( pChildNode );
			}
		}
		return CNodePtr();
	}

private:
	static CNodePtrList CNodeToolSet::m_nodesList;
	static CNodePtrArray CNodeToolSet::m_nodesArray;
};



#endif //__NODETOOLSET_HPP__
