/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Camera.cpp
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
#include "Configuration.hpp"
#include "Exceptions.hpp"
#include "Types.hpp"
#include "Node.hpp"
#include "Primitives.hpp"
#include "Scene.hpp"
#include "Element.hpp"
#include "HelperElement.hpp"
#include "Graphics.hpp"
#include "Camera.hpp"

CCamera::CCamera(): 
	m_pTexture( new CTexture() )
{
	m_nIdX = 0;
	m_nIdY = 0;

	m_sType = L"Camera";
}

CCamera::CCamera( const CCamera& camera ):
	m_pTexture( new CTexture() )

{
	m_nIdX = 0;
	m_nIdY = 0;

	m_sType = L"Camera";
}


CCamera::~CCamera()
{
	CElementPtrListIt it;
	CElementPtrListIt begin_it = m_ElementList.begin();
	CElementPtrListIt end_it =  m_ElementList.end();

	for( it = begin_it; it != end_it; ++it )
	{
		delete (CElement*)(*it);
	}

}

CNodePtr CCamera::GetChild( int n )
{
	CElementPtrListIt pos;
	CElementPtrListIt begin_pos = m_ElementList.begin();
	CElementPtrListIt end_pos	= m_ElementList.end();

	int i = 0;
	for(  pos = begin_pos; pos != end_pos; ++pos )
		if( n == i++ )
			return static_cast<CNodePtr>(*pos);

	return NULL;
}

CElement* CCamera::AddElement()
{
	CElement* pElement = new CElement();
	m_ElementList.push_back( pElement );
	return pElement;
}

CElement* CCamera::AddElement( const CElement& element )
{
	CElement* pElement = new CElement( element );
	m_ElementList.push_back( pElement );
	return pElement;
}

void CCamera::RemoveElement( CElement* pElement )
{
	assert( pElement != NULL );

	CElementPtrListIt it = 
		std::find( m_ElementList.begin(), m_ElementList.end(), pElement );

	if( it != m_ElementList.end() )
	{
		delete (CElement*)(*it);
		m_ElementList.erase( it );
	}
		
}

void CCamera::SetSource( const BITMAPINFO* pBitmapInfo, const BYTE* pSrcData  )
{
	m_pTexture->Update( pBitmapInfo, pSrcData );
}

CTexture* CCamera::GetTexture()
{
	return m_pTexture.get();
}

void CCamera::SetId( int idx, int idy )
{
	m_nIdX = idx;
	m_nIdY = idy;
}

void CCamera::SetIdX( int idx )
{
	m_nIdX = idx;
}

void CCamera::SetIdY( int idy )
{
	m_nIdY = idy;
}

int CCamera::GetIdX()
{
	return m_nIdX;
}

int CCamera::GetIdY()
{
	return m_nIdY;
}

std::pair<int, int> CCamera::GetId()
{
	return make_pair( m_nIdX, m_nIdY );
}

//////////////////////////////////////////////////////////////////////////

