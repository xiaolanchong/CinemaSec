/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ElementToolSet.cpp
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
#include "Configuration.hpp"
#include "Node.hpp"
#include "Primitives.hpp"
#include "Element.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "NodeToolSet.hpp"
#include "ElementToolSet.hpp"


bool CElementToolSet::FindActive( CElement** ppElement )
{
	CScene* pSC = CApplicationManager::GetInstance()->GetActiveScene();
	if( pSC == NULL )
		return false;

	list<CElement*> nodes;

	CNodeToolSet::CollectNodes<CElement>( pSC, nodes );

	list<CElement*>::iterator pos;
	list<CElement*>::iterator begin_pos = nodes.begin();
	list<CElement*>::iterator end_pos = nodes.end();

	for( pos = begin_pos; pos != end_pos; ++pos )
	{
		if( (*pos)->GetStatus() )
		{
			*ppElement = (*pos);
			return true;
		}
	}

	return false;
}

void CElementToolSet::DeactivateAll()
{
	CScene* pSC = CApplicationManager::GetInstance()->GetActiveScene();
	if( pSC == NULL )
		return;

	list<CElement*> nodes;

	CNodeToolSet::CollectNodes<CElement>( pSC, nodes );

	list<CElement*>::iterator pos;
	list<CElement*>::iterator begin_pos = nodes.begin();
	list<CElement*>::iterator end_pos = nodes.end();

	for( pos = begin_pos; pos != end_pos; ++pos )
	{
		(*pos)->SetStatus(false);
	}
}

enum COORDTYPE { X_COORD, Y_COORD };
int g_sortCoord = X_COORD;  // 0 means sort by x-coords first, 1 means sort by y-coords first
// a function to sort a Vertex - we may want to sort the array primarily by x or y
// this will called by qsort()
int SortVector(const void* v1, const void* v2)
{
	Vertex* vec1 = (Vertex*)v1;
	Vertex* vec2 = (Vertex*)v2;

	if (g_sortCoord == X_COORD)
	{
		if (vec1->x < vec2->x)
			return -1;
		else if (vec1->x > vec2->x)
			return 1;
		else
		{
			if (vec1->y < vec2->y)
				return -1;
			else if (vec1->y > vec2->y)
				return 1;
			else
				return 0;
		}
	}
	else
	{
		if (vec1->y < vec2->y)
			return -1;
		else if (vec1->y > vec2->y)
			return 1;
		else
		{
			if (vec1->x < vec2->x)
				return -1;
			else if (vec1->x > vec2->x)
				return 1;
			else
				return 0;
		}
	}
}

float Determinant3x3( float* v )
{
	// calculate the determinant of a 3x3 matrix
	float p0 = v[0] * (v[4] * v[8] - v[5] * v[7]);
	float p1 = -v[1] * (v[3] * v[8] - v[5] * v[6]);
	float p2 = v[2] * (v[3] * v[7] - v[4] * v[6]);
	float f = p0 + p1 + p2;
	return f;
}

void CircumCircle( Vertex* centre, float* radius, Vertex* points )
{
	float x1 = points[0].x , x2 = points[1].x, x3 = points[2].x;
	float y1 = points[0].y, y2 = points[1].y, y3 = points[2].y;

	float x1y1 = x1*x1+y1*y1, x2y2 = x2*x2+y2*y2, x3y3 = x3*x3+y3*y3;

	float mat[9];
	mat[0] = x1; mat[1] = y1; mat[2] = 1.0f; mat[3] = x2; mat[4] = y2; mat[5] = 1.0f; mat[6] = x3; mat[7] = y3; mat[8] = 1.0f;
	float a = Determinant3x3(mat);

	mat[0] = x1y1; mat[3] = x2y2; mat[6] = x3y3;
	float bx = Determinant3x3(mat);

	mat[1] = x1; mat[4] = x2; mat[7] = x3;
	float by = - Determinant3x3(mat);

	mat[2] = y1; mat[5] = y2; mat[8] = y3;
	float c = - Determinant3x3(mat);

	float twoA = 2.0f * a;
	centre->x = bx / twoA;
	centre->y = by / twoA;
	//	centre->z = 0.0f;

	*radius = fabsf(sqrtf(bx*bx + by*by - 4*a*c) / twoA);
}

