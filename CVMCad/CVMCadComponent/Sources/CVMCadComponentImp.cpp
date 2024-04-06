/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CVMCadComponentImp.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-27
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Exceptions.hpp"
#include "Model.hpp"
#include "Graphics.hpp"
#include "../resource.h"


#include "CVMCadComponent.hpp"
#include "CVMCadComponentImp.hpp"


typedef MSXML2::IXMLDOMNodePtr XMLNodePtr;
typedef MSXML2::IXMLDOMNodeListPtr XMLNodeListPtr;
typedef MSXML2::IXMLDOMNamedNodeMapPtr XMLNamedNodeMapPtr;
typedef MSXML2::IXMLDOMElementPtr XMLElementPtr;
typedef MSXML2::IXMLDOMAttributePtr XMLAttributePtr;
typedef MSXML2::IXMLDOMProcessingInstructionPtr XMLProcessingInstructionPtr;
typedef MSXML2::IXMLDOMDocumentPtr XMLDomDocumentPtr;


__declspec(dllexport) 
HRESULT CreateRender( HWND hWnd, IRoomRender** pRoomRender )
{

	try
	{
		DWORD dwValue;
		CRegKey key;

		if(  ERROR_SUCCESS == key.Open( HKEY_LOCAL_MACHINE, L"SOFTWARE\\ELVEES\\Common", KEY_READ  ) 
			&& ERROR_SUCCESS == key.QueryDWORDValue( L"LangLCID", dwValue ) )
		{
			SetThreadLocale( dwValue );
		}

		CGraphicsDevice* pGR = CGraphicsDevice::GetInstance();
		pGR->Create( hWnd );

		CVMCadComponentImp* pCVMCadComponentImp = new CVMCadComponentImp( hWnd );
		*pRoomRender = pCVMCadComponentImp;
		return S_OK;
	}
	catch( ... )
	{
		return E_FAIL;
	}
}

CVMCadComponentImp::CVMCadComponentImp( HWND hWnd ):
	m_pD3DXFont( NULL )
{
	m_hWnd = hWnd;

	m_fPanX = 0;
	m_fPanY = 0;

	m_fZoomX = 1;
	m_fZoomY = 1;

	m_fAspectRatio = 1;

	m_dwTextColor = 0xffffffff;

	m_pD3DXFrontBuffer = NULL;

	m_bRenderToScreenShot = false;

	::ZeroMemory( &m_logFont, sizeof( LOGFONTW ) );
}

CVMCadComponentImp::~CVMCadComponentImp()
{
	CGraphicsDevice::GetInstance()->DestroyInstance();
}


void CVMCadComponentImp::PopulateTree( std::vector<MSXML2::IXMLDOMNodePtr>& tree, MSXML2::IXMLDOMNodePtr pRoot  )
{
	MSXML2::IXMLDOMNodeListPtr pXMLNodeList;
	MSXML2::IXMLDOMNodePtr pXMLNode;

	pXMLNodeList = pRoot->childNodes;
	for( long i = 0; i < pXMLNodeList->length; ++i )
	{
		pXMLNode = pXMLNodeList->item[i];
		tree.push_back( pXMLNode );
		CVMCadComponentImp::PopulateTree( tree, pXMLNode );
	}
}

HRESULT CVMCadComponentImp::Load( const void* pMemory, size_t s )
{
	LoadScene( pMemory, s, m_pScene );

	if( m_pScene.get() == NULL )
		return E_FAIL;

	else
	{
		Vertex topLeft, bottomRight;
		m_GVPtrVertices.clear();
		CNodeToolSet::CollectNodes( m_pScene.get(), m_GVPtrVertices );
		CVertexToolSet::GetBounds( &m_GVPtrVertices, topLeft, bottomRight );

		m_fAspectRatio = ( bottomRight.x - topLeft.x ) / ( bottomRight.y - topLeft.y );

		Update();
		return S_OK;
	}
	
}

HRESULT CVMCadComponentImp::LoadScene( const void* pMemory, size_t size, boost::shared_ptr<CScene>& pSceneComponent )
{
	boost::shared_ptr<CScene> pActiveScene;

	//Out buffer
	CString str( (char*)pMemory, (int)size );

	try
	{
		wstring sNodeName;
		wstring sAttrVal;
		_variant_t varLoadResult((bool)FALSE);
		_variant_t varAttrVal;

		MSXML2::IXMLDOMNodeListPtr pXMLNodeList = NULL;
		MSXML2::IXMLDOMNodePtr pXMLParentNode = NULL;
		MSXML2::IXMLDOMNodePtr pXMLChildNode = NULL;
		MSXML2::IXMLDOMNodePtr pXMLNode = NULL;
		MSXML2::IXMLDOMElementPtr pXMLRootElement = NULL;
		MSXML2::IXMLDOMNamedNodeMapPtr pXMLNamedNodeMap = NULL;
		MSXML2::IXMLDOMDocumentPtr pXMLDoc = NULL;

		CCamera* pLastCamera;
		CElement* pLastElement;


		//Create MSXML DOM object
		EVAL_HR(pXMLDoc.CreateInstance("Msxml2.DOMDocument"));

		varLoadResult = pXMLDoc->loadXML( str.GetBuffer() );
		if( (bool)varLoadResult == FALSE )
			throw CBaseException<CSceneToolSet>( L"CSceneToolSet", L"LoadScene()", L"Can't load XML document", L"" );

		list<MSXML2::IXMLDOMNodePtr> nodes;

		if( FAILED(pXMLDoc->get_documentElement( &pXMLRootElement )) )
			throw CBaseException<CSceneToolSet>( L"CSceneToolSet", L"LoadScene()", L"XML document is not valid", L"" );

		std::vector<MSXML2::IXMLDOMNodePtr> tree;
		PopulateTree( tree, pXMLDoc );

		for( std::vector<MSXML2::IXMLDOMNodePtr>::iterator it = tree.begin(); it != tree.end(); ++it )
		{
			pXMLParentNode = (MSXML2::IXMLDOMNodePtr)(*it);
			pXMLNodeList = pXMLParentNode->childNodes;

			sNodeName = _bstr_t( pXMLParentNode->nodeName );

			if( sNodeName.compare( L"Scene" ) == 0 )
			{
				boost::shared_ptr<CScene> pScene( new CScene() );
				pActiveScene = pScene;
				pXMLNamedNodeMap = pXMLParentNode->attributes;
			}
			else if( sNodeName.compare( L"Camera") == 0 )
			{
				pLastCamera = pActiveScene->AddCamera();
				pXMLNamedNodeMap = pXMLParentNode->attributes;

				int idx = 0, idy = 0;

				if( pXMLNode = pXMLNamedNodeMap->getNamedItem(L"idx") )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					int idx = Convert::ToType<int>( sAttrVal );
					pLastCamera->SetIdX( idx );
				}

				if( pXMLNode = pXMLNamedNodeMap->getNamedItem(L"idy") )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					int idy = Convert::ToType<int>( sAttrVal );
					pLastCamera->SetIdY( idy );
				}

			}
			else if( sNodeName.compare( L"Element" )  == 0 )
			{
				pLastElement = pLastCamera->AddElement();
			}

			else if( sNodeName.compare( L"Vertex" ) == 0 )
			{
				pXMLNamedNodeMap = pXMLParentNode->attributes;
				Vertex v;

				//x
				if( pXMLNode = pXMLNamedNodeMap->getNamedItem( L"x" ) )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					v.x = Convert::ToType<float>( sAttrVal );
				}

				//y
				if( pXMLNode = pXMLNamedNodeMap->getNamedItem( L"y" ) )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					v.y = Convert::ToType<float>( sAttrVal );
				}

				//tu
				if( pXMLNode = pXMLNamedNodeMap->getNamedItem( L"tu" ) )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					v.tu = Convert::ToType<float>( sAttrVal );
				}

				//tv
				if( pXMLNode = pXMLNamedNodeMap->getNamedItem( L"tv" ) )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					v.tv = Convert::ToType<float>( sAttrVal );
				}

				//color
				if ( pXMLNode = pXMLNamedNodeMap->getNamedItem( L"color" ) )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					v.color = Convert::ToType<uint>( sAttrVal );
				}

				VertexArray* pVertices;
				pLastElement->GetVertices( &pVertices );
				pVertices->push_back( v );

			}
			else if( sNodeName.compare( L"Index" ) == 0 )
			{
				pXMLNamedNodeMap = pXMLParentNode->attributes;
				//index
				if( pXMLNode = pXMLNamedNodeMap->getNamedItem( L"value" ) )
				{
					sAttrVal = (pXMLNode->nodeValue).bstrVal;
					Index i = Convert::ToType<uint>( sAttrVal );

					IndexArray* pIndices;
					pLastElement->GetIndices( &pIndices );
					pIndices->push_back( i );
				}
			}
		}

		pSceneComponent = pActiveScene;
		return S_OK;

	}
	catch( ... )
	{
		return E_FAIL;
	}
}

