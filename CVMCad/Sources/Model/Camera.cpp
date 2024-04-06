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
#include "Camera.hpp"

CCamera::CCamera()
{
	m_nIdX = 0;
	m_nIdY = 0;

	m_sType = L"Camera";
	m_bActive = false;
	m_pBitmapInfo  = NULL;
	m_pSrcData = NULL;
	m_pBitmap = NULL;
}

CCamera::CCamera( const CCamera& camera )
{
	m_nIdX = 0;
	m_nIdY = 0;

	m_sType = L"Camera";
	m_bActive = false;
	m_pBitmapInfo  = NULL;
	m_pSrcData = NULL;
	m_pBitmap = NULL;
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

	if( m_pSrcData != NULL )
		delete m_pSrcData;

	if( m_pBitmapInfo != NULL )
		delete m_pBitmapInfo;

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
	ASSERT( pElement != NULL );

	CElementPtrListIt it = 
		std::find( m_ElementList.begin(), m_ElementList.end(), pElement );

	if( it != m_ElementList.end() )
	{
		delete (CElement*)(*it);
		m_ElementList.erase( it );
	}
		
}

void CCamera::SetSource( BITMAPINFO* pBitmapInfo, BYTE* pSrcData  )
{
	if( m_pBitmapInfo != NULL )
	{
        delete m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}

	if( m_pSrcData != NULL )
	{
		delete[] m_pSrcData;
		m_pSrcData = NULL;
	}

	BITMAPINFOHEADER&  bih = pBitmapInfo->bmiHeader;
	uint nBytesPerPixel;
	uint nBitmapSize;

	if( bih.biBitCount == 32 )
		nBytesPerPixel = 4;
	else if( bih.biBitCount == 24 )
		nBytesPerPixel = 3;
	else if( bih.biBitCount == 16 )
		nBytesPerPixel = 2;
	else
		return;	//FIXME throw here

	m_pBitmapInfo = new BITMAPINFO;
	memcpy( m_pBitmapInfo, pBitmapInfo, sizeof( BITMAPINFO ) );

	nBitmapSize = ( bih.biWidth * bih.biHeight)*nBytesPerPixel;

	m_pSrcData = new BYTE[ nBitmapSize ];
	memcpy( m_pSrcData, pSrcData, nBitmapSize );

	//virtual void	Release() = 0;
	//virtual HRESULT LockFrame( const BITMAPINFO * &ppHeader, const BYTE * &ppImage, int   &nImgSize ) = 0;
	//virtual HRESULT UnlockFrame()	= 0;
}


void CCamera::SetSource( const wstring& sFileName )
{
	m_sFileName = sFileName;
}

void CCamera::GetSource( BITMAPINFO** ppBitmapInfo, BYTE** ppSrcData )
{
	*ppBitmapInfo = m_pBitmapInfo; 
	*ppSrcData = m_pSrcData;
}

void CCamera::GetSource( wstring& sFileName )
{
	sFileName = m_sFileName;
}

void CCamera::SetStatus( bool bActive )
{
	m_bActive = bActive;
}

bool CCamera::GetStatus()
{
	return m_bActive;
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

std::pair<int, int> CCamera::GetId()
{
	return make_pair( m_nIdX, m_nIdY );
}

//////////////////////////////////////////////////////////////////////////