int InCircle( Vertex& point, Vertex& centre, float radius)
{
	float x1 = centre.x, x2 = point.x;
	float y1 = centre.y, y2 = point.y;

	float dist = sqrtf((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
	if (dist < radius)
		return 1;
	else
		return 0;

}

bool CElementToolSet::Triangulate( CElement* pElement )
{
	int i;

	// calculate the "supertriangle"- a triangle that surrounds all the points
	Vertex min,max;

	VertexArray* pVertices;
	IndexArray* pIndices;

	pElement->GetVertices( &pVertices );
	pElement->GetIndices( &pIndices );

	int nNumVertices = (int)(*pVertices).size();

	//If vertices num < 3, doing nothing
	if( nNumVertices < 3 )
		return false;

	min = (*pVertices)[0];
	max = (*pVertices)[0];

	for (i=1; i < nNumVertices; i++)
	{
		if ((*pVertices)[i].x < min.x)
			min.x = (*pVertices)[i].x;
		else if ((*pVertices)[i].x > max.x)
			max.x = (*pVertices)[i].x;

		if ((*pVertices)[i].y < min.y)
			min.y = (*pVertices)[i].y;
		else if ((*pVertices)[i].y > max.y)
			max.y = (*pVertices)[i].y;
	}

	float distX = max.x - min.x;
	float distY = max.y - min.y;
	float mostRange = (distX > distY) ? distX : distY;
	float midX = (max.x + min.x ) / 2.0f;
	float midY = (max.y + min.y ) / 2.0f;

	Vertex superTri[3];
	superTri[0].x = midX - 2.0f * mostRange;
	superTri[0].y = midY - mostRange;

	superTri[1].x = midX;
	superTri[1].y = midY + 2.0f * mostRange;


	superTri[2].x = midX + 2.0f * mostRange;
	superTri[2].y = midY - mostRange;


	// sort our points by the axis with greatest range
	if (mostRange == distX)
	{
		g_sortCoord = X_COORD;
	}
	else
		g_sortCoord = Y_COORD;

	qsort( &((*pVertices))[0], nNumVertices, sizeof(Vertex), SortVector);


	list<Triangle> triList; // I chose a list because we will be doing a lot of random deletions
	vector<Edge> edges;


	typedef list<Triangle>::iterator rTriangleIt;

	rTriangleIt triIt;
	rTriangleIt eraseIt;


	Triangle sTri; // the super triangle
	sTri.Vertices[0] = nNumVertices;
	sTri.Vertices[1] = nNumVertices + 1;
	sTri.Vertices[2] = nNumVertices + 2;

	sTri.radius = -1.0f;  // mark radius invalid (i.e. we haven't calculated the Circumcircle)

	triList.push_back(sTri);


	list<Triangle> triListDone;

	Vertex curPoints[3];

	for (i = 0; i < nNumVertices; i++) // for every vertex
	{
		Vertex& curPoint = (*pVertices)[i];

		int numEdges = 0; // empty the edge buffer
		edges.clear();

		for (triIt = triList.begin(); triIt != triList.end(); )  // for every triangle in the list
		{
			Triangle& curItem = (*triIt);

			int temp;
			for (temp=0; temp < 3; temp++)  // calculate the 3 current vertices
			{
				int idx = curItem.Vertices[temp];
				if ( idx >= nNumVertices ) // does this index correspond to the supertriangle?
				{
					curPoints[temp] = superTri[idx - nNumVertices];
				}
				else  // this index points to the regular points array
					curPoints[temp] = (*pVertices)[idx];
			}

			if (curItem.radius == -1.0f)  // do we need to generate the circumcircle for this triangle?
				CircumCircle( &curItem.Centre, &curItem.radius, &curPoints[0]);

			// decide if we are finished with this triangle
			// since the points are sorted, once we find a point outside the circumcircle of this triangle, 
			// no points beyond that will ever be inside it again.  move this triangle to the 'done' list
			if (g_sortCoord == X_COORD)
			{
				if ((curPoint.x - curItem.Centre.x) > curItem.radius)
				{
					triListDone.push_back(curItem);
					triList.erase(triIt++);
					continue;
				}
			}
			else
			{
				if ((curPoint.y - curItem.Centre.y) > curItem.radius)
				{
					triListDone.push_back(curItem);
					triList.erase(triIt++);
					continue;
				}

			}

			// is the current point inside the circumcircle?
			if ( InCircle(curPoint, curItem.Centre, curItem.radius)) 
			{
				// yes, so add the 3 edges into our edge buffer

				//        A
				//        *
				//       * *
				//      *   *
				//     *  X  *
				//    *       *
				//   B*********C

				// add edges AC, CB, and BA to the edge list

				Edge e0,e1,e2;
				e0.Vertices[0] = curItem.Vertices[0];
				e0.Vertices[1] = curItem.Vertices[1];
				e1.Vertices[0] = curItem.Vertices[1];
				e1.Vertices[1] = curItem.Vertices[2];
				e2.Vertices[0] = curItem.Vertices[2];
				e2.Vertices[1] = curItem.Vertices[0];

				edges.push_back(e0);
				edges.push_back(e1);
				edges.push_back(e2);
				numEdges+=3;

				// now remove triangle from triangle list
				eraseIt = triIt;
				triIt--;
				triList.erase(eraseIt);
			}

			++triIt;
		}

		int j,k;

		// remove all edges that are in the list twice
		for (j = 0; j < numEdges-1; j++)
		{
			Edge& e0 = edges[j];
			for (k=j+1; k < numEdges; k++)
			{
				Edge& e1 = edges[k];
				if ((e0.Vertices[0] == e1.Vertices[1]) && (e0.Vertices[1] == e1.Vertices[0]))
				{
					e0.Vertices[0] = -1;  // just mark these invalid, so we don't have to remove them from the array
					e0.Vertices[1] = -1;
					e1.Vertices[0] = -1;
					e1.Vertices[1] = -1;
				}
			}
		}

		for (j=0; j < numEdges; j++)
		{
			Edge& e0 = edges[j];
			if ( (e0.Vertices[0] != -1) && (e0.Vertices[1] != -1) )
			{

				Triangle newTri;   // add this triangle to the list
				newTri.Vertices[0] = e0.Vertices[0];
				newTri.Vertices[1] = e0.Vertices[1];
				newTri.Vertices[2] = i;
				newTri.radius = -1.0f;
				triList.push_back(newTri);
			}
		}
	}

	// add all the triangles that were marked "done" above
	triList.insert(triList.end(), triListDone.begin(), triListDone.end());  

	// remove all the triangles that include a supertriangle vertex
	for (triIt = triList.begin(); triIt != triList.end(); triIt++)
	{
		Triangle curItem = *triIt;
		if ((curItem.Vertices[0] >= nNumVertices) 
			|| (curItem.Vertices[1] >= nNumVertices) 
			|| (curItem.Vertices[2] >= nNumVertices ))
		{
			eraseIt = triIt;
			triIt--;
			triList.erase(eraseIt);
		}
	}

	//	*numTris = triList.size();
	pIndices->clear();

	for( list<Triangle>::iterator i = triList.begin(); i != triList.end(); ++i )
	{
		Triangle t = *i;
		pIndices->push_back( t.Vertices[2] );
		pIndices->push_back( t.Vertices[1] );
		pIndices->push_back( t.Vertices[0] );
	}
	return true;
}


