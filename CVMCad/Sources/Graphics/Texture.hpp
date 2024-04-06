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

class CTexture
{
public:
	CTexture();
	virtual ~CTexture();

public:
	virtual void CreateStatic( const wstring& sFileName );
	virtual void Update( const BITMAPINFO* pBitmapInfo, const BYTE* pSrcData );
	virtual void Set();

	//Helper functions
	static PBITMAPINFO CreateBitmapInfoStruct( HBITMAP hBmp );


protected:
	virtual void Initialize();

protected:
	bool m_bStatic;
	bool m_bInititalized;
	uint m_nWidth;
	uint m_nHeight;
	IDirect3DTexture9* m_pTexture;

};


#endif //__TEXTURE_HPP__