HRESULT CVMCadComponentImp::LockFrame( const BITMAPINFO* &ppHeader, const BYTE* &ppImage, int& nImgSize )
{
	CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();

	m_bRenderToScreenShot = true;

    try
	{
		IDirect3DDevice9* pDevice = pGD->GetDirect3DDevice();

		if( m_pD3DXFrontBuffer != NULL )
			throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"LockFrame", L"Frame is already locked" , L"");

		CComPtr<IDirect3DSurface9> pFrontBuffer;

		D3DDISPLAYMODE dm;
		pDevice->GetDisplayMode( 0, &dm );

		CRect rc;
		::GetClientRect( m_hActiveChildWnd, &rc );

		if( FAILED( pDevice->CreateRenderTarget( rc.Width(), rc.Height(), dm.Format, D3DMULTISAMPLE_NONE, 0, TRUE,  &(pFrontBuffer.p), NULL ) ) )
			throw CBaseException<CGraphicsDevice>( L"CVMCadComponentImp", L"LockFrame", L"Can't create offscreen surface" , L"");

		if( FAILED( pDevice->SetRenderTarget( 0, pFrontBuffer.p ) ) )
			throw CBaseException<CGraphicsDevice>( L"CVMCadComponentImp", L"LockFrame", L"Can't create offscreen surface" , L"");

        if( FAILED( Render( m_hActiveChildWnd, m_hActiveChildWndClrBg ) ) )
			throw CBaseException<CGraphicsDevice>( L"CVMCadComponentImp", L"LockFrame", L"Can't create render to surface" , L"");


		//if( FAILED( D3DXSaveSurfaceToFile( L"D:\\Test.bmp", D3DXIFF_BMP, pFrontBuffer.p, NULL, NULL) ) )
		//	throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"LockFrame", L"Can't create bmp with surface" , L"");

	
		if( FAILED( D3DXSaveSurfaceToFileInMemory( &m_pD3DXFrontBuffer, D3DXIFF_BMP, pFrontBuffer.p, NULL, NULL) ) )
			throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"LockFrame", L"Can't create bmp with surface" , L"");

		ppHeader = (BITMAPINFO*)( sizeof( BITMAPFILEHEADER ) + (BYTE*)m_pD3DXFrontBuffer->GetBufferPointer() );
		ppImage = (BYTE*)( sizeof( BITMAPINFO ) + sizeof( BITMAPFILEHEADER ) + (BYTE*)m_pD3DXFrontBuffer->GetBufferPointer() );
		nImgSize = WIDTHBYTES( ppHeader->bmiHeader.biWidth * ppHeader->bmiHeader.biBitCount ) * ppHeader->bmiHeader.biHeight ;

		pGD->SetActiveSwapChain( m_hActiveChildWnd );

	}
	catch( CBaseException<CGraphicsDevice> e )
	{
		m_bRenderToScreenShot = false;
		return E_FAIL;
	}

	m_bRenderToScreenShot = false;
	return S_OK;

}

HRESULT CVMCadComponentImp::UnlockFrame()
{
	CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();

	try
	{
		if( m_pD3DXFrontBuffer == NULL )
			throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"UnlockFrame", L"Frame didn't locked" , L"");

		m_pD3DXFrontBuffer->Release();
		m_pD3DXFrontBuffer = NULL;

	}
	catch( CBaseException<CGraphicsDevice> e )
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CVMCadComponentImp::TestComponent()
{
	CGraphicsDevice* pDevice = CGraphicsDevice::GetInstance();
	assert( pDevice );

	D3DCAPS9 caps;
	if( FAILED( pDevice->GetDirect3DDevice()->GetDeviceCaps( &caps ) ) )
		return E_FAIL;

	if( ( caps.Caps2 & ~D3DCAPS2_DYNAMICTEXTURES )
		&&( caps.TextureCaps & ~D3DPTEXTURECAPS_NONPOW2CONDITIONAL )
		&&( caps.TextureFilterCaps & ~( D3DPTFILTERCAPS_MAGFLINEAR | D3DPTFILTERCAPS_MINFLINEAR )  )
		)
		return S_OK;
    
	return E_FAIL;
}



HRESULT CVMCadComponentImp::Resize( HWND hWnd, DWORD dwWidth, DWORD dwHeight )
{
	CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
	
	//if( !IsWindowVisible( hWnd ) )
	//	return S_OK;

	try
	{
		m_hActiveChildWnd = hWnd;
		pGD->SetActiveSwapChain( hWnd );
		pGD->ResizeSwapChain( dwWidth, dwHeight );

		Vertex topLeft;
		Vertex bottomRight;

		m_GVPtrVertices.resize(0);
		CNodeToolSet::CollectNodes<Vertex>( m_pScene.get(), m_GVPtrVertices );

		fWindowWidth = (float)dwWidth;
		fWindowHeight = (float)dwHeight;

		CVertexToolSet::GetBounds( &m_GVPtrVertices, topLeft, bottomRight );

		fSceneWidth = ( bottomRight.x - topLeft.x );
		fSceneHeight = ( bottomRight.y - topLeft.y );

		double dAWindowW = fWindowWidth / fSceneWidth;
		double dAWindowH = fWindowHeight / fSceneHeight;

		double __min = min ( dAWindowW, dAWindowH );

		m_fZoomX = __min;
		m_fZoomY = __min;

		m_fPanX = -topLeft.x * m_fZoomX + (fWindowWidth - m_fZoomX * fSceneWidth )*0.5f;
		m_fPanY = -topLeft.y * m_fZoomY + (fWindowHeight - m_fZoomY * fSceneHeight)*0.5f;
        Update();


		return S_OK;
	}
	catch( ... )
	{
		return E_FAIL;
	}
}



std::pair<int, int> CVMCadComponentImp::GetCamera( const POINT& pt )
{
	assert( m_pScene != NULL );
	Vertex v;
	Vertex p0, p1, p2;
	Index i0,i1,i2;

	v.x = (float)pt.x;
	v.y = (float)pt.y;

	CNode* pNode = NULL;
	for( int i = 0; pNode = m_pScene->GetChild( i ); ++i )
	{
		if( CCamera* pCamera = dynamic_cast<CCamera*>(pNode) )
		{
			for( int j = 0; pNode = pCamera->GetChild(j); ++j )
			{
				if( CElement* pElement = dynamic_cast<CElement*>(pNode) )
				{
					VertexArray* pVertices;
					IndexArray* pIndices;

					pElement->GetVertices( &pVertices );
					pElement->GetIndices( &pIndices );

					assert( pIndices->size() % 3 == 0 );

					for( size_t k = 0; k < pIndices->size(); k+=3 )
					{
						i0 = (*pIndices)[k];
						i1 = (*pIndices)[k+1];
						i2 = (*pIndices)[k+2];

						p0.x = (*pVertices)[ i0 ].x * m_fZoomX + m_fPanX;
						p0.y = (*pVertices)[ i0 ].y * m_fZoomY + m_fPanY;

						p1.x = (*pVertices)[ i1 ].x * m_fZoomX + m_fPanX;
						p1.y = (*pVertices)[ i1 ].y * m_fZoomY + m_fPanY;

						p2.x = (*pVertices)[ i2 ].x * m_fZoomX + m_fPanX;
						p2.y = (*pVertices)[ i2 ].y * m_fZoomY + m_fPanY;

						if( Triangle::PointInTriangle( v, p0, p1, p2 ) )
						{
							return pCamera->GetId();
						}
					}
				}
			}
		}
	}

	return std::make_pair(-1,-1);
}


