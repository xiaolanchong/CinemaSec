#include "stdafx.h"
#include ".\longoperation.h"

#define WND_CLASS _T("LongOperationWClass")

LongOperation::LongOperation(HWND hParentWnd, HINSTANCE hResInst, UINT nIDDescription, UINT nIDAnimation):
	m_hResInst(hResInst),
	m_hSyncEvent( CreateEvent( 0, TRUE, FALSE, 0 ) ),
	m_nIDDescription(nIDDescription),
	m_nIDAnimation(nIDAnimation),
	m_hAnimWnd(NULL),
	m_lTimer(0)
{


	WNDCLASSEX wcx; 

	// Fill in the window class structure with parameters 
	// that describe the main window. 

	wcx.cbSize = sizeof(wcx);				// size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW;  // redraw if size changes 
	wcx.lpfnWndProc = WndProc;			 // points to window procedure 
	wcx.cbClsExtra = 0;                // no extra class memory 
	wcx.cbWndExtra = 0;                // no extra window memory 
	wcx.hInstance = m_hResInst;         // handle to instance 
	wcx.hIcon = 0;              // predefined app. icon 
	wcx.hCursor = 0;                    // predefined arrow 
	wcx.hbrBackground = (HBRUSH)COLOR_BTNFACE;     // lightgray usually 
	wcx.lpszMenuName =  0;    // name of menu resource 
	wcx.lpszClassName = WND_CLASS;  // name of window class 
	wcx.hIconSm = 0;

/*	ATOM a =*/ RegisterClassEx(&wcx);

	Start(hParentWnd);
}

LongOperation::~LongOperation(void)
{
	Stop();
	CloseHandle(m_hSyncEvent);
}

HWND	LongOperation::Create(const RECT& rc)
{
	if( m_nIDDescription )
	{
		int res = LoadStringW( m_hResInst, m_nIDDescription, m_szDescription, MAX_DESCRIPTION_LENGTH );
		_ASSERTE( res );
	}
	else memset( m_szDescription, 0, sizeof(WCHAR) * MAX_DESCRIPTION_LENGTH );

	// HACK
	SIZE sizeOwn = { 256, 64 };
	RECT rcOwn;
	SIZE sizeCenter = 
	{ 
		(rc.left + rc.right )/2, (rc.top + rc.bottom )/2
	};
	SetRect( &rcOwn, 
		sizeCenter.cx - sizeOwn.cx/2, sizeCenter.cy - sizeOwn.cy/2, 
		sizeCenter.cx + sizeOwn.cx/2, sizeCenter.cy + sizeOwn.cy/2 );

	HWND hWnd = CreateWindowEx( WS_EX_TOPMOST|WS_EX_TOOLWINDOW, WND_CLASS, 0, WS_VISIBLE| WS_BORDER|WS_POPUP/*|SS_CENTER*/ , 
		rcOwn.left, rcOwn.top, sizeOwn.cx, sizeOwn.cy, 0, 0, GetModuleHandle(0), 0  );

	SetWindowLongPtr( hWnd, GWL_USERDATA, LONG_PTR(this) );

	if( m_nIDAnimation )
	{
		m_hAnimWnd = Animate_Create( hWnd, 1, WS_VISIBLE|WS_CHILD|ACS_TIMER|ACS_AUTOPLAY|ACS_TRANSPARENT, m_hResInst);
		_ASSERTE( m_hAnimWnd );
		SetWindowPos( m_hAnimWnd, 0, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOSIZE );
		Animate_OpenEx( m_hAnimWnd, m_hResInst, MAKEINTRESOURCE( m_nIDAnimation ) );
	}

	const DWORD c_dwPeriod = 500;
	SetTimer( hWnd,0xff, c_dwPeriod, 0 );
	return hWnd;
}

DWORD	WINAPI  LongOperation::ThreadProc(void* pParam)
{
	LongOperation* pThis = (LongOperation*) pParam;

	HWND hWnd = pThis->Create( pThis->m_rcMainWnd );
	//SetCapture( hWnd );
	while( true ) 
	{
		DWORD res = MsgWaitForMultipleObjects( 1, &pThis->m_hSyncEvent, FALSE, INFINITE, QS_ALLEVENTS   );
		
		if( res == WAIT_OBJECT_0 ) break;

		MSG msg;
		BOOL bRet;
		while( (bRet = PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )) != 0)
		{
			if (bRet == -1)
			{
				// handle the error and possibly exit
				break;
			}
			else
			{
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}
		}
		if( bRet == -1 ) break;

	}
	KillTimer( hWnd, 0xff );
