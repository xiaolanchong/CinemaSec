// Y800VfW.cpp: implementation of the CY800VFW class.
//
//////////////////////////////////////////////////////////////////////

#include "Y800ED.h"
#include "Y800VfW.h"

#include "resource.h"

//////////////////////////////////////////////////////////////////////
// CY800VFW
//////////////////////////////////////////////////////////////////////

CY800VFW* __stdcall CY800VFW::Open(ICOPEN* icOpen)
{
	if(icOpen && icOpen->fccType != ICTYPE_VIDEO)
		return NULL;

	CY800VFW* pInst = new CY800VFW();

	if(icOpen)
		icOpen->dwError = pInst ? ICERR_OK : ICERR_MEMORY;

	return pInst;
}

LRESULT __stdcall CY800VFW::Close(CY800VFW* pInst)
{
	if(pInst)
	{
		if(IsBadReadPtr(pInst, sizeof(CY800VFW)))
		{
			TCHAR stMessage[128];
			wsprintf(stMessage, TEXT("Error: Y800ED!DriverProc(0x%08X, *, DRV_CLOSE, *, *) bad DriverId\n"), pInst);

			OutputDebugString(stMessage);
			return (LRESULT)ICERR_BADPARAM;
		}

		delete pInst; // this caused problems when deleting at app close time
	}

	return (LRESULT)DRVCNF_OK;
}

//////////////////////////////////////////////////////////////////////
// CY800VFW
//////////////////////////////////////////////////////////////////////

CY800VFW::CY800VFW()
{
	m_dwFCCUsed = FOURCC_Y800;

	m_bDebugInfo = FALSE;
	m_bDiscardData = FALSE;
	
	BZ2_bzBuffToBuffCompress = NULL;
	BZ2_bzBuffToBuffDecompress = NULL;

	m_hBZLib = NULL;

	m_pCFrame = NULL;
	m_pDFrame = NULL;
}

CY800VFW::~CY800VFW()
{
	if(m_hBZLib)
		::FreeLibrary(m_hBZLib);

	if(m_pCFrame)
		free(m_pCFrame);

	if(m_pDFrame)
		free(m_pDFrame);
}

bool CY800VFW::InitBZLib()
{
	if(m_hBZLib)
		return true;

	bool bInited = false;

	do
	{
		m_hBZLib = LoadLibrary("libbz2.dll");

		if(!m_hBZLib)
			break;

		BZ2_bzBuffToBuffCompress = (fBZ2_bzBuffToBuffCompress)GetProcAddress(m_hBZLib, "BZ2_bzBuffToBuffCompress");
		if(!BZ2_bzBuffToBuffCompress)
			break;

		BZ2_bzBuffToBuffDecompress = (fBZ2_bzBuffToBuffDecompress)GetProcAddress(m_hBZLib, "BZ2_bzBuffToBuffDecompress");
		if(!BZ2_bzBuffToBuffDecompress)
			break;

		bInited = true;
	}
	while(false);

	if(!bInited)
	{
		if(m_hBZLib) ::FreeLibrary(m_hBZLib);

		BZ2_bzBuffToBuffCompress = NULL;
		BZ2_bzBuffToBuffDecompress = NULL;

		m_hBZLib = NULL;
	}

	return bInited;
}

//////////////////////////////////////////////////////////////////////
// About dialog
//////////////////////////////////////////////////////////////////////

BOOL CY800VFW::QueryAbout()
{
	return TRUE;
}

DWORD CY800VFW::About(HWND hWnd)
{
	DialogBox(g_hModuleY800, MAKEINTRESOURCE(IDD_ABOUT), hWnd, ProcAbout);
	return ICERR_OK;
}

