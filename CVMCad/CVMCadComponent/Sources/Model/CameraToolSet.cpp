/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CameraToolSet.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-31
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Types.hpp"
#include "Node.hpp"
#include "Primitives.hpp"
#include "Element.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "NodeToolSet.hpp"
#include "VertexToolSet.hpp"
#include "CameraToolSet.hpp"

bool CCameraToolSet::FindActive( CCamera** ppCamera )
{
	//CScene* pSC = CApplicationManager::GetInstance()->GetActiveScene();
	//if( pSC == NULL )
	//	return false;

	//vector<CCamera*> nodes;

	//CNodeToolSet::CollectNodes<CCamera>( pSC, nodes );

	//vector<CCamera*>::iterator pos;
	//vector<CCamera*>::iterator begin_pos = nodes.begin();
	//vector<CCamera*>::iterator end_pos = nodes.end();

	//for( pos = begin_pos; pos != end_pos; ++pos )
	//{
	//	if( (*pos)->GetStatus() )
	//	{
	//		*ppCamera = (*pos);
	//		return true;
	//	}
	//}

	return false;
}

void CCameraToolSet::DeactivateAll()
{
	//CScene* pSC = CApplicationManager::GetInstance()->GetActiveScene();
	//if( pSC == NULL )
	//	return;

	//vector<CCamera*> nodes;

	//CNodeToolSet::CollectNodes<CCamera>( pSC, nodes );

	//vector<CCamera*>::iterator pos;
	//vector<CCamera*>::iterator begin_pos = nodes.begin();
	//vector<CCamera*>::iterator end_pos = nodes.end();

	//for( pos = begin_pos; pos != end_pos; ++pos )
	//	(*pos)->SetStatus(false);
}

void CCameraToolSet::ApplyNormalMapping( CCamera* pCamera )
{
	//VertexPtrArray vertices;
	//CNodeToolSet::CollectNodes<Vertex>( pCamera, vertices);

	//if( vertices.empty() )
	//	return;
	//
	//Vertex topLeft, bottomRight;
	//CVertexToolSet::GetBounds( &vertices, topLeft, bottomRight );

	//float fBoundWidth = bottomRight.x - topLeft.x;
	//float fBoundHeight = bottomRight.y - topLeft.y;

	////Apply mapping
	//for( size_t i = 0; i < vertices.size(); ++i )
	//{
	//	vertices[i]->tu = ( vertices[i]->x - topLeft.x ) / fBoundWidth;
	//	vertices[i]->tv = ( vertices[i]->y - topLeft.y ) / fBoundHeight;
	//}
 //   
    
}