//	ReleaseCapture();
	DestroyWindow(hWnd);
	return 0;
}


void	LongOperation::Start( HWND hMainWnd )
{
	ResetEvent(m_hSyncEvent);
	GetWindowRect( hMainWnd, &m_rcMainWnd );
	
	DWORD id;
	m_hThread = CreateThread( 0, 0, ThreadProc, this, 0, &id );
}

void	LongOperation::Stop()
{
	SetEvent( m_hSyncEvent );
	DWORD res = WaitForSingleObject( m_hThread, 100 );
	if( res == WAIT_TIMEOUT )
	{
		TerminateThread(m_hThread, ~0UL);
	}
	CloseHandle(m_hThread);
	m_hThread = 0;
}

LRESULT	LongOperation::WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam  )
{
	const int c_nDotNumber = 4;
	const int c_nBackColor = COLOR_BTNFACE;
	LongOperation* pThis = GetThis(hWnd);
	switch(msg) 
	{
	case WM_PAINT:
		{	
			if( !pThis )
			{
				ValidateRect( hWnd, NULL );
				break;
			}

			std::wstring sInitText( pThis->m_szDescription /*L"Operation is proceeding"*/);
			
			std::wstring sEndText (sInitText);
			sEndText += std::wstring( c_nDotNumber, L'.');

			std::wstring sAdd(pThis->m_lTimer, L'.');
			std::wstring sTempString = sInitText + sAdd;

		PAINTSTRUCT ps;
		BeginPaint( hWnd, &ps );
		RECT rc;
		GetClientRect( hWnd, &rc );
	
		if( ps.fErase )
		{
			// erase background
			HBRUSH hBrush = GetSysColorBrush( c_nBackColor );//CreateSolidBrush( RGB(255,255,255) );
			FillRect( ps.hdc, &rc, hBrush );
		}

		RECT rcCalc = {0,0,0,0}; 
		//rcCalc = rc;
		int nOldBackMode = SetBkMode( ps.hdc, TRANSPARENT );
		DrawTextW( ps.hdc, sEndText.c_str(), -1, &rcCalc, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_CALCRECT);
		
		if( pThis->m_hAnimWnd )
		{
			RECT rcAnim;
			GetClientRect( pThis->m_hAnimWnd, &rcAnim );
			rc.left		= rcAnim.right/2 + rc.right/2 - rcCalc.right/2 ;
		}
		else
			rc.left += rc.right/2 - rcCalc.right/2;
		DrawTextW( ps.hdc, sTempString.c_str(), -1, &rc, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		SetBkMode( ps.hdc, nOldBackMode );
//		DeleteObject(hBrush);
		EndPaint(hWnd, &ps);
		}
		break;
	case WM_TIMER:
		{
			if( !pThis ) break;
			++pThis->m_lTimer ;
			if( pThis->m_lTimer > c_nDotNumber ) pThis->m_lTimer  = 0;
			InvalidateRect( hWnd, 0, TRUE );
		}
		break;
	case WM_ERASEBKGND:
		{
			HDC hDC = (HDC)wParam;
			RECT rc;
			GetClientRect( hWnd, &rc );

			OSVERSIONINFOEXW ver;
			ver.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEXW );
			GetVersionExW( (OSVERSIONINFOW*)&ver );
			if( ver.dwMajorVersion == 5 && ver.dwMinorVersion == 0 )
			{
				// HACK for win2k
				HBRUSH hBrush = CreateSolidBrush( RGB(255,255,255) );
				FillRect( hDC, &rc, hBrush );
				DeleteObject( hBrush );
			}
			else
			{
				// erase background
				HBRUSH hBrush = GetSysColorBrush( c_nBackColor );
				FillRect( hDC, &rc, hBrush );
			}
			return TRUE;
		}
	break;
	case WM_CTLCOLORSTATIC:
		{
			HDC		hAnimateDC	= (HDC)wParam;
			HWND	hAnimateWnd = (HWND)lParam;
			if( pThis && pThis->m_hAnimWnd  && hAnimateWnd == hAnimateWnd )
			{
				SetBkColor( hAnimateDC,  GetSysColor( c_nBackColor ));
				return (LRESULT)GetSysColorBrush( c_nBackColor );
			}
		}
		break;
	default:
		return DefWindowProc( hWnd, msg, wParam, lParam );
	}
	return 0;
}

LongOperation* LongOperation::GetThis( HWND hWnd )
{
	LONG_PTR lParam = GetWindowLongPtr( hWnd, GWL_USERDATA );
	return reinterpret_cast<LongOperation*>( lParam );
}