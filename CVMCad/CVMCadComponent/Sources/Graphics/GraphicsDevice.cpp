/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: GraphicsDevice.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-06
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

CGraphicsDevice* CGraphicsDevice::m_pSelf = 0;

MeshVertex::MeshVertex( float x, float y, float tu, float tv, uint color ) 
{
	this->x = x;
	this->y = y;
	this->z = 0.0f;
	this->rhw = 1.0f;
	this->tu = tu;
	this->tv = tv;
	this->color = color;
}

CGraphicsDevice* CGraphicsDevice::GetInstance()
{
	if ( m_pSelf == NULL )
	{
		m_pSelf = new CGraphicsDevice();
	}

	return m_pSelf;
}

void CGraphicsDevice::DestroyInstance()
{
	if( m_pSelf != NULL )
	{
		delete m_pSelf;
		m_pSelf = NULL;
	}
}

CGraphicsDevice::CGraphicsDevice()
{
	m_pDevice = NULL;
	m_hDeviceWnd = NULL;
	m_ActiveSwapChain.hChildWnd = NULL;
	m_ActiveSwapChain.pSwapChain = NULL;

}

CGraphicsDevice::~CGraphicsDevice()
{
	for( list<SwapChainInfo>::iterator i = m_SwapChainList.begin(); i != m_SwapChainList.end(); ++i )
	{
		if( (*i).pSwapChain != NULL )
		{
			(*i).pSwapChain->Release();
			(*i).pSwapChain = NULL;
		}
	}

	if( m_pDevice != NULL )
	{
        m_pDevice->Release();
		m_pDevice = NULL;
	}

	if( m_pD3D != NULL )
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
}


IDirect3DDevice9* CGraphicsDevice::GetDirect3DDevice()
{
	return m_pDevice;
}

void CGraphicsDevice::Create( HWND hParent )
{
	if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"Create()", L"Can't create Direct3D", L"" );

	D3DDISPLAYMODE mode;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT , &mode);


	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );	
	d3dpp.Windowed			= TRUE;
	d3dpp.SwapEffect		= D3DSWAPEFFECT_COPY;
	d3dpp.BackBufferFormat	= mode.Format;
	d3dpp.BackBufferWidth	= mode.Width;
	d3dpp.BackBufferHeight	= mode.Height;
	HRESULT hRes;

	// m_hWnd is top level window handle
	if( FAILED( hRes = 	m_pD3D->CreateDevice( 
		D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL,
		hParent,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED,
		&d3dpp, &m_pDevice) ) ) 
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"Create", L"Can't create d3d device", L"");

	m_pDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
	m_pDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
	m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pDevice->SetRenderState( D3DRS_LOCALVIEWER, FALSE );
	m_pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	m_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pDevice->SetFVF( D3D_VERTEX_FVF );
}


void CGraphicsDevice::EnableColorVertex( bool bEnable )
{
	if( bEnable )
	{
		m_pDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE  );
	}
	else
	{
		m_pDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE  );
	}
}

void CGraphicsDevice::EnableTexturedVertex( bool bEnable )
{
	if( !bEnable )
	{
		m_pDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE );
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE  );
	}
	else
	{
		m_pDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE  );
	}

}

void CGraphicsDevice::SetTexture( IDirect3DTexture9* pTexture )
{
	assert( pTexture );
	m_pDevice->SetTexture( 0, pTexture );
}

void CGraphicsDevice::EnableWireframe( bool bEnable )
{
	if( bEnable )
		m_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	else
		m_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}

void CGraphicsDevice::CreateSwapChain( HWND hChildWnd )
{

	D3DDISPLAYMODE mode;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT , &mode);

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed			= TRUE;
	d3dpp.SwapEffect		= D3DSWAPEFFECT_COPY;
	d3dpp.BackBufferFormat	= D3DFMT_UNKNOWN;
	d3dpp.hDeviceWindow	= hChildWnd;

	SwapChainInfo si;
	si.hChildWnd = hChildWnd;

	if( FAILED( m_pDevice->CreateAdditionalSwapChain( &d3dpp, &si.pSwapChain) ) )
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"CreateSwapChain", L"Can't create additional swap chain", L"");

	m_SwapChainList.push_back(si);

}

void CGraphicsDevice::DestroySwapChain( HWND hChildWnd )
{
	for(  list<SwapChainInfo>::iterator i = m_SwapChainList.begin(); i!= m_SwapChainList.end(); ++i )
	{
		if ( hChildWnd == (*i).hChildWnd )
		{
			(*i).pSwapChain->Release();
			(*i).pSwapChain = NULL;
			(*i).hChildWnd = NULL;
			m_SwapChainList.erase( i );
			return;
		}
	}
}

void CGraphicsDevice::ResizeSwapChain( int nWidth, int nHeight )
{
	if( nWidth == 0 || nHeight == 0 )
		return;

	assert( m_ActiveSwapChain.pSwapChain != NULL );

	list<SwapChainInfo>::iterator iSwapChain = m_SwapChainList.end();

	list<SwapChainInfo>::iterator i = m_SwapChainList.begin();
	for(; i != m_SwapChainList.end(); ++i )
	{
		if( (*i).hChildWnd == m_ActiveSwapChain.hChildWnd )
		{
            iSwapChain = i;
			if( (*iSwapChain).pSwapChain != NULL  )
			{
				(*iSwapChain).pSwapChain->Release();
				(*iSwapChain).pSwapChain = NULL;
			}
			break;
		}
	}

	assert( i != m_SwapChainList.end() );

	//Recreate swap chain with new backbuffer size
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed			= TRUE;
	d3dpp.SwapEffect		= D3DSWAPEFFECT_COPY;
	d3dpp.hDeviceWindow		= (*iSwapChain).hChildWnd;
	d3dpp.BackBufferFormat	= D3DFMT_UNKNOWN;

	if( FAILED( m_pDevice->CreateAdditionalSwapChain( &d3dpp, &(*iSwapChain).pSwapChain) ) )
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"ResizeSwapChain", L"Can't create additional swap chain", L"");
	
	IDirect3DSurface9* pBackBuffer = NULL;
	if( FAILED( (*i).pSwapChain->GetBackBuffer( 0,D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ) ) ){
		throw CBaseException<CGraphicsDevice>(L"CGraphicsDevice", L"ResizeSwapChain", L"Can't access swap chain back buffer", L"");
	}

	if( FAILED( m_pDevice->SetRenderTarget( 0, pBackBuffer ) ) ){
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"ResizeSwapChain", L"Can't set swap chain as render target" , L"");
	}

	m_ActiveSwapChain.pSwapChain = (*iSwapChain).pSwapChain;
	m_ActiveSwapChain.hChildWnd = (*iSwapChain).hChildWnd;

	if( pBackBuffer != NULL )
		pBackBuffer->Release();
}


void CGraphicsDevice::SetActiveSwapChain( HWND hParent )
{

	//Find swap chain associated with pDeviceWnd
	list<SwapChainInfo>::iterator pos;
	list<SwapChainInfo>::iterator begin_pos = m_SwapChainList.begin();
	list<SwapChainInfo>::iterator end_pos = m_SwapChainList.end();


	for( pos = begin_pos; pos != end_pos; ++pos )
	{
		if ( hParent == (*pos).hChildWnd )
		{
			m_ActiveSwapChain.pSwapChain = (*pos).pSwapChain; 
			m_ActiveSwapChain.hChildWnd = (*pos).hChildWnd;

			IDirect3DSurface9* pBackBuffer = NULL;
			if( FAILED( (*pos).pSwapChain->GetBackBuffer( 0,D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ) ) ){
				throw CBaseException<CGraphicsDevice>(L"CGraphicsDevice", L"ResizeSwapChain", L"Can't access swap chain back buffer", L"");
			}

			if( FAILED( m_pDevice->SetRenderTarget( 0, pBackBuffer ) ) ){
				throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"ResizeSwapChain", L"Can't set swap chain as render target" , L"");
			}

			if( pBackBuffer != NULL )
				pBackBuffer->Release();
			return;
		}
	}

	if( pos == end_pos )
	{
		CreateSwapChain( hParent );
		SetActiveSwapChain( hParent );
	}
}


void CGraphicsDevice::BeginScene()
{
	if( FAILED ( m_pDevice->BeginScene() ) )
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"BeginScene()", L"Can't begin scene", L"");
}


void CGraphicsDevice::EndScene()
{
	if( FAILED ( m_pDevice->EndScene() ) )
		throw CBaseException<CGraphicsDevice>(  L"CGraphicsDevice", L"EndScene()", L"Can't end scene", L"");
}


void CGraphicsDevice::Present()
{
	if( FAILED( m_ActiveSwapChain.pSwapChain->Present( NULL, NULL, NULL, NULL, NULL ) ) )
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"Present()", L"Can't present scene", L"");

	::ValidateRect( m_ActiveSwapChain.hChildWnd, NULL );
}


void CGraphicsDevice::Clear( DWORD dwColor )
{
    if( FAILED(m_pDevice->Clear(0,NULL, D3DCLEAR_TARGET,dwColor,0.0,0) ) )
		throw CBaseException<CGraphicsDevice>(  L"CGraphicsDevice", L"Clear( DWORD dwColor", L"Can't clear primary surface", L"");
}
