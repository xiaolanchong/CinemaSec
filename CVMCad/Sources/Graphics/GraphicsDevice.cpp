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
CSingletonDestroyer<CGraphicsDevice> CGraphicsDevice::m_destroyer;

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
	if ( m_pSelf == 0 )
	{
		m_pSelf = new CGraphicsDevice();
		m_destroyer.SetDestroyable( m_pSelf );
	}

	return m_pSelf;
}

CGraphicsDevice::CGraphicsDevice()
{
	m_pDevice = NULL;
	m_pDeviceWnd = NULL;
	m_ActiveSwapChain.pDeviceWnd = NULL;
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

	if( m_ActiveSwapChain.pSwapChain != NULL )
	{
		m_ActiveSwapChain.pSwapChain->Release();
		m_ActiveSwapChain.pSwapChain = NULL;
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

void CGraphicsDevice::Create( CWnd* pDeviceWnd )
{
	if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"Create()", L"Can't create Direct3D", L"" );

	CRect rClientRect;
	pDeviceWnd->GetClientRect( &rClientRect );

	int nWidth = (rClientRect.right - rClientRect.left);
	int nHeight = ( rClientRect.bottom - rClientRect.top );

	if( nWidth <= 0 || nHeight <= 0 )
	{
		nWidth = 1;
		nHeight = 1;
	}


	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );	
	d3dpp.Windowed			= TRUE;
	d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat	= D3DFMT_UNKNOWN;
	d3dpp.BackBufferWidth	= nWidth;
	d3dpp.BackBufferHeight	= nHeight;
	d3dpp.BackBufferCount	= 1;

	
	HRESULT hRes;

	// m_hWnd is top level window handle
	if( FAILED( hRes = 	m_pD3D->CreateDevice( 
		D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL,
		pDeviceWnd->GetSafeHwnd(),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &m_pDevice) ) ) 
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"Create", L"Can't create d3d device", L"");

	m_pDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
	m_pDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
	m_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pDevice->SetRenderState( D3DRS_LOCALVIEWER, FALSE );
	m_pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	m_pDevice->SetFVF( D3D_VERTEX_FVF );
}

void CGraphicsDevice::Resize( int nWidth, int nHeight )
{

	if( nWidth <= 0 || nHeight <= 0 )
		return;

	IDirect3DSwapChain9* pActiveSwapChain = NULL;

	m_pDevice->GetSwapChain( 0, &pActiveSwapChain );

	if( pActiveSwapChain != NULL )
	{
		pActiveSwapChain->Release();
		pActiveSwapChain = NULL;
	}

	//Recreate swap chain with new backbuffer size
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed			= TRUE;
	d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow		= m_pDeviceWnd->m_hWnd;
	d3dpp.BackBufferFormat	= D3DFMT_UNKNOWN;
	d3dpp.BackBufferCount	= 1;
	d3dpp.BackBufferWidth	= nWidth;
	d3dpp.BackBufferHeight  = nHeight;
	d3dpp.Flags				= 0;


	if( FAILED( m_pDevice->CreateAdditionalSwapChain( &d3dpp, &m_ActiveSwapChain.pSwapChain) ) )
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"ResizeSwapChain", L"Can't create additional swap chain", L"");

	IDirect3DSurface9* pBackBuffer = NULL;
	if( FAILED( m_ActiveSwapChain.pSwapChain->GetBackBuffer( 0,D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ) ) ){
		throw CBaseException<CGraphicsDevice>(L"CGraphicsDevice", L"ResizeSwapChain", L"Can't access swap chain back buffer", L"");
	}

	if( FAILED( m_pDevice->SetRenderTarget( 0, pBackBuffer ) ) ){
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"ResizeSwapChain", L"Can't set swap chain as render target", L"" );
	}

	if( pBackBuffer != NULL )
		pBackBuffer->Release();

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

void CGraphicsDevice::EnableWireframe( bool bEnable )
{
	if( bEnable )
		m_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	else
		m_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}

void CGraphicsDevice::CreateSwapChain( CWnd* pDeviceWnd )
{
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed			= TRUE;
	d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat	= D3DFMT_UNKNOWN;
	d3dpp.BackBufferWidth	= 1;
	d3dpp.BackBufferHeight	= 1;
	d3dpp.BackBufferCount	= 1;
	d3dpp.hDeviceWindow	= pDeviceWnd->GetSafeHwnd();
	d3dpp.Flags				= D3DPRESENTFLAG_DEVICECLIP;

	SwapChainInfo si;
	si.pDeviceWnd = pDeviceWnd;

	if( 0 == m_SwapChainList.size() )
	{
		if( FAILED( m_pDevice->GetSwapChain( 0, &si.pSwapChain ) ) )
			throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"CreateSwapChain", L"Can't create additional swap chain", L"");
	}
	else
	{
		if( FAILED( m_pDevice->CreateAdditionalSwapChain( &d3dpp, &si.pSwapChain) ) )
			throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"CreateSwapChain", L"Can't create additional swap chain", L"");
	}

	m_SwapChainList.push_back(si);

}


void CGraphicsDevice::DestroySwapChain( CWnd* pDeviceWnd )
{
	for(  list<SwapChainInfo>::iterator i = m_SwapChainList.begin(); i!= m_SwapChainList.end(); ++i )
	{
		if ( pDeviceWnd == (*i).pDeviceWnd )
		{
			(*i).pSwapChain->Release();
			(*i).pSwapChain = NULL;
			(*i).pDeviceWnd = NULL;
			m_SwapChainList.erase( i );
			return;
		}
	}
}

void CGraphicsDevice::ResizeSwapChain( int nWidth, int nHeight )
{
	if( nWidth == 0 || nHeight == 0 )
		return;

	if( m_ActiveSwapChain.pSwapChain != NULL )
	{
        int n = m_ActiveSwapChain.pSwapChain->Release();
		m_ActiveSwapChain.pSwapChain = NULL;
	}

	//Recreate swap chain with new backbuffer size
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed			= TRUE;
	d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow		= m_ActiveSwapChain.pDeviceWnd->GetSafeHwnd();
	d3dpp.BackBufferFormat	= D3DFMT_UNKNOWN;
	d3dpp.BackBufferCount	= 1;
	d3dpp.BackBufferWidth	= nWidth;
	d3dpp.BackBufferHeight  = nHeight;
	d3dpp.Flags				= D3DPRESENTFLAG_DEVICECLIP;

	if( FAILED( m_pDevice->CreateAdditionalSwapChain( &d3dpp, &m_ActiveSwapChain.pSwapChain) ) )
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"ResizeSwapChain", L"Can't create additional swap chain", L"");
	
	IDirect3DSurface9* pBackBuffer = NULL;
	if( FAILED( m_ActiveSwapChain.pSwapChain->GetBackBuffer( 0,D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ) ) ){
		throw CBaseException<CGraphicsDevice>(L"CGraphicsDevice", L"ResizeSwapChain", L"Can't access swap chain back buffer", L"");
	}

	if( FAILED( m_pDevice->SetRenderTarget( 0, pBackBuffer ) ) ){
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"ResizeSwapChain", L"Can't set swap chain as render target" , L"");
	}

	if( pBackBuffer != NULL )
		pBackBuffer->Release();
}


void CGraphicsDevice::SetActiveSwapChain( CWnd* pDeviceWnd )
{

	if( m_ActiveSwapChain.pDeviceWnd == pDeviceWnd )
		return;

	//Find swap chain associated with pDeviceWnd
	list<SwapChainInfo>::iterator pos;
	list<SwapChainInfo>::iterator begin_pos = m_SwapChainList.begin();
	list<SwapChainInfo>::iterator end_pos = m_SwapChainList.end();


	for( pos = begin_pos; pos != end_pos; ++pos )
	{
		if ( pDeviceWnd == (*pos).pDeviceWnd )
		{
			if( m_ActiveSwapChain.pSwapChain != NULL )
			{
				m_ActiveSwapChain.pSwapChain->Release();
				m_ActiveSwapChain.pSwapChain = NULL;
			}

			(*pos).pSwapChain->AddRef();
			m_ActiveSwapChain.pSwapChain = (*pos).pSwapChain; 
			m_ActiveSwapChain.pDeviceWnd = (*pos).pDeviceWnd;

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
		TRACE( "CMesh::SetActiveSwapChain() - Can't find associated swap chain" , L"");
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
	if( FAILED( m_ActiveSwapChain.pSwapChain->Present( 
		NULL, NULL, m_ActiveSwapChain.pDeviceWnd->GetSafeHwnd(), NULL, NULL ) ) )
		throw CBaseException<CGraphicsDevice>( L"CGraphicsDevice", L"Present()", L"Can't present scene", L"");

	m_ActiveSwapChain.pDeviceWnd->ValidateRect( NULL );
}

void CGraphicsDevice::Clear( DWORD dwColor )
{
    if( FAILED(m_pDevice->Clear(0,NULL, D3DCLEAR_TARGET,dwColor,1.0,0) ) )
		throw CBaseException<CGraphicsDevice>(  L"CGraphicsDevice", L"Clear( DWORD dwColor", L"Can't clear primary surface", L"");
}

