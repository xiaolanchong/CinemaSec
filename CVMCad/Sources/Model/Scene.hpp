/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Scene.hpp
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
#ifndef __SCENE_HPP__
#define __SCENE_HPP__

class CCamera;
class CNode;

typedef std::list<CCamera*> CCameraPtrList;
typedef std::vector<CCamera*> CCameraPtrArray;
typedef CCameraPtrList::iterator CCameraPtrListIt;
typedef CCameraPtrArray::iterator CCameraPtrArrayIt;

class CScene : public CNode
{
public:
	CScene();
	virtual ~CScene();

public:
    CCamera* AddCamera();
	CCamera* AddCamera( const CCamera& camera );
	void RemoveCamera( CCamera* pCamera );
	CNodePtr GetChild( int n );

private:
	CCameraPtrList m_CameraList;
};


#endif //__SCENE_HPP__
