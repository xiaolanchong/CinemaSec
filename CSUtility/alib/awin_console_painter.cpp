///////////////////////////////////////////////////////////////////////////////////////////////////
// windows_console_painter.cpp
// ---------------------
// begin     : Sep 2005
// modified  : 20 Sep 2005
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : aaahaaah@hotmail.com, aaah@mail.ru
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "console_paint_interface.h"

#pragma once

namespace alib
{

const int DEFAULT_WIN_POS = 16;
const int DEFAULT_WIN_SIZE = 256;
const int EXTRA_TEXT_HEIGHT = 2;

const DWORD RESIZABLE_WINDOW = (WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
const DWORD NON_RESIZABLE_WINDOW = WS_CAPTION;

const TCHAR szWindowClass[] = _T("DemoWindowClass");     // the main window class name
static BOOL g_bRegistered = FALSE;

DWORD WINAPI WinPainterThreadProc( LPVOID lpParameter );
LRESULT CALLBACK WindowProcedure( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

//=================================================================================================
//=================================================================================================
struct WindowsConsolePainter : public alib::IConsolePainter
{
  CRITICAL_SECTION m_critSect;    //!< data protector
  HWND             m_hwnd;        //!< handle of demo window
  HINSTANCE        m_hInstance;   //!< handle of application instance
  HANDLE           m_hThread;     //!< handle of window thread
  BITMAPINFO       m_bi;          //!< bitmap header
  AImage           m_image;       //!< temporal image
  bool             m_bResizable;  //!< nonzero if the window can be resized
  bool             m_bInvertY;    //!< nonzero means that axis Y must be inverted on drawing
  bool             m_bReady;      //!< nonzero if the object has been properly initialized
  StdStr           m_title;       //!< window title
  StdStr           m_statusText;  //!< status bar text
  int              m_textHeight;  //!< height of status bar

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void ResizeWindow()
{
  if (m_hwnd == 0)
    return;

  int   W = (m_image.empty() ? 100 : m_image.width());
  int   H = (m_image.empty() ? 100 : m_image.height());
  DWORD style = (m_bResizable ? RESIZABLE_WINDOW : NON_RESIZABLE_WINDOW);
  RECT  rect;

  rect.left = 0;
  rect.right = W;
  rect.top = 0;
  rect.bottom = H + m_textHeight;

  if (::AdjustWindowRect( &rect, style, FALSE ))
  {
    RECT pos;
    if (::GetWindowRect( m_hwnd, &pos ))
      ::MoveWindow( m_hwnd, pos.left, pos.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
  }
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void PrintText( HDC hdc, const RECT & clientRect )
{
  SIZE size;
  RECT rect = clientRect;
  int  y = rect.bottom - m_textHeight + EXTRA_TEXT_HEIGHT/2;
  int  length = (int)(_tcslen( m_statusText.c_str() ));

  COLORREF old = ::SetBkColor( hdc, ::GetSysColor( COLOR_3DFACE ) );
  ::TextOut( hdc, 0, y, m_statusText.c_str(), length );
  ::GetTextExtentPoint32( hdc, m_statusText.c_str(), length, &size );
  rect.left = size.cx;
  rect.top = y;
  rect.bottom = y + m_textHeight;
  if ((rect.left < rect.right) && (rect.top < rect.bottom))
    FillRect( hdc, &rect, (HBRUSH)(COLOR_3DFACE+1) );
  ::SetBkColor( hdc, old );
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void DrawImage( HDC hdc, const RECT & clientRect )
{
  if (m_image.empty())
    return;

  int cx = clientRect.right - clientRect.left;
  int cy = clientRect.bottom - clientRect.top;

  if ((m_bi.bmiHeader.biSize == 0) ||
      (m_bi.bmiHeader.biWidth < m_image.width()) || (cx < m_image.width()) ||
      (abs( m_bi.bmiHeader.biHeight ) < m_image.height()) || (cy < (m_image.height()+m_textHeight)))
  {
    m_bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_bi.bmiHeader.biWidth = m_image.width();
    m_bi.bmiHeader.biHeight = (m_bInvertY ? -(m_image.height()) : +(m_image.height()));
    m_bi.bmiHeader.biPlanes = 1;
    m_bi.bmiHeader.biBitCount = 32;
    m_bi.bmiHeader.biCompression = BI_RGB;
    m_bi.bmiHeader.biSizeImage = m_image.size() * sizeof(AImage::value_type);
    ResizeWindow();
  }

  ::SetDIBitsToDevice( hdc, 0, -m_textHeight, cx, cy,
                       0, 0, 0, m_image.height(), m_image.begin(), &m_bi, DIB_RGB_COLORS );
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
WindowsConsolePainter( const TCHAR * title, bool bResizable, bool bInvertAxisY )
{
  ::InitializeCriticalSection( &m_critSect );
  m_hwnd = 0;
  m_hInstance = (HINSTANCE )(::GetModuleHandle( 0 ));
  m_hThread = 0;
  memset( &m_bi, 0, sizeof(m_bi) );
  m_image.release();
  m_bResizable = bResizable;
  m_bInvertY = bInvertAxisY;
  m_bReady = false;
  m_title = (title != 0) ? title : _T("Demo window");
  m_statusText.clear();
  m_textHeight = 0;

  // Registering window class once in the main thread!
  if (m_hInstance != 0)
  {
    WNDCLASSEX wcex;

    wcex.cbSize        = sizeof(WNDCLASSEX); 
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = (WNDPROC)WindowProcedure;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = m_hInstance;
    wcex.hIcon         = LoadIcon( 0, IDI_APPLICATION );
    wcex.hCursor       = LoadCursor( 0, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
    wcex.lpszMenuName  = 0;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm       = 0;

    if (!g_bRegistered)
      g_bRegistered = ::RegisterClassEx( &wcex );

    if (g_bRegistered)
      m_hThread = ::CreateThread( 0, 0, WinPainterThreadProc, this, 0, 0 );
  }
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
virtual ~WindowsConsolePainter()
{
  ::EnterCriticalSection( &m_critSect );
  m_bReady = false;
  if ((m_hThread != 0) && (m_hwnd != 0))
  {
    DWORD wait = WAIT_TIMEOUT;
    for (int t = 10; (t < 3000) && (wait == WAIT_TIMEOUT); t += 10)
    {
      ::PostMessage( m_hwnd, WM_QUIT, 0, 0 );
      wait = ::WaitForSingleObject( m_hThread, t );
    }

    if ((wait == WAIT_TIMEOUT) || (wait == WAIT_FAILED))
    {
      std::cout << "Failed to close demo window normally" << std::endl;
      if (!(::TerminateThread( m_hThread, 1 )))
        std::cout << "Failed to terminate demo window's thread" << std::endl;
    }

    if (!(::CloseHandle( m_hThread )))
      std::cout << "Failed to close handle of demo window's thread" << std::endl;
  }
  ::LeaveCriticalSection( &m_critSect );

  ::DeleteCriticalSection( &m_critSect );
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
virtual void DrawDemoImage( const AImage * pImage, const TCHAR * statusText  ) throw(...)
{
  ::EnterCriticalSection( &m_critSect );
  if ((m_hwnd != 0) && m_bReady)
  {
    HDC hdc = ::GetDC( m_hwnd );
    if (hdc != 0)
    {
      RECT clientRect;
      ::GetClientRect( m_hwnd, &clientRect );

      if (pImage != 0) m_image = (*pImage);
      DrawImage( hdc, clientRect );

      m_statusText = (statusText != 0) ? statusText : _T("");
      PrintText( hdc, clientRect );

      ::ReleaseDC( m_hwnd, hdc );
    }
  }
  ::LeaveCriticalSection( &m_critSect );
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
virtual void DrawDemoImage( const Arr2ub * pImage, const TCHAR * statusText  ) throw(...)
{
  ::EnterCriticalSection( &m_critSect );
  if ((m_hwnd != 0) && m_bReady)
  {
    HDC hdc = ::GetDC( m_hwnd );
    if (hdc != 0)
    {
      RECT clientRect;
      ::GetClientRect( m_hwnd, &clientRect );

      // Copy grayscaled image to color one.
      if (pImage != 0)
      {
        m_image.resize2( *pImage, false );
        ASSERT( sizeof(AImage::value_type) == 4 );

        ubyte       * dst = reinterpret_cast<ubyte*>( m_image.begin() );
        const ubyte * src = pImage->begin();

        for (int i = 0, n = m_image.size(); i < n; i++)
        {
          dst[0] = (dst[1] = (dst[2] = src[i]));
          dst += sizeof(AImage::value_type);
        }
        ASSERT( dst == reinterpret_cast<ubyte*>( m_image.end() ) );
      }

      DrawImage( hdc, clientRect );

      m_statusText = (statusText != 0) ? statusText : _T("");
      PrintText( hdc, clientRect );

      ::ReleaseDC( m_hwnd, hdc );
    }
  }
  ::LeaveCriticalSection( &m_critSect );
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
virtual void SwapAndDrawDemoImage( AImage * pImage, const TCHAR * statusText  ) throw(...)
{
  ::EnterCriticalSection( &m_critSect );
  if ((m_hwnd != 0) && m_bReady)
  {
    HDC hdc = ::GetDC( m_hwnd );
    if (hdc != 0)
    {
      RECT clientRect;
      ::GetClientRect( m_hwnd, &clientRect );

      if (pImage != 0) pImage->swap( m_image );
      DrawImage( hdc, clientRect );

      m_statusText = (statusText != 0) ? statusText : _T("");
      PrintText( hdc, clientRect );

      ::ReleaseDC( m_hwnd, hdc );
    }
  }
  ::LeaveCriticalSection( &m_critSect );
}

};


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
DWORD WINAPI WinPainterThreadProc( LPVOID lpParameter )
{
  WindowsConsolePainter * painter = reinterpret_cast<WindowsConsolePainter*>( lpParameter );
  if ((painter == 0) || (painter->m_hInstance == 0))
    return 1;

  // Create and show window.
  DWORD style = (painter->m_bResizable ? RESIZABLE_WINDOW : NON_RESIZABLE_WINDOW);
  painter->m_hwnd = ::CreateWindow( szWindowClass, painter->m_title.c_str(), (style | WS_OVERLAPPED),
                                    CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WIN_SIZE, DEFAULT_WIN_SIZE,
                                    0, 0, painter->m_hInstance, 0 );
  if (painter->m_hwnd == 0)
    return 3;
  ShowWindow( painter->m_hwnd, SW_SHOWNORMAL );
  UpdateWindow( painter->m_hwnd );

  // Calculate status bar height.
  HDC hdc = ::GetDC( painter->m_hwnd );
  if (hdc != 0)
  {
    TEXTMETRIC tm;
    ::GetTextMetrics( hdc, &tm );
    ::ReleaseDC( painter->m_hwnd, hdc );
    painter->m_textHeight = tm.tmHeight + EXTRA_TEXT_HEIGHT;
  }

  // Store pointer to the painter as associated window data.
  ::SetWindowLongPtr( painter->m_hwnd, GWLP_USERDATA, (LONG_PTR)painter );
  ::SetWindowPos( painter->m_hwnd, HWND_TOP, DEFAULT_WIN_POS, DEFAULT_WIN_POS,
                  DEFAULT_WIN_SIZE, DEFAULT_WIN_SIZE,
                  SWP_SHOWWINDOW | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE );

  // Windows' main message loop.
  MSG msg;
  painter->m_bReady = true;
  while (GetMessage( &msg, 0, 0, 0 )) 
  {
    if (!TranslateAccelerator( msg.hwnd, 0, &msg )) 
    {
      TranslateMessage( &msg );
      DispatchMessage( &msg );
    }
  }
  return 0;
}


//-------------------------------------------------------------------------------------------------
//  FUNCTION: WindowProcedure(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND - process the application menu
//  WM_PAINT   - Paint the main window
//  WM_DESTROY - post a quit message and return
//-------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProcedure( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
  PAINTSTRUCT             ps;
  HDC                     hdc;
  WindowsConsolePainter * painter = 0;

  switch (message) 
  {
    case WM_PAINT:
      hdc = BeginPaint( hwnd, &ps );
      painter = reinterpret_cast<WindowsConsolePainter*>( ::GetWindowLongPtr( hwnd, GWLP_USERDATA ) );
      if ((painter != 0) && !(painter->m_image.empty()))
      {
        if (::TryEnterCriticalSection( &(painter->m_critSect) ))
        {
          RECT clientRect;
          ::GetClientRect( hwnd, &clientRect );
          painter->DrawImage( hdc, clientRect );
          painter->PrintText( hdc, clientRect );
          ::LeaveCriticalSection( &(painter->m_critSect) );
        }
      }
      EndPaint( hwnd, &ps );
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    case WM_SYSCOMMAND:
      if ((wParam & 0xFFF0) != SC_CLOSE)
        return DefWindowProc( hwnd, message, wParam, lParam );
      break;

    default:
      return DefWindowProc( hwnd, message, wParam, lParam );
  }
  return 0;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
alib::IConsolePainter *
CreateWindowsConsolePainter( const TCHAR * title, bool bResizable, bool bInvertAxisY )
{
  WindowsConsolePainter * p = new WindowsConsolePainter( title, bResizable, bInvertAxisY );
  return p;
}

} // namespace alib

