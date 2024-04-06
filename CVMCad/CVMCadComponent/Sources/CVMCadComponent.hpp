/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CVMCadComponent.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-27
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __CVMCADCOMPONENT_HPP__
#define __CVMCADCOMPONENT_HPP__

struct IRoomRender
{
	virtual HRESULT Load( const void* pMemory, size_t s ) = 0;
	virtual HRESULT Render( HWND hWnd, COLORREF dwBgColor ) = 0;
	virtual HRESULT SetImage( int nCamX, int nCamY, const BITMAPINFO* pBmpHdr, const BYTE* pSrcData ) = 0;
	virtual HRESULT Resize( HWND hWnd, DWORD dwWidth, DWORD dwHeight ) = 0;
	virtual void	Release() = 0;
	virtual HRESULT SetText( const LOGFONTW* pLogFont, LPCWSTR szText, COLORREF dwColor ) = 0;
	virtual std::pair<int, int> GetCamera( const POINT& pt ) = 0;
	virtual HRESULT LockFrame( const BITMAPINFO* &ppHeader, const BYTE* &ppImage, int& nImgSize ) = 0;
	virtual HRESULT UnlockFrame() = 0; 
	virtual HRESULT TestComponent() = 0;
};

extern "C" __declspec(dllexport) HRESULT CreateRender( HWND hWnd, IRoomRender** pRoomRender );


#endif //__CVMCADCOMPONENT_HPP__
