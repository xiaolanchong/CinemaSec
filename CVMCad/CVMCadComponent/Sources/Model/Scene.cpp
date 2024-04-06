/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Scene.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-15
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"

#include "Exceptions.hpp"
#include "Types.hpp"
#include "Node.hpp"
#include "Primitives.hpp"
#include "Element.hpp"
#include "Camera.hpp"
#include "Scene.hpp"

CScene::CScene()
{
	m_sType = L"Scene";
}
CScene::~CScene()
{
	CCameraPtrListIt it;
	CCameraPtrListIt begin_it = m_CameraList.begin();
	CCameraPtrListIt end_it =  m_CameraList.end();

	for( it = begin_it; it != end_it; ++it )
		delete (CCamera*)(*it);
	
}

CCamera* CScene::AddCamera()
{
	CCamera* pCamera = new CCamera();
	m_CameraList.push_back( pCamera );
	return pCamera;
}

CCamera* CScene::AddCamera( const CCamera& camera )
{
	CCamera* pCamera = new CCamera( camera );
	m_CameraList.push_back( pCamera );
	return pCamera;
}
void CScene::RemoveCamera( CCamera* pCamera )
{
	CCameraPtrListIt it = 
		std::find( m_CameraList.begin(), m_CameraList.end(), pCamera );

	if( it != m_CameraList.end() )
	{
		delete (CCamera*)(*it);
		m_CameraList.erase( it );
	}
}

CNodePtr CScene::GetChild( int n )
{
	CCameraPtrListIt pos;
	CCameraPtrListIt begin_pos	= m_CameraList.begin();
	CCameraPtrListIt end_pos	= m_CameraList.end();

	int i = 0;
	for(  pos = begin_pos; pos != end_pos; ++pos )
	{
		if( n == i++ )
		{
			return static_cast<CNodePtr>((CCamera*)*pos);
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
