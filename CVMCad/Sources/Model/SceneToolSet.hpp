/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: SceneToolSet.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-29
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: Scene component helper tools
*
*
*/
#pragma once
#ifndef __SCENETOOLSET_HPP__
#define __SCENETOOLSET_HPP__

typedef list< pair<CNodePtr, MSXML2::IXMLDOMElementPtr>  > NodeElement;

class CScene;

class CSceneToolSet
{
public:
	static void LoadScene( const void* pMemory, size_t size, CScene** ppSceneComponent );
	static void LoadScene( const wstring& sFileName, CScene** ppSceneComponent );
	static void SaveScene( const wstring& sFileName, CScene* pSceneComponent );
private:
	static void PopulateTree( std::vector<MSXML2::IXMLDOMNodePtr>& tree, MSXML2::IXMLDOMNodePtr pRoot  );
	static void PopulateTree( CNodePtrArray& tree, CNode* pRoot  );

};



#endif //__SCENETOOLSET_HPP__