HRESULT CVMCadComponentImp::SetText( const LOGFONTW* pLogFont, LPCWSTR szText, COLORREF dwColor )
{
	if ( memcmp( pLogFont, &m_logFont, sizeof(LOGFONTW) ) != 0  )
	{
		CComPtr<ID3DXFont> pFont;
		m_dwTextColor = D3DCOLOR_XRGB ( GetRValue( dwColor ), GetGValue( dwColor ), GetBValue( dwColor )  ); 

		if(  FAILED( D3DXCreateFont( CGraphicsDevice::GetInstance()->GetDirect3DDevice(),
			pLogFont->lfHeight, pLogFont->lfWidth, pLogFont->lfWeight, 0, pLogFont->lfItalic, pLogFont->lfCharSet, 
			OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, pLogFont->lfPitchAndFamily, pLogFont->lfFaceName, &pFont ) ) )
		{
			return E_FAIL;
		}

		m_logFont = *pLogFont;
		m_pD3DXFont = pFont;
	}
	
	m_TextString.clear();
	m_TextString.append( szText );

	return S_OK;
}

HRESULT CVMCadComponentImp::Render( HWND hWnd, COLORREF dwBgColor )
{
	CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
	IDirect3DDevice9* pDevice = pGD->GetDirect3DDevice();
	assert(pDevice != NULL );

	//if( !IsWindowVisible( hWnd ) )
	//	return S_OK;
    
	m_hActiveChildWnd = hWnd;
	m_hActiveChildWndClrBg = dwBgColor;

	dwBgColor = D3DCOLOR_XRGB ( GetRValue( dwBgColor ), GetGValue( dwBgColor ), GetBValue( dwBgColor )  ); 

	try
	{
		CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();

		if( !m_bRenderToScreenShot )
		{
			pGD->SetActiveSwapChain( hWnd );
		}
	
        if( m_pScene == NULL )
		{
			pGD->Clear( dwBgColor );
			pGD->Present();
			return S_OK;
		}

		pGD->Clear( dwBgColor );
		pGD->BeginScene();

		for( size_t i = 0; i < m_ScenePresentation.size(); ++i )
		{
			IDirect3DTexture9* pDXTexture = NULL;
			CTexture* pTexture = m_ScenePresentation[i].pCamera->GetTexture();
			if( pTexture->GetTexture( &pDXTexture ) )
			{
				pGD->EnableTexturedVertex( true );
				pGD->EnableWireframe(false);
				pGD->SetTexture( pDXTexture );
			}
			else
			{
				pGD->EnableTexturedVertex( false );
				pGD->EnableWireframe(true );
			}

			for( size_t j = 0; j < m_ScenePresentation[i].geometry.size(); ++j )
			{
				m_ScenePresentation[i].geometry[j].second->Draw();
			}
		}

		if( m_pD3DXFont.p != NULL )
		{
			RECT rect;
			::GetClientRect( hWnd, &rect );
			m_pD3DXFont->DrawTextW( NULL, m_TextString.c_str(), -1, &rect, DT_LEFT|DT_NOCLIP, m_dwTextColor );
		}

		pGD->EndScene();
		pGD->Present();
	}
	catch( CBaseException<CGraphicsDevice> e )
	{
		e.DisplayErrorMessage();
	}
	catch( ... ) 
	{
		return E_FAIL;
	}

	return S_OK;
}

