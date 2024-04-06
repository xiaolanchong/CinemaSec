/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Texture.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-30
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__



//virtual void	Release() = 0;
//virtual HRESULT LockFrame( const BITMAPINFO * &ppHeader, const BYTE * &ppImage, int   &nImgSize ) = 0;
//virtual HRESULT UnlockFrame()	= 0;

class CTexture
{
public:
	CTexture();
	virtual ~CTexture();

public:
	virtual void Update( const BITMAPINFO* pBitmapInfo, const BYTE* pSrcData );
	virtual bool GetTexture( IDirect3DTexture9** ppTexture );

protected:
	D3DFORMAT m_TexFormat;
	uint m_nBmpWidth;
	uint m_nBmpHeight;
	uint m_nTexWidth;
	uint m_nTexHeight;
	uint m_nBmpPitch;
	uint m_nScale;

	uint m_nBmpBPP;
	uint m_nTexBPP;

	IDirect3DTexture9* m_pTexture;

};


#endif //__TEXTURE_HPP__
