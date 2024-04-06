// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "CVMCadComponentTest.h"
#include "CVMCadComponent.hpp"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WIDTHBYTES(bits) \
	((DWORD)(((bits)+31) & (~31)) / 8)

// CChildView

CChildView::CChildView()
{
	m_pRoomRender = NULL;
}

CChildView::~CChildView()
{
	m_pRoomRender->Release();
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);


	return TRUE;
}


void CChildView::OnLButtonDown( UINT nFlags, CPoint point )
{
    if( m_pRoomRender != NULL )
	{
        m_CameraId = m_pRoomRender->GetCamera( point );
		m_LMBDownPos.first = point.x;
		m_LMBDownPos.second = point.y;

		SetTimer( 1, 1, NULL );


		BITMAPINFO* bm;
		BYTE* pData;
		int imgSize;

		m_pRoomRender->TestComponent();
 

		OnPaint();

		m_pRoomRender->LockFrame( bm, pData, imgSize );



		m_pRoomRender->UnlockFrame();
	}
}


void CChildView::OnPaint() 
{

	if( m_pRoomRender == NULL )
	{
		CreateRender( GetSafeHwnd(), &m_pRoomRender );
		CFile file( L"Test.cvm", CFile::modeRead | CFile::typeBinary );

		m_wndStatic.Create( L"", WS_CHILD|WS_VISIBLE, CRect(0,10,400,300), this  );
	//	m_wndStatic2.Create( L"", WS_CHILD|WS_VISIBLE, CRect(450,10,900,300), this  );

		ULONGLONG lg = file.GetLength();
		BYTE* pArray = new BYTE[ lg ];

		file.Read( pArray, lg );

		m_pRoomRender->Load( pArray, lg );

		delete[] pArray;

		m_nCounter = 0;
 
		BITMAPINFO bmInfo_tl;
		BITMAPINFO bmInfo_tr;
		BITMAPINFO bmInfo_bl;
		BITMAPINFO bmInfo_br;


		std::wstring str = L"y8.bmp"; 

		CFile file_tl( L"y8.bmp", CFile::modeRead  | CFile::typeBinary | CFile::osRandomAccess  );
		CFile file_tr( L"rgb24.bmp", CFile::modeRead  | CFile::typeBinary | CFile::osRandomAccess  );
		CFile file_bl( L"yuv.bmp", CFile::modeRead  | CFile::typeBinary | CFile::osRandomAccess  );
		CFile file_br( L"rgb32.bmp", CFile::modeRead  | CFile::typeBinary | CFile::osRandomAccess  );

 
		file_tl.Seek( sizeof( BITMAPFILEHEADER ), CFile::begin  );
		file_tl.Read( &bmInfo_tl, sizeof( BITMAPINFO ) ); 

		file_tr.Seek( sizeof( BITMAPFILEHEADER ), CFile::begin  );
		file_tr.Read( &bmInfo_tr, sizeof( BITMAPINFO ) ); 

		file_bl.Seek( sizeof( BITMAPFILEHEADER ), CFile::begin  );
		file_bl.Read( &bmInfo_bl, sizeof( BITMAPINFO ) ); 

		file_br.Seek( sizeof( BITMAPFILEHEADER ), CFile::begin  );
		file_br.Read( &bmInfo_br, sizeof( BITMAPINFO ) ); 
 
	
		boost::scoped_array<BYTE> pBuffer_tl( new BYTE[  bmInfo_tl.bmiHeader.biSizeImage  ] );
		boost::scoped_array<BYTE> pBuffer_tr( new BYTE[  bmInfo_tr.bmiHeader.biSizeImage  ] );
		boost::scoped_array<BYTE> pBuffer_bl( new BYTE[  bmInfo_bl.bmiHeader.biSizeImage  ] );
		boost::scoped_array<BYTE> pBuffer_br( new BYTE[  bmInfo_br.bmiHeader.biSizeImage  ] );


		file_tl.Read( pBuffer_tl.get(), bmInfo_tl.bmiHeader.biSizeImage );
		file_tr.Read( pBuffer_tr.get(), bmInfo_tr.bmiHeader.biSizeImage );
		file_bl.Read( pBuffer_bl.get(), bmInfo_bl.bmiHeader.biSizeImage );
		file_br.Read( pBuffer_br.get(), bmInfo_br.bmiHeader.biSizeImage );

		{
			m_biTL = bmInfo_tl;
			boost::shared_array<BYTE> ptr( new BYTE[  bmInfo_tl.bmiHeader.biSizeImage ] );
			m_pTL = ptr;
			memcpy( m_pTL.get(), pBuffer_tl.get(), bmInfo_tl.bmiHeader.biSizeImage );
		}

		{
			m_biTR = bmInfo_tr;

			boost::shared_array<BYTE> ptr( new BYTE[  bmInfo_tr.bmiHeader.biSizeImage ] );
			m_pTR = ptr;
			memcpy( m_pTR.get(), pBuffer_tr.get(),  bmInfo_tr.bmiHeader.biSizeImage );
		}
		{
			m_biBL = bmInfo_bl;

			boost::shared_array<BYTE> ptr( new BYTE[   bmInfo_bl.bmiHeader.biSizeImage ] );
			m_pBL = ptr;
			memcpy( m_pBL.get(), pBuffer_bl.get(),  bmInfo_bl.bmiHeader.biSizeImage );
		}
		{
			m_biBR = bmInfo_br;

			boost::shared_array<BYTE> ptr( new BYTE[   bmInfo_br.bmiHeader.biSizeImage ] );
			m_pBR = ptr;
			memcpy( m_pBR.get(), pBuffer_br.get(),  bmInfo_br.bmiHeader.biSizeImage );
		}
	}


if( m_pRoomRender != NULL )
{
	std::wstring buffer(100,L'');
	wsprintf( &buffer[0], L"::Current camera is %d,%d::Pos(%d,%d)::Conter is %d", m_CameraId.first, m_CameraId.second, 
		m_LMBDownPos.first, m_LMBDownPos.second, m_nCounter++ );
	
	m_pRoomRender->SetText( buffer.c_str(), RGB( 0xff,0xff,0xff ) );

	int x; 
	int y;
	
	x = rand()%2;
	y = rand()%3;

	m_pRoomRender->SetImage( x, y, 0, 0 );

	x = rand()%2;
	y = rand()%3;

	m_pRoomRender->SetImage( x, y, &m_biTR, m_pTR.get() );

	x = rand()%2;
	y = rand()%3;

	m_pRoomRender->SetImage( x, y, &m_biBL, m_pBL.get() );

	x = rand()%2;
	y = rand()%3;

	m_pRoomRender->SetImage( x, y, &m_biBR, m_pBR.get() );

  
	const BITMAPINFO* pBI;
	const BYTE* pImage;
	int imgSize;

	CRect rect;
	m_wndStatic.GetClientRect( rect );
	m_pRoomRender->Render( m_wndStatic.m_hWnd, RGB( 0xdd,0xcc,0xcc ) );
//	m_pRoomRender->Render( m_wndStatic2.m_hWnd, RGB( 0xd0,0xd0,0xd0 ) ); 

}

}

void CChildView::OnSize( UINT nType, int cx, int cy )
{
	if( m_pRoomRender != NULL )
	{
		
		m_wndStatic.SetWindowPos( NULL, 0,0, cx, cy, SWP_NOZORDER );
//		m_wndStatic2.SetWindowPos( NULL, 450,0, cx/2, cy/2, SWP_NOZORDER );

		m_pRoomRender->Resize( m_wndStatic.GetSafeHwnd(), cx, cy );
//		m_pRoomRender->Resize( m_wndStatic2.GetSafeHwnd(), cx/2, cy/2 );

	}

}


void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	OnPaint();
}
