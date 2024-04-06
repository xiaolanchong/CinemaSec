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
	m_nWidth = 0;
	m_nHeight = 0;
	m_pTexture = NULL;
	m_bInititalized = false;
	m_bStatic = false;
}

CTexture::~CTexture()
{
	if( m_pTexture != NULL )
		m_pTexture->Release();
}

void CTexture::CreateStatic( const wstring& sFileName )
{
	CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();

	if( m_bInititalized || sFileName.empty() )
		return;

	if( FAILED ( D3DXCreateTextureFromFile( pGD->GetDirect3DDevice(), sFileName.c_str(), &m_pTexture ) ) )
		throw CBaseException<CGraphicsDevice>( L"CTexture", L"CreateStatic()", L"Can't create texture", L"" );


	m_bStatic = true;
	m_bInititalized = true;
}

void CTexture::Update( const BITMAPINFO* pBitmapInfo, const BYTE* pSrcData )
{

	if( m_bStatic )
		return;

	if( pBitmapInfo == NULL || pSrcData == NULL )
	{
		m_bInititalized = false;
		return;
	}

	const BITMAPINFOHEADER& bih = pBitmapInfo->bmiHeader;

	if( bih.biWidth != m_nWidth || bih.biHeight != m_nHeight )
	{
		m_nWidth = bih.biWidth;
		m_nHeight = bih.biHeight;
		Initialize();
	}

	bool bPower2;
	if( !(( m_nWidth-1 )&( m_nWidth)) &&
		!(( m_nHeight-1 )&( m_nHeight )) )
		bPower2 = true;
	else
		bPower2 = false;


	D3DLOCKED_RECT rc;
	if( FAILED( m_pTexture->LockRect( 0, &rc, 0, D3DLOCK_DISCARD  )) )
	{
		throw CBaseException<CTexture>( L"CTexture", L"Update()", L"Can't lock texture", L"" );
	}


	BYTE* pDestData = (BYTE*)rc.pBits;

	if( !bPower2 )
	{
		for( DWORD i = 0; i < m_nHeight; ++i, pDestData += rc.Pitch )
		{
			DWORD dwLine = pBitmapInfo->bmiHeader.biHeight - i - 1;
			memcpy( pDestData, pSrcData + dwLine * m_nWidth * 4,  m_nWidth * 4 );
		}
	}
	else
	{

		for( DWORD i = 0; i < m_nHeight; ++i, pDestData += rc.Pitch )
		{
			DWORD dwLine = pBitmapInfo->bmiHeader.biHeight - 2 * i - 1;
			for( DWORD j = 0; j < m_nWidth; ++j)
			{
				DWORD dwSrcOff = 4 * ( dwLine * pBitmapInfo->bmiHeader.biWidth + 2* j );
				pDestData[j * 4 + 0] = pSrcData[ dwSrcOff + 2 ];
				pDestData[j * 4 + 1] = pSrcData[ dwSrcOff + 1 ];
				pDestData[j * 4 + 2] = pSrcData[ dwSrcOff + 0 ];
				pDestData[j * 4 + 3] = 0xFF;
			}
		}

	}
	m_pTexture->UnlockRect(0);

	m_bInititalized = true;
}

void CTexture::Initialize()
{
	CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
	IDirect3DDevice9* pDevice = pGD->GetDirect3DDevice();
	ASSERT(pDevice != NULL );

	//Release old texture
	if( m_pTexture != NULL )
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}

	//Then create new one
	if( FAILED( pDevice->CreateTexture( m_nWidth, m_nHeight, 1, D3DUSAGE_DYNAMIC, 
		D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture, NULL   ) ) )
		throw CBaseException<CGraphicsDevice>( L"CTexture", L"Initialize()", L"Can't create texture", L"" );
}

void CTexture::Set()
{
	if( m_bInititalized )
	{
		CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
		IDirect3DDevice9* pDevice = pGD->GetDirect3DDevice();

		ASSERT(pDevice != NULL );
		ASSERT(m_pTexture != NULL );

		pDevice->SetTexture( 0, m_pTexture );
	}

}

PBITMAPINFO CTexture::CreateBitmapInfoStruct( HBITMAP hBmp )
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD cClrBits;

	// Retrieve the bitmap's color format, width, and height.
	if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
		return NULL;//FIXME throw

	// Convert the color format to a count of bits.
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	// Allocate memory for the BITMAPINFO structure. (This structure
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD
	// data structures.)
	if (cClrBits != 24)
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
		sizeof(BITMAPINFOHEADER) +
		sizeof(RGBQUAD) * (1 << cClrBits));

	// There is no RGBQUAD array for the 24-bit-per-pixel format.
	else
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
		sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1<<cClrBits);

	// If the bitmap is not compressed, set the BI_RGB flag.
	pbmi->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color
	// indices and store the result in biSizeImage.
	// For Windows NT/2000, the width must be DWORD aligned unless
	// the bitmap is RLE compressed. This example shows this.
	// For Windows 95/98, the width must be WORD aligned unless the
	// bitmap is RLE compressed.
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits+31) & ~31)/8
		* pbmi->bmiHeader.biHeight;

	// Set biClrImportant to 0, indicating that all of the
	// device colors are important.
	pbmi->bmiHeader.biClrImportant = 0;

	return pbmi;
}