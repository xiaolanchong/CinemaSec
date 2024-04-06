/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Texture.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-30
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Exceptions.hpp"
#include "GraphicsDevice.hpp"
#include "Texture.hpp"

CTexture::CTexture()
{
	m_pTexture = NULL;

	m_nBmpWidth = 0;
	m_nBmpHeight = 0;
	m_nTexWidth = 0;
	m_nTexHeight = 0;
	m_nBmpBPP = 0;
	m_nTexBPP = 0;
	m_nScale = 1;

	m_nBmpPitch = 0;
}

CTexture::~CTexture()
{
	if( m_pTexture != NULL )
		m_pTexture->Release();
}

bool CTexture::GetTexture( IDirect3DTexture9** ppTexture )
{
	if( m_pTexture )
	{
		*ppTexture = m_pTexture;
		return true;
	}
	
    *ppTexture = NULL;
	return false;
	
}

void CTexture::Update( const BITMAPINFO* pBitmapInfo, const BYTE* pSrcData )
{
	BOOL bLocked = FALSE;
	CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
	IDirect3DDevice9* pDevice = pGD->GetDirect3DDevice();

	uint nBmpWidth;
	uint nBmpHeight;
	uint nBmpBPP;

	if( pBitmapInfo == NULL && pSrcData == NULL )
	{
		nBmpWidth = 128;
		nBmpHeight = 128;
        nBmpBPP = 32;
	}
	else
	{
		nBmpWidth = pBitmapInfo->bmiHeader.biWidth;
		nBmpHeight =pBitmapInfo->bmiHeader.biHeight;
		nBmpBPP = pBitmapInfo->bmiHeader.biBitCount;
	}
	
	try {
		if( nBmpWidth != m_nBmpWidth 
			|| nBmpHeight != m_nBmpHeight
			|| nBmpBPP != m_nBmpBPP )
		{
			//Release old texture
			if( m_pTexture != NULL )
			{
				m_pTexture->Release();
				m_pTexture = NULL;
			}


			//Tex format
			if( nBmpBPP == 8 
				|| nBmpBPP == 16 )
			{
				m_TexFormat = D3DFMT_X1R5G5B5;
				m_nBmpBPP = nBmpBPP;
				m_nTexBPP = 16;
			}
			else if( nBmpBPP == 24 
				|| nBmpBPP == 32 )
			{
				m_TexFormat = D3DFMT_X8R8G8B8;
				m_nBmpBPP = nBmpBPP;
				m_nTexBPP = 32;
			}
			else
				throw CBaseException<CGraphicsDevice>( L"CTexture", L"Update()", L"Bitmap have invalid BPP value", L"" );

	
			m_nBmpWidth =  nBmpWidth;
			m_nBmpHeight = nBmpHeight;

			//Shrink if needed
			if( m_nBmpWidth > 400 && m_nBmpHeight > 300 )
			{
				//Divide by 
				m_nScale = 3;
				m_nTexWidth = ( m_nBmpWidth/3 );
				m_nTexHeight =( m_nBmpHeight/3 );
			}
			else
			{
				m_nScale = 1;
				m_nTexWidth = m_nBmpWidth;
				m_nTexHeight = m_nBmpHeight;
			}


			//Then create new one
			if( FAILED( pDevice->CreateTexture( m_nTexWidth, m_nTexHeight, 1, D3DUSAGE_DYNAMIC, 
				m_TexFormat, D3DPOOL_DEFAULT, &m_pTexture, NULL   ) ) )
				throw CBaseException<CGraphicsDevice>( L"CTexture", L"Update()", L"Can't create texture", L"" );
		}

		D3DLOCKED_RECT rc;
		if( FAILED( m_pTexture->LockRect( 0, &rc, 0, D3DLOCK_DISCARD  )) )
			throw CBaseException<CGraphicsDevice>( L"CTexture", L"FillTexture()", L"Can't lock texture", L""  );

		bLocked = TRUE;
		BYTE* pDestData = (BYTE*)rc.pBits;


		uint nTexBytesPerPixel = m_nTexBPP>>3;
		uint nBmpBytesPerPixel = m_nBmpBPP>>3;

		m_nBmpPitch = WIDTHBYTES( m_nBmpWidth * m_nBmpBPP );

		//////////////////////////////////////////////////////////////////////////
		if( pBitmapInfo == NULL && pSrcData == NULL )
		{
			for( DWORD i = 0; i < m_nTexHeight; ++i, pDestData += rc.Pitch )
			{
				for( DWORD j = 0; j < m_nTexWidth; j++ )
				{ 
					pDestData[ nTexBytesPerPixel * j + 0 ] = 0xFF;
					pDestData[ nTexBytesPerPixel * j + 1 ] = 0x00;
					pDestData[ nTexBytesPerPixel * j + 2 ] = 0x00;
					pDestData[ nTexBytesPerPixel * j + 3 ] = 0xFF;
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
   		else if( m_nBmpBPP == 8 || m_nBmpBPP == 16  )
		{
			for( DWORD i = 0; i < m_nTexHeight; ++i, pDestData += rc.Pitch, pSrcData+= ( m_nScale * m_nBmpPitch) )
			{
				for( DWORD j = 0; j < m_nTexWidth; j++ )
				{
					//Normalize color 
					BYTE bYVal = pSrcData[ m_nScale * nBmpBytesPerPixel *j  ]>>3;
					
					WORD z = 0x8000 | WORD( bYVal << 10 ) | WORD( bYVal << 5 ) | WORD( bYVal);
					
					pDestData[ nTexBytesPerPixel * j + 0 ] = LOBYTE(z);
					pDestData[ nTexBytesPerPixel * j + 1 ] = HIBYTE(z);
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		else if( m_nBmpBPP == 24 || m_nBmpBPP == 32 )
		{
			pSrcData = pSrcData + ( m_nBmpHeight - 1 ) * m_nBmpPitch;
			for( DWORD i = 0; i < m_nTexHeight; ++i, pDestData += rc.Pitch, pSrcData -= ( m_nScale * m_nBmpPitch ) )
			{
				for( DWORD j = 0; j < m_nTexWidth; j++ )
				{ 
					pDestData[ nTexBytesPerPixel * j + 0 ] = pSrcData[ m_nScale * nBmpBytesPerPixel * j ];
					pDestData[ nTexBytesPerPixel * j + 1 ] = pSrcData[ m_nScale * nBmpBytesPerPixel * j ];
					pDestData[ nTexBytesPerPixel * j + 2 ] = pSrcData[ m_nScale * nBmpBytesPerPixel * j ];
					pDestData[ nTexBytesPerPixel * j + 3 ] = 0xff;
				}
			}
		}
		else
		{
			if( bLocked == TRUE )
			{
				m_pTexture->UnlockRect(0);
				bLocked = FALSE;
			}

			CBaseException<CGraphicsDevice>( L"CTexture", L"FillTexture()", L"Error: Unknown bmp format ", L""  );
		}


		m_pTexture->UnlockRect(0);
		bLocked = FALSE;
	}
	catch ( CBaseException<CGraphicsDevice> e ) 
	{
		if( bLocked == TRUE )
		{
			m_pTexture->UnlockRect(0);
			bLocked = FALSE;
		}

		//Rethrow
		throw e;
	}



}