BOOL CALLBACK CY800VFW::ProcAbout(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_COMMAND)
	{
		switch(LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hwndDlg, 0);
			break;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Configure dialog
//////////////////////////////////////////////////////////////////////

BOOL CY800VFW::QueryConfigure()
{
	return TRUE;
}

DWORD CY800VFW::Configure(HWND hWnd)
{
	DialogBoxParam(g_hModuleY800, MAKEINTRESOURCE(IDD_CONFIGURE), hWnd, ProcConfigure, (LPARAM)this);
	return ICERR_OK;
}

BOOL CALLBACK CY800VFW::ProcConfigure(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CY800VFW* pThis = (CY800VFW*)(DWORD_PTR)::GetWindowLong(hwndDlg, GWL_USERDATA);

	if(uMsg == WM_INITDIALOG)
	{
		// remember this pointer in user data
		pThis = (CY800VFW*)lParam;
		SetWindowLong(hwndDlg, GWL_USERDATA, (LONG)lParam);

		// set compression mode
		::CheckRadioButton(hwndDlg, IDC_UNCOMPRESSED_RADIO, IDC_COMPRESSED_RADIO,
			pThis->m_dwFCCUsed == FOURCC_Y8BZ ? IDC_COMPRESSED_RADIO : IDC_UNCOMPRESSED_RADIO);

		// fill fourcc combobox
		//

		struct FCCNames	{
			DWORD	dwFCC;
			LPCTSTR	stName;
		};

		FCCNames FCCMap[] = {
			{ FOURCC_Y800, TEXT("Y800") },
			{ FOURCC_GRAY, TEXT("GRAY") },
			{ FOURCC_Y8,   TEXT("Y8") }	};

		int nItem;
		HWND hwndFormat = ::GetDlgItem(hwndDlg, IDC_FOURCC_COMBO);

		for(int i=0; i < countof(FCCMap); i++)
		{
			nItem = (int)::SendMessage(hwndFormat, CB_ADDSTRING, 0L, (LPARAM)FCCMap[i].stName);

			if(nItem != CB_ERR)
			{
				::SendMessage(hwndFormat, CB_SETITEMDATA, nItem, (LPARAM)FCCMap[i].dwFCC);

				if(FCCMap[i].dwFCC == pThis->m_dwFCCUsed)
					::SendMessage(hwndFormat, CB_SETCURSEL, nItem, 0L);
			}
		}

		// Set debug options
		::CheckDlgButton(hwndDlg, IDC_ENABLE_DEBUGINFO, pThis->m_bDebugInfo ? BST_CHECKED : BST_UNCHECKED);
		::CheckDlgButton(hwndDlg, IDC_DISCART_DATA,   pThis->m_bDiscardData ? BST_CHECKED : BST_UNCHECKED);
	}
	else if(uMsg == WM_COMMAND)
	{
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				bool bUseCompression;

				bUseCompression = ::IsDlgButtonChecked(hwndDlg, IDC_COMPRESSED_RADIO) == BST_CHECKED ? TRUE : FALSE;

				if(bUseCompression)
				{
					if(!pThis->InitBZLib())
					{
						::MessageBox(hwndDlg, TEXT("Did you install libbz2.dll?\nPlease install binaries from http://www.bzip.org"),
							TEXT("Unexpected error"), MB_OK | MB_ICONSTOP);

						::SetFocus( ::GetDlgItem(hwndDlg, IDC_COMPRESSED_RADIO) );
						break;
					}

					pThis->m_dwFCCUsed = FOURCC_Y8BZ;
				}
				else
				{
					HWND hwndFormat = ::GetDlgItem(hwndDlg, IDC_FOURCC_COMBO);
					int nItem = (int)::SendMessage(hwndFormat, CB_GETCURSEL, 0L, 0L);

					if(nItem != CB_ERR)
					{
						pThis->m_dwFCCUsed = (DWORD)::SendMessage(hwndFormat, CB_GETITEMDATA, nItem, 0L);
					}
					else
					{
						::MessageBox(hwndDlg, TEXT("Select FourCC to use"), TEXT("No fcc selected"), MB_OK | MB_ICONSTOP);
						::SetFocus(hwndFormat);
						break;
					}
				}

				// Enable global debug info if one of the instance enabled

				pThis->m_bDebugInfo   = ::IsDlgButtonChecked(hwndDlg, IDC_ENABLE_DEBUGINFO) == BST_CHECKED ? TRUE : FALSE;
				pThis->m_bDiscardData = ::IsDlgButtonChecked(hwndDlg, IDC_DISCART_DATA)     == BST_CHECKED ? TRUE : FALSE;

				if(pThis->m_bDebugInfo)
					g_bDebuginfo = TRUE;

				::EndDialog(hwndDlg, IDOK);
			}
			break;

		case IDCANCEL:
			EndDialog(hwndDlg, IDCANCEL);
			break;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Codec state
//////////////////////////////////////////////////////////////////////

DWORD CY800VFW::GetState(LPVOID pState, DWORD dwSize)
{
	if(!pState)
		return sizeof(DWORD);

	CopyMemory(pState, &m_dwFCCUsed, sizeof(DWORD));

	return ICERR_OK;
}

DWORD CY800VFW::SetState(LPVOID pState, DWORD dwSize)
{
	// Default state
	if(!pState) 
	{
		m_dwFCCUsed = FOURCC_Y800;
		return 0;
	}
	
	CopyMemory(&m_dwFCCUsed, pState, sizeof(DWORD));

	if( m_dwFCCUsed != FOURCC_Y800 &&
		m_dwFCCUsed != FOURCC_GRAY &&
		m_dwFCCUsed != FOURCC_Y8   &&
		m_dwFCCUsed != FOURCC_Y8BZ)
	{
		m_dwFCCUsed = FOURCC_Y800;
	}

	return 0; // sizeof(DWORD);
}

DWORD CY800VFW::GetInfo(ICINFO* icInfo, DWORD dwSize)
{
	if(icInfo == NULL)
		return sizeof(icInfo);

	if(dwSize < sizeof(icInfo))
		return 0;

	icInfo->dwSize = sizeof(icInfo);
	icInfo->fccType = ICTYPE_VIDEO;
	icInfo->fccHandler = FOURCC_Y800;
	icInfo->dwFlags = 0;
	icInfo->dwVersion    = 0x00010002; // 1.2
	icInfo->dwVersionICM = ICVERSION;

	wcscpy(icInfo->szName, L"Y800");
	wcscpy(icInfo->szDescription, L"Y800 Video Codec v1.2");

	return sizeof(icInfo);
}

//////////////////////////////////////////////////////////////////////
// Helper function
//////////////////////////////////////////////////////////////////////

BOOL CY800VFW::IsY800Format(const LPBITMAPINFOHEADER lpbiOut)
{
	if(lpbiOut && !IsBadReadPtr(lpbiOut, sizeof(BITMAPINFOHEADER)))
	{
		// check compression
		if( lpbiOut->biCompression == FOURCC_GRAY ||
			lpbiOut->biCompression == FOURCC_Y800 ||
			lpbiOut->biCompression == FOURCC_Y8   ||
			// Compressed Y8 format
			lpbiOut->biCompression == FOURCC_Y8BZ)
		{
			// check other parameters
			if( lpbiOut->biHeight >= 0 &&  // no negative height
				lpbiOut->biWidth  >= 0 &&  // valid width
				lpbiOut->biPlanes  == 1 && // one plane
			//	lpbiOut->biClrUsed == 0 && // no palette table
				lpbiOut->biClrImportant == 0 &&
				lpbiOut->biBitCount == 8)  // luminance only
			{
				// Everything is good
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

BOOL CY800VFW::IsSupportedFormat(const LPBITMAPINFOHEADER lpbiIn)
{
	if(!lpbiIn || IsBadReadPtr(lpbiIn, sizeof(LPBITMAPINFOHEADER)))
		return FALSE;

	// Work only with one plane images
	// width must be positive

	if(lpbiIn->biPlanes != 1 || lpbiIn->biWidth <= 0)
		return FALSE;

/*
	// If biHeight is negative, indicating a top-down DIB,
	// biCompression must be either BI_RGB or BI_BITFIELDS.
	// Top-down DIBs cannot be compressed. 

	if( lpbiIn->biHeight < 0 &&
		lpbiIn->biCompression != BI_RGB &&
		lpbiIn->biCompression != BI_BITFIELDS)
		return FALSE;
*/

	switch(lpbiIn->biCompression)
	{
	case BI_RGB:
	case BI_BITFIELDS:
	case mmioFOURCC('D','I','B',' '):
		{
			if((lpbiIn->biBitCount == 24 || lpbiIn->biBitCount == 32) &&
				lpbiIn->biClrUsed == 0)
				return TRUE;
		}
		break;

	case mmioFOURCC('Y','U','Y','2'):
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Compress
//////////////////////////////////////////////////////////////////////

DWORD CY800VFW::CompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	// Check output format
	if(lpbiOut && !IsY800Format(lpbiOut))
		return ICERR_BADFORMAT;

	// Check input format
	if(!lpbiIn)
		return ICERR_BADPARAM;

	return IsSupportedFormat(lpbiIn) ? ICERR_OK : ICERR_BADFORMAT;
}

DWORD CY800VFW::CompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	if(lpbiIn && !IsSupportedFormat(lpbiIn))
		return ICERR_BADFORMAT;

	if(!lpbiOut)
		return sizeof(BITMAPINFOHEADER);

	lpbiOut->biSize = sizeof(BITMAPINFOHEADER);
	lpbiOut->biBitCount = 8;
	lpbiOut->biPlanes   = 1;
	lpbiOut->biCompression = m_dwFCCUsed;
		
	lpbiOut->biClrUsed = 0;
	lpbiOut->biClrImportant = 0;
	lpbiOut->biXPelsPerMeter = 0;
	lpbiOut->biYPelsPerMeter = 0;

	lpbiOut->biWidth  = labs(lpbiIn->biWidth);
	lpbiOut->biHeight = labs(lpbiIn->biHeight);

	lpbiOut->biSizeImage = ((lpbiOut->biWidth + 3) & (~3)) * lpbiOut->biHeight;

	return ICERR_OK;
}

DWORD CY800VFW::CompressGetSize(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER)
{
	if(!lpbiIn)
		return ICERR_BADPARAM;

	if(lpbiIn->biWidth <= 0 || lpbiIn->biHeight == 0)
		return ICERR_BADFORMAT;

	return ((lpbiIn->biWidth + 3) & (~3)) * labs(lpbiIn->biHeight);
}

DWORD CY800VFW::CompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	if(m_dwFCCUsed == FOURCC_Y8BZ && !BZ2_bzBuffToBuffCompress)
		return ICERR_INTERNAL;

	return CompressQuery(lpbiIn, lpbiOut);
}

DWORD CY800VFW::CompressEnd()
{
	if(m_pCFrame)
	{
		free(m_pCFrame);
		m_pCFrame = NULL;
	}

	return ICERR_OK;
}

DWORD CY800VFW::Compress(ICCOMPRESS* icInfo, DWORD dwSize)
{
#ifdef _DEBUG
	if(!icInfo || IsBadReadPtr(icInfo, sizeof(ICCOMPRESS)))
		return ICERR_BADPARAM;

	if(CompressQuery(icInfo->lpbiInput, icInfo->lpbiOutput) != ICERR_OK)
		return ICERR_BADFORMAT;
#endif

	if(icInfo->lpckid)
		*icInfo->lpckid = FOURCC_Y800;

	if(icInfo->lpdwFlags)
		*icInfo->lpdwFlags = AVIIF_KEYFRAME;

	BOOL bRes = FALSE;

	if( icInfo->lpbiInput->biCompression == BI_RGB ||
		icInfo->lpbiInput->biCompression == BI_BITFIELDS ||
		icInfo->lpbiInput->biCompression == mmioFOURCC('D','I','B',' '))
	{
		if(icInfo->lpbiInput->biBitCount == 24)
		{
			bRes = CompressRGB24(icInfo->lpbiInput, icInfo->lpInput, icInfo->lpOutput);
		}
		else if(icInfo->lpbiInput->biBitCount == 32)
		{
			bRes = CompressRGB32(icInfo->lpbiInput, icInfo->lpInput, icInfo->lpOutput);
		}
	}
	else if(icInfo->lpbiInput->biCompression == mmioFOURCC('Y','U','Y','2'))
	{
		bRes = CompressYUY2(icInfo->lpbiInput, icInfo->lpInput, icInfo->lpOutput);
	}
	else
	{
		return ICERR_BADFORMAT;
	}

	icInfo->lpbiOutput->biSizeImage = ((icInfo->lpbiInput->biWidth + 3) & (~3)) * labs(icInfo->lpbiInput->biHeight);

	if(bRes && icInfo->lpbiOutput->biCompression == FOURCC_Y8BZ)
	{
		unsigned int nSize = icInfo->lpbiOutput->biSizeImage;

		if(!m_pCFrame)
			m_pCFrame = malloc(nSize*2);

		if(BZ2_bzBuffToBuffCompress && BZ_OK == BZ2_bzBuffToBuffCompress(
			reinterpret_cast<char*>(m_pCFrame), &nSize, 
			reinterpret_cast<char*>(icInfo->lpOutput), nSize, 9, 0, 0))
		{
			CopyMemory(icInfo->lpOutput, m_pCFrame, nSize);
			icInfo->lpbiOutput->biSizeImage = (DWORD)nSize;
		}
		else
			bRes = false;
	}

	if(m_bDiscardData)
		icInfo->lpbiOutput->biSizeImage = 2;

	return bRes ? ICERR_OK : ICERR_INTERNAL;
}

//////////////////////////////////////////////////////////////////////
// Decompress
//////////////////////////////////////////////////////////////////////

DWORD CY800VFW::DecompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	if(!lpbiIn)
		return ICERR_BADPARAM;

	// Check input format
	if(!IsY800Format(lpbiIn))
		return ICERR_BADFORMAT;

	if(!lpbiOut)
		return ICERR_OK;

	// must be 1:1 (no stretching)
	if(lpbiOut->biWidth != lpbiIn->biWidth || labs(lpbiOut->biHeight) != lpbiIn->biHeight)
		return ICERR_BADFORMAT;

	// Check output format
	return IsSupportedFormat(lpbiOut) ? ICERR_OK : ICERR_BADFORMAT;
}

DWORD CY800VFW::DecompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	if(lpbiIn && !IsY800Format(lpbiIn))
		return ICERR_BADFORMAT;

	if(!lpbiOut)
		return sizeof(BITMAPINFOHEADER);

	lpbiOut->biSize = sizeof(BITMAPINFOHEADER);
	lpbiOut->biBitCount = 32;
	lpbiOut->biPlanes   = 1;
	lpbiOut->biCompression = BI_RGB;

	lpbiOut->biClrUsed = 0;
	lpbiOut->biClrImportant = 0;
	lpbiOut->biXPelsPerMeter = 0;
	lpbiOut->biYPelsPerMeter = 0;

	lpbiOut->biWidth  = lpbiIn->biWidth;
	lpbiOut->biHeight = lpbiIn->biHeight;
	lpbiOut->biSizeImage = 4 * lpbiIn->biWidth * lpbiIn->biHeight;

	return ICERR_OK;
}

DWORD CY800VFW::DecompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	return DecompressQuery(lpbiIn, lpbiOut);
}

DWORD CY800VFW::DecompressEnd()
{
	if(m_pDFrame)
	{
		free(m_pDFrame);
		m_pDFrame = NULL;
	}

	return ICERR_OK;
}

DWORD CY800VFW::DecompressGetPalette(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	return ICERR_BADFORMAT; // palette-mapped output only
}

DWORD CY800VFW::Decompress(ICDECOMPRESS* icInfo, DWORD dwSize)
{
#ifdef _DEBUG
	if(!icInfo || IsBadReadPtr(icInfo, sizeof(ICDECOMPRESS)))
		return ICERR_BADPARAM;

	if(DecompressQuery(icInfo->lpbiInput, icInfo->lpbiOutput) != ICERR_OK)
		return ICERR_BADFORMAT;
#endif

	// Discarted data
	if(icInfo->lpbiInput->biSizeImage == 2)
	{
		ZeroMemory(icInfo->lpOutput, icInfo->lpbiOutput->biSizeImage);
		return ICERR_OK;
	}
	
	// Check. Is it compressed?
	//

	void* pInput = icInfo->lpInput;
	unsigned int nImageSize = ((icInfo->lpbiInput->biWidth + 3) & (~3)) * labs(icInfo->lpbiInput->biHeight);

	if(icInfo->lpbiInput->biCompression == FOURCC_Y8BZ)
	{
		if(!m_pDFrame)
		{
			m_pDFrame = malloc(nImageSize);
			InitBZLib();
		}

		if(m_pDFrame && BZ2_bzBuffToBuffDecompress &&
			BZ_OK == BZ2_bzBuffToBuffDecompress(
				reinterpret_cast<char*>(m_pDFrame), &nImageSize,
				reinterpret_cast<char*>(icInfo->lpInput), icInfo->lpbiInput->biSizeImage, 0, 0) )
		{
			pInput = m_pDFrame;
		}	
		else
		{
			return ICERR_INTERNAL;
		}
	}
	else if(icInfo->lpbiInput->biSizeImage != nImageSize)
		return ICERR_BADPARAM;

	// Decompress image
	//

	BOOL bRes = FALSE;

	if( icInfo->lpbiOutput->biCompression == BI_RGB ||
		icInfo->lpbiOutput->biCompression == BI_BITFIELDS ||
		icInfo->lpbiOutput->biCompression == mmioFOURCC('D','I','B',' '))
	{
		if(icInfo->lpbiOutput->biBitCount == 24)
		{
			bRes = DecompressRGB24(icInfo->lpbiOutput, pInput, icInfo->lpOutput);
		}
		else if(icInfo->lpbiOutput->biBitCount == 32)
		{
			bRes = DecompressRGB32(icInfo->lpbiOutput, pInput, icInfo->lpOutput);
		}
		else
		{
			return ICERR_BADFORMAT;
		}
	}
	else if(icInfo->lpbiOutput->biCompression == mmioFOURCC('Y','U','Y','2'))
	{
		bRes = DecompressYUY2(icInfo->lpbiOutput, pInput, icInfo->lpOutput);
	}
	else
	{
		return ICERR_BADFORMAT;
	}

	return bRes ? ICERR_OK : ICERR_INTERNAL;
}

//////////////////////////////////////////////////////////////////////
// Compress/Decompress functions (trusted)
//////////////////////////////////////////////////////////////////////

BOOL CY800VFW::CompressRGB24(LPBITMAPINFOHEADER lpbiIn, const void* pSrc, void* pDst)
{
//	const int yb = 6416;  // int(0.114*219/255*65536+0.5);
//	const int yg = 33039; // int(0.587*219/255*65536+0.5);
//	const int yr = 16829; // int(0.299*219/255*65536+0.5);

	const BYTE *pRGB;
	BYTE *pY8;

	long lCol, lRow;
	long lWidthY8 = (lpbiIn->biWidth + 3L) & (~3L);
	long lWidthRGB = (3L*lpbiIn->biWidth + 3L) & (~3L);
	
	for(lRow = 0; lRow < labs(lpbiIn->biHeight); lRow++)
	{
		pY8 = reinterpret_cast<BYTE*>(pDst) + lWidthY8 * lRow;

		if(lpbiIn->biHeight < 0)
			pRGB = reinterpret_cast<const BYTE*>(pSrc) + lWidthRGB * lRow;
		else
			pRGB = reinterpret_cast<const BYTE*>(pSrc) + lWidthRGB * (lpbiIn->biHeight - 1 - lRow);
		
		for(lCol = 0; lCol < lpbiIn->biWidth; lCol++)
		{
			pY8[0] = (BYTE)((6416*pRGB[0] + 33039*pRGB[1] + 16829*pRGB[2] + 0x108000) >> 16);

			pY8  += 1;
			pRGB += 3;
		}
	}

	return TRUE;
}

BOOL CY800VFW::CompressRGB32(LPBITMAPINFOHEADER lpbiIn, const void* pSrc, void* pDst)
{
	const BYTE *pRGB;
	BYTE *pY8;

	long lCol, lRow;
	long lWidthY8 = (lpbiIn->biWidth + 3L) & (~3L);

	for(lRow = 0; lRow < labs(lpbiIn->biHeight); lRow++)
	{
		pY8 = reinterpret_cast<BYTE*>(pDst) + lWidthY8 * lRow;

		if(lpbiIn->biHeight < 0)
			pRGB = reinterpret_cast<const BYTE*>(pSrc) + 4 * lpbiIn->biWidth * lRow;
		else
			pRGB = reinterpret_cast<const BYTE*>(pSrc) + 4 * lpbiIn->biWidth * (lpbiIn->biHeight - 1 - lRow);

		for(lCol = 0; lCol < lpbiIn->biWidth; lCol++)
		{
			pY8[0] = (BYTE)((6416*pRGB[0] + 33039*pRGB[1] + 16829*pRGB[2] + 0x108000) >> 16);

			pY8  += 1;
			pRGB += 4;
		}
	}

	return TRUE;
}

BOOL CY800VFW::CompressYUY2(LPBITMAPINFOHEADER lpbiIn, const void* pSrc, void* pDst)
{
	const BYTE *pYUY2;
	BYTE *pY8;

	long lCol, lRow;
	long lWidthY8 = (lpbiIn->biWidth + 3L) & (~3L);

	for(lRow = 0; lRow < labs(lpbiIn->biHeight); lRow++)
	{
		pY8 = reinterpret_cast<BYTE*>(pDst) + lWidthY8 * lRow;
		pYUY2 = reinterpret_cast<const BYTE*>(pSrc) + 2 * lpbiIn->biWidth * lRow;

		for(lCol = 0; lCol < lpbiIn->biWidth; lCol++)
		{
			pY8[0] = pYUY2[0];

			pY8 += 1;
			pYUY2 += 2;
		}
	}

	return TRUE;
}

BOOL CY800VFW::DecompressRGB24(LPBITMAPINFOHEADER lpbiOutput, const void* pSrc, void* pDst)
{
	BYTE *pRGB;
	const BYTE *pY8;

	long lCol, lRow;
	long lWidthY8 = (lpbiOutput->biWidth + 3L) & (~3L);
	long lWidthRGB = (3L*lpbiOutput->biWidth + 3L) & (~3L);

	for(lRow = 0; lRow < labs(lpbiOutput->biHeight); lRow++)
	{
		pY8 = reinterpret_cast<const BYTE*>(pSrc) + lWidthY8 * lRow;

		if(lpbiOutput->biHeight < 0)
			pRGB = reinterpret_cast<BYTE*>(pDst) + lWidthRGB * lRow;
		else
			pRGB = reinterpret_cast<BYTE*>(pDst) + lWidthRGB * (lpbiOutput->biHeight - 1 - lRow);

		for(lCol = 0; lCol < lpbiOutput->biWidth; lCol++)
		{
			pRGB[0] = pY8[0];
			pRGB[1] = pY8[0];
			pRGB[2] = pY8[0];

			pY8  += 1;
			pRGB += 3;
		}
	}

	return TRUE;
}

BOOL CY800VFW::DecompressRGB32(LPBITMAPINFOHEADER lpbiOutput, const void* pSrc, void* pDst)
{
	BYTE *pRGB;
	const BYTE *pY8;

	long lCol, lRow;
	long lWidthY8 = (lpbiOutput->biWidth + 3L) & (~3L);

	for(lRow = 0; lRow < labs(lpbiOutput->biHeight); lRow++)
	{
		pY8 = reinterpret_cast<const BYTE*>(pSrc) + lWidthY8 * lRow;

		if(lpbiOutput->biHeight < 0)
			pRGB = reinterpret_cast<BYTE*>(pDst) + 4 * lpbiOutput->biWidth * lRow;
		else
			pRGB = reinterpret_cast<BYTE*>(pDst) + 4 * lpbiOutput->biWidth * (lpbiOutput->biHeight - 1 - lRow);

		for(lCol = 0; lCol < lpbiOutput->biWidth; lCol++)
		{
			pRGB[0] = pY8[0];
			pRGB[1] = pY8[0];
			pRGB[2] = pY8[0];
			pRGB[3] = 0xFF;

			pY8  += 1;
			pRGB += 4;
		}
	}

	return TRUE;
}

BOOL CY800VFW::DecompressYUY2(LPBITMAPINFOHEADER lpbiOutput, const void* pSrc, void* pDst)
{
	BYTE *pYUY2;
	const BYTE *pY8;

	long lCol, lRow;
	long lWidthY8 = (lpbiOutput->biWidth + 3L) & (~3L);

	for(lRow = 0; lRow < labs(lpbiOutput->biHeight); lRow++)
	{
		pY8 = reinterpret_cast<const BYTE*>(pSrc) + lWidthY8 * lRow;
		pYUY2 = reinterpret_cast<BYTE*>(pDst) + 2 * lpbiOutput->biWidth * lRow;

		for(lCol = 0; lCol < lpbiOutput->biWidth; lCol++)
		{
			pYUY2[0] = pY8[0];
			pYUY2[1] = 0x80;

			pY8 += 1;
			pYUY2 += 2;
		}
	}

	return TRUE;
}