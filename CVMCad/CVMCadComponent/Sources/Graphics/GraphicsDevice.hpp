 /*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: GraphicsDevice.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-06
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __GRAPHICSDEVICE_HPP__
#define __GRAPHICSDEVICE_HPP__

using namespace std;
using namespace boost;

typedef unsigned short MeshIndex;

#define D3D_VERTEX_FVF (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)


struct MeshVertex
{
	MeshVertex( float x = 0.0f, float y = 0.0f, float tu = 0.0f, float tv = 0.0f, uint color = 0xffff0000 );

    float x;
	float y;
	float z;
	float rhw ;
	D3DCOLOR color;
	float tu;
	float tv;

};

typedef vector<MeshVertex> MeshVertexArray;
typedef vector<MeshVertex>::iterator MeshVertexArrayIt;
typedef vector<MeshIndex> MeshIndexArray;
typedef vector<MeshIndex>::iterator MeshIndexArrayIt;


class CGraphicsDevice
{
	template<typename T> friend class CSingletonDestroyer;
public:
	static CGraphicsDevice* GetInstance();

public:
	IDirect3DDevice9* GetDirect3DDevice();
	void Create( HWND hParent );
//	void Resize( int nWidth, int nHeight );
	void Destroy();
	
public:
	void CreateSwapChain( HWND hParent );
	void DestroySwapChain( HWND hParent );
	void SetActiveSwapChain( HWND hParent );
	void ResizeSwapChain( int nWidth, int nHeight );

	void Clear( DWORD dwColor = 0xff9099AE );
	void BeginScene();
	void EndScene();
	void Present();
	void DestroyInstance();

	void SetAmbientColor( DWORD dwColor );
	void GetAmbientColor( DWORD& dwColor );
	void EnableColorVertex( bool bEnable );
	void EnableTexturedVertex( bool bEnable );
	void EnableWireframe( bool bEnable );
	void SetTexture( IDirect3DTexture9* pTexture );

	void LockFrame( const BITMAPINFO* &ppHeader, const BYTE* &ppImage, int& nImgSize );
	void UnlockFrame(); 


protected:
	CGraphicsDevice();
	virtual ~CGraphicsDevice();


protected:
	struct SwapChainInfo
	{
		HWND hChildWnd;
		IDirect3DSwapChain9* pSwapChain;
	};


private:
	static CGraphicsDevice* m_pSelf;
	static CSingletonDestroyer<CGraphicsDevice> m_destroyer;

	BITMAPINFO m_biFrontBuffer;
	
	HWND m_hDeviceWnd;
	IDirect3D9* m_pD3D;
	IDirect3DDevice9* m_pDevice;
	SwapChainInfo m_ActiveSwapChain;
	list<SwapChainInfo> m_SwapChainList;



};



#endif //__GRAPHICSDEVICE_HPP__