void CVMCadComponentImp::Update()
{
	if( m_pScene == NULL )
		return;

	//Clear Geometry View vertices
	m_GVPtrVertices.resize(0);

	//Collect all vertices in scene
	CNodeToolSet::CollectNodes<Vertex>( m_pScene.get(), m_GVPtrVertices );


	//Clear previously founded cameras and elements
	m_TempCamerasArray.resize(0);

	//Collect cameras
	CNodeToolSet::CollectNodes<CCamera>( m_pScene.get(), m_TempCamerasArray);

	//Populate scene presentation
	while( m_ScenePresentation.size() < m_TempCamerasArray.size() )
	{
		SceneSegment sg;
		sg.pCamera = NULL;
		m_ScenePresentation.push_back( sg );
	}

	while( m_ScenePresentation.size() > m_TempCamerasArray.size() )
		m_ScenePresentation.pop_back();

	for( size_t i = 0; i < m_ScenePresentation.size(); ++i )
	{

		//Attach camera
		m_ScenePresentation[i].pCamera = m_TempCamerasArray[i];

		//and camera childs
		m_TempElementsArray.resize(0);
		CNodeToolSet::CollectNodes<CElement>( m_TempCamerasArray[i], m_TempElementsArray );

		//Resize to fit current elements num
		while( m_ScenePresentation[i].geometry.size() < m_TempElementsArray.size() )
		{
			shared_ptr<CMesh> ptr( new CMesh() );
			m_ScenePresentation[i].geometry.push_back( make_pair( (CElement*)NULL, ptr ) );
		}
		while( m_ScenePresentation[i].geometry.size() > m_TempElementsArray.size() )
			m_ScenePresentation[i].geometry.pop_back();

		for( size_t j = 0; j < m_ScenePresentation[i].geometry.size(); ++j )
		{
			VertexArray* pVertices;
			IndexArray* pIndices;
			VertexArray tVertices;

			//Get element vertices & indices
			m_TempElementsArray[j]->GetVertices( &pVertices );
			m_TempElementsArray[j]->GetIndices( &pIndices );

			//Apply current view transformation
			tVertices.resize( pVertices->size() );
			for(  size_t k = 0; k < pVertices->size(); ++k )
			{
				tVertices[k].x = (*pVertices)[k].x;
				tVertices[k].y = (*pVertices)[k].y;

				tVertices[k].x *= m_fZoomX;
				tVertices[k].y *= m_fZoomY;

				tVertices[k].x += m_fPanX;
				tVertices[k].y += m_fPanY;

				tVertices[k].tu = (*pVertices)[k].tu;
				tVertices[k].tv = (*pVertices)[k].tv;

				tVertices[k].color = (*pVertices)[k].color;
				tVertices[k].selected = (*pVertices)[k].selected;
			}

			//Attach element and it representation
			m_ScenePresentation[i].geometry[j].first = m_TempElementsArray[j];
			m_ScenePresentation[i].geometry[j].second->Update( &tVertices, pIndices );
		}
	}
}

HRESULT CVMCadComponentImp::SetImage( int nCamX, int nCamY, const BITMAPINFO* pBmpHdr, const BYTE* pSrcData )
{
	try
	{
		CCamera* pCamera = NULL;
		for( size_t i = 0; i < m_ScenePresentation.size(); ++i )
		{

			if( ( m_ScenePresentation[i].pCamera->GetIdX() == nCamX ) && ( m_ScenePresentation[i].pCamera->GetIdY() == nCamY ) )
			{
				pCamera = m_ScenePresentation[i].pCamera;
				pCamera->SetSource( pBmpHdr, pSrcData );
				break;
			}
		}

		//Can't find camera with specified idx && idy
		if( pCamera )
		{
			Update();
			return S_OK;
		}
		else 
			return E_FAIL;
	}
	catch( ... )
	{
		return E_FAIL;
	}
}

void CVMCadComponentImp::Release()
{
	delete (CVMCadComponentImp*)this;
}
