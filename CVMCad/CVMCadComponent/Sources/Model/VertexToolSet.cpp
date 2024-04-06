/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: VertexToolSet.cpp
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
#include "VertexToolSet.hpp"


void CVertexToolSet::Align( VertexPtrArray* pVertices, float v, ToolOptions o )
{




}

void CVertexToolSet::Mirror( VertexPtrArray* pVertices, float v, ToolOptions o )
{
	VertexPtrArrayIt pos;
	VertexPtrArrayIt begin_pos = pVertices->begin();
	VertexPtrArrayIt end_pos = pVertices->end();
	Vertex* pVertex;
	float dx,dy;

	switch( o )
	{
	case ToolOptions::HorizontalMirror:
		for( pos = begin_pos; pos != end_pos; ++pos )
		{
			pVertex = dynamic_cast<Vertex*>(*pos);
			assert( pVertex != NULL );

			dx = v - pVertex->x;
			pVertex->x = 2*dx;
		}
		break;

	case ToolOptions::VerticalMirror:
		for( pos = begin_pos; pos != end_pos; ++pos )
		{
			pVertex = dynamic_cast<Vertex*>(*pos);
			assert( pVertex != NULL );

			dy = v - pVertex->y;
			pVertex->y = 2*dy;
		}
		break;
	}


}

void CVertexToolSet::Translate( VertexPtrArray* pVertices, float dx, float dy )
{
	VertexPtrArrayIt pos;
	VertexPtrArrayIt begin_pos = pVertices->begin();
	VertexPtrArrayIt end_pos = pVertices->end();
	Vertex* pVertex;

	for( pos = begin_pos; pos != end_pos; ++pos )
	{
		pVertex = dynamic_cast<Vertex*>((*pos));
		assert( pVertex != NULL );

		if( pVertex->selected )
		{
			pVertex->x += dx;
			pVertex->y += dy;
		}
	}
}

void CVertexToolSet::TCTranslate( VertexPtrArray* pVertices, float dx, float dy )
{
	VertexPtrArrayIt pos;
	VertexPtrArrayIt begin_pos = pVertices->begin();
	VertexPtrArrayIt end_pos = pVertices->end();
	Vertex* pVertex;

	for( pos = begin_pos; pos != end_pos; ++pos )
	{
		pVertex = dynamic_cast<Vertex*>((*pos));
		assert( pVertex != NULL );

		if( pVertex->selected )
		{
			//tu
			if( (pVertex->tu + dx) > 1.0f )
				pVertex->tu = 1.0f;
			else if( (pVertex->tu + dx) < 0.0f )
				pVertex->tu = 0.0f;
			else
				pVertex->tu += dx; 

			//tv
			if( (pVertex->tv + dy) > 1.0f )
				pVertex->tv = 1.0f;	
			else if( (pVertex->tv + dy) < 0.0f )
				pVertex->tv = 0.0f;
			else
				pVertex->tv += dy;

		}
	}
}

void CVertexToolSet::Rotate( VertexPtrArray* pVertices, float a, float x, float y )
{
	VertexPtrArrayIt pos;
	VertexPtrArrayIt begin_pos = pVertices->begin();
	VertexPtrArrayIt end_pos = pVertices->end();
	Vertex* pVertex;

	for( pos = begin_pos; pos != end_pos; ++pos )
	{
		pVertex = *pos;

		pVertex->x = cosf(a) * (pVertex->x) + x;
		pVertex->y = sinf(a) * (pVertex->y) + y;
	}
}

void CVertexToolSet::Scale( VertexPtrArray* pVertices, float dx, float dy, float x, float y )
{



}

void CVertexToolSet::HitTest( VertexPtrArray* pVertices, Vertex& topLeft, Vertex& bottomRight )
{
	VertexPtrArrayIt pos;
	VertexPtrArrayIt begin_pos = pVertices->begin();
	VertexPtrArrayIt end_pos = pVertices->end();
	Vertex* pVertex;

	for( pos = begin_pos; pos != end_pos; ++pos )
	{
		pVertex = (*pos);
		pVertex->selected = false;

		if( ( topLeft.x <= pVertex->x) 
			&& ( topLeft.y <= pVertex->y )
			&& ( bottomRight.x >= pVertex->x )
			&& ( bottomRight.y >= pVertex->y ) )
		{
			pVertex->selected = true;
		}

	}
}

bool CVertexToolSet::HitTestOnSelected( VertexPtrArray* pVertices, Vertex& point )
{
	VertexPtrArrayIt pos;
	VertexPtrArrayIt begin_pos = pVertices->begin();
	VertexPtrArrayIt end_pos = pVertices->end();
	Vertex* pVertex;

	float fPointRadius = 3.0f;

	for( pos = begin_pos; pos != end_pos; ++pos )
	{
		pVertex = (*pos);

		if( ( point.x >= pVertex->x - fPointRadius ) 
			&& ( point.x <= pVertex->x + fPointRadius )
			&& ( point.y >= pVertex->y - fPointRadius )
			&& ( point.y <= pVertex->y + fPointRadius ))
		{
			pVertex->selected = true;
			return true;
		}
	}

	return false;
}


bool CVertexToolSet::TCHitTest( VertexPtrArray* pVertices, Vertex& point, float fPointRadius )
{
	VertexPtrArrayIt pos;
	VertexPtrArrayIt begin_pos = pVertices->begin();
	VertexPtrArrayIt end_pos = pVertices->end();
	Vertex* pVertex;

	for( pos = begin_pos; pos != end_pos; ++pos )
	{
		pVertex = (*pos);

		if( ( point.x >= pVertex->tu - fPointRadius ) 
			&& ( point.x <= pVertex->tu + fPointRadius )
			&& ( point.y >= pVertex->tv - fPointRadius )
			&& ( point.y <= pVertex->tv + fPointRadius ))
		{
			pVertex->selected = true;
			return true;
		}
	}

	return false;
}

void CVertexToolSet::GetBounds( VertexPtrArray* pVertices, Vertex& topLeft, Vertex& bottomRight )
{
	if( pVertices->size() == 0 )
		return;

	Vertex min = *((*pVertices)[0]);
	Vertex max = *((*pVertices)[0]);

	for ( size_t i = 1; i < pVertices->size(); ++i )
	{
		if ( (*pVertices)[i]->x < min.x)
			min.x = (*pVertices)[i]->x;
		else if ( (*pVertices)[i]->x > max.x)
			max.x = (*pVertices)[i]->x;

		if ( (*pVertices)[i]->y < min.y )
			min.y = (*pVertices)[i]->y;
		else if ( (*pVertices)[i]->y > max.y)
			max.y = (*pVertices)[i]->y;
	}

	topLeft.x = min.x;
	topLeft.y = min.y;

	bottomRight.x = max.x;
	bottomRight.y = max.y;
}

void CVertexToolSet::Splice( VertexPtrArray* pVertices, float minDX, float minDY )
{


}