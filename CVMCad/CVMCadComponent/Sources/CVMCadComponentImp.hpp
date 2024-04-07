/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CVMCadComponentImp.hpp
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
#ifndef __CVMCADCOMPONENTIMP_HPP__
#define __CVMCADCOMPONENTIMP_HPP__

struct IRoomRender;

using namespace std;

class CVMCadComponentImp : public IRoomRender
{
public:
public:
	struct SceneSegment
	{
		CCamera* pCamera;
		vector< pair<CElement*, boost::shared_ptr<CMesh> > > geometry;
	};

public:
	CVMCadComponentImp( HWND hWnd );
	virtual ~CVMCadComponentImp();

public:

	HRESULT Load( const void* pMemory, size_t s );
	HRESULT LoadScene( const void* pMemory, size_t size, boost::shared_ptr<CScene>& pSceneComponent );
	void	PopulateTree( std::vector<MSXML2::IXMLDOMNodePtr>& tree, MSXML2::IXMLDOMNodePtr pRoot  );
	HRESULT Render( HWND hWnd, COLORREF dwBgColor );
	HRESULT SetImage( int nCamX, int nCamY, const BITMAPINFO* pBmpHdr, const BYTE* pSrcData );
	HRESULT Resize( HWND hWnd, DWORD dwWidth, DWORD dwHeight );
	HRESULT SetText( const LOGFONTW* pLogFont,  LPCWSTR szText, COLORREF dwColor );
	
	std::pair<int, int> GetCamera( const POINT& pt );

	HRESULT LockFrame( const BITMAPINFO*& ppHeader, const BYTE*& ppImage, int& nImgSize );
	HRESULT UnlockFrame(); 
	void	Release();
	void	Update();
	HRESULT	TestComponent();

private:
	bool m_bRenderToScreenShot;
	HWND m_hWnd;
	HWND m_hActiveChildWnd;
	COLORREF m_dwTextColor;
	COLORREF m_hActiveChildWndClrBg;

	ID3DXBuffer* m_pD3DXFrontBuffer;
	LOGFONTW m_logFont;

	std::wstring m_TextString;

	boost::shared_ptr<CScene> m_pScene;
	CComPtr<ID3DXFont> m_pD3DXFont;
	VertexPtrArray m_GVPtrVertices;

	//Scene presentation
	std::vector<SceneSegment> m_ScenePresentation;
	std::vector<CElement*> m_TempElementsArray;
	std::vector<CCamera*> m_TempCamerasArray;

	boost::shared_ptr<BYTE> m_bScreenBuffer;

	float m_fPanX;
	float m_fPanY;

	float m_fAspectRatio;

	float m_fZoomX;
	float m_fZoomY;

	float fWindowWidth;
	float fWindowHeight;

	float fSceneWidth;
	float fSceneHeight;

	std::wstring m_Text;

};

#endif //__CVMCADCOMPONENTIMP_HPP__
