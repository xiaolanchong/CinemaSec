// DlgShowStream.cpp: implementation of the CDlgShowStream class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "DlgShowStream.h"

#include "resource.h"

CDlgShowStream::CDlgShowStream(CHCS::IStream* pStream)
	: m_pStream(pStream)
	, m_pFrame(NULL)
	, m_bOwner(false)
{
	m_nTimer = 0;

	if(m_pStream)
		m_pStream->AddRef();
}

CDlgShowStream::~CDlgShowStream()
{
	if(m_pFrame)
		m_pFrame->Release();

	if(m_pStream)
		m_pStream->Release();
}

int CDlgShowStream::DoModal(HWND hWndParent, LPARAM dwInitParam)
{
	return (int)DialogBoxParam(_Module.GetResourceInstance(),
		MAKEINTRESOURCE(IDD_SHOWSTREAM), hWndParent,
		(DLGPROC)DlgProc, (LPARAM)(LONG_PTR)this);
}

bool CDlgShowStream::Create(HWND hWndParent, CHCS::IStream* pStream)
{
	HWND hWnd = NULL;
	CDlgShowStream* pDlg = NULL;
	
	
	pDlg = new CDlgShowStream(pStream);

	if(pDlg)
	{
		pDlg->m_bOwner = true;

		hWnd = CreateDialogParam(_Module.GetResourceInstance(),
			MAKEINTRESOURCE(IDD_SHOWSTREAM), hWndParent,
			(DLGPROC)DlgProc, (LPARAM)(LONG_PTR)pDlg);

		if(hWnd)
		{
			ShowWindow(hWnd, SW_SHOW);
			return true;
		}

		delete pDlg;
	}

	return false;
}

BOOL CALLBACK CDlgShowStream::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandled = TRUE;
	CDlgShowStream *pThis = (CDlgShowStream*)(LONG_PTR)GetWindowLong(hWnd, DWL_USER);

	switch(msg)
	{
	case WM_INITDIALOG:
		{
			SetWindowLong(hWnd, DWL_USER, (LONG)lParam);
			pThis = (CDlgShowStream*)(LONG_PTR)lParam;

			HICON hIcon = ::LoadIcon(_Module.GetResourceInstance(),
				MAKEINTRESOURCE(IDI_MAIN_ICON));

			SendMessage(hWnd, SB_SETICON, TRUE, (LPARAM)hIcon);
			SendMessage(hWnd, SB_SETICON, FALSE, (LPARAM)hIcon);

			if(pThis->m_pStream)
				pThis->m_nTimer = (UINT)SetTimer(hWnd, 1, 100, NULL);

			SetWindowText(hWnd, TEXT("ShowStream"));
		}
		break;

	case WM_DESTROY:
		{
			if(pThis->m_bOwner)
				delete pThis;
		}
		break;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDOK:
			case IDCANCEL:
				{
					if(pThis->m_nTimer)
						KillTimer(hWnd, pThis->m_nTimer);

					if(pThis->m_bOwner)
						DestroyWindow(hWnd);
					else
                    	EndDialog(hWnd, LOWORD(wParam));
				}
				break;

			default:
				bHandled = FALSE;
			}
		}
		break;

	case WM_SIZE:
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_GETMINMAXINFO:
		{
			MINMAXINFO* lpInfo = (MINMAXINFO*)lParam;

			if(pThis->m_pFrame)
			{
				BITMAPINFO bi;
				pThis->m_pFrame->GetFormat(&bi);

				lpInfo->ptMinTrackSize.x = bi.bmiHeader.biWidth/4; 
				lpInfo->ptMinTrackSize.y = bi.bmiHeader.biHeight/4;

				lpInfo->ptMaxTrackSize.x = bi.bmiHeader.biWidth; 
				lpInfo->ptMaxTrackSize.y = bi.bmiHeader.biHeight;

				lpInfo->ptMaxSize.x = bi.bmiHeader.biWidth; 
				lpInfo->ptMaxSize.y = bi.bmiHeader.biHeight;
			}
		}
		break;

	case WM_LBUTTONDBLCLK:
		{
			::SendMessage(hWnd, WM_NCLBUTTONDBLCLK, HTCAPTION, 0);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;

	case WM_LBUTTONDOWN:
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;

	case WM_LBUTTONUP:
		SendMessage(hWnd, WM_NCLBUTTONUP, HTCAPTION, 0);
		break;

	case WM_MBUTTONUP:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case WM_TIMER:
		{
			SAFE_RELEASE(pThis->m_pFrame);

			if(pThis->m_pStream->GetNextFrame(&pThis->m_pFrame, 200))
				InvalidateRect(hWnd, NULL, FALSE);
		}
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			::BeginPaint(hWnd, &ps);

			RECT rectClient;
			GetClientRect(hWnd, &rectClient);

			HDC hDC = ::GetDC(hWnd);

			HGDIOBJ hOldBrush = ::SelectObject(hDC, ::GetStockObject(BLACK_BRUSH));

			if(pThis->m_pFrame)
			{
				::SetStretchBltMode(hDC, STRETCH_HALFTONE);
				::SetBkMode(hDC, TRANSPARENT);
				::SetTextColor(hDC, 0xFF8000);

				if(!pThis->m_pFrame->Draw(hDC, 0, 0, rectClient.right, rectClient.bottom))
				{
					LPCTSTR stNoDraw =  TEXT("Draw failed");

					FillRect(hDC, &rectClient, (HBRUSH)(COLOR_WINDOW+1));
					TextOut(hDC, 5, 5, stNoDraw, lstrlen(stNoDraw));
				}
				else
				{
					__time64_t ltime = pThis->m_pFrame->GetTime();

					LPCTSTR stImgTime = _tctime64(&ltime);
					LPCTSTR stImgNoSignal = TEXT("No signal");

					if(stImgTime)
						::TextOut(hDC, 5, rectClient.bottom - 20, stImgTime, lstrlen(stImgTime)-1);

					if(!pThis->m_pFrame->IsSignalLocked())
						::TextOut(hDC, 5, 5, stImgNoSignal, lstrlen(stImgNoSignal));
				}
			}
			else
			{
				FillRect(hDC, &rectClient, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
			}

			GdiFlush();
			SelectObject(hDC, hOldBrush);

			ReleaseDC(hWnd, hDC);
			EndPaint(hWnd, &ps);
		}
		break;

	default:
		bHandled = FALSE;
	}

	return bHandled;
}