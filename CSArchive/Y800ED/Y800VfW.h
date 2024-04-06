// Y800VfW.h: interface for the CY800VFW class.
//
//////////////////////////////////////////////////////////////////////

#ifndef Y800VFW_H__INCLUDED_
#define Y800VFW_H__INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#include "bzlib2.h"

//////////////////////////////////////////////////////////////////////
// CY800VFW

class CY800VFW
{
private:
	DWORD m_dwFCCUsed;
	
	BOOL m_bDebugInfo;
	BOOL m_bDiscardData;

private:	
	/*explicit*/ CY800VFW();
	virtual ~CY800VFW();

public:
	static CY800VFW* __stdcall Open(ICOPEN* icInfo);
	static LRESULT __stdcall Close(CY800VFW* pInst);

	BOOL QueryAbout();
	DWORD About(HWND hwnd);

	BOOL QueryConfigure();
	DWORD Configure(HWND hwnd);

	DWORD GetState(LPVOID pv, DWORD dwSize);
	DWORD SetState(LPVOID pv, DWORD dwSize);

	DWORD GetInfo(ICINFO* icInfo, DWORD dwSize);

	DWORD CompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD CompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD CompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD CompressGetSize(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD Compress(ICCOMPRESS* icinfo, DWORD dwSize);
	DWORD CompressEnd();

	DWORD DecompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD DecompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD DecompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD Decompress(ICDECOMPRESS* icinfo, DWORD dwSize);
	DWORD DecompressGetPalette(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD DecompressEnd();

	/*
	DWORD DrawQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
	DWORD DrawBegin(ICDRAWBEGIN* icinfo, DWORD dwSize);
	DWORD Draw(ICDRAW* icinfo, DWORD dwSize);
	DWORD DrawEnd();
	DWORD DrawWindow(PRECT prc);
	*/

protected:
	static BOOL CALLBACK ProcAbout(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK ProcConfigure(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Using dynamic linkage
	bool InitBZLib();

	BOOL IsY800Format(const LPBITMAPINFOHEADER lpbiOut);
	BOOL IsSupportedFormat(const LPBITMAPINFOHEADER lpbiIn);

	BOOL CompressRGB24(LPBITMAPINFOHEADER lpbiIn, const void* pSrc, void* pDst);
	BOOL CompressRGB32(LPBITMAPINFOHEADER lpbiIn, const void* pSrc, void* pDst);
	BOOL CompressYUY2(LPBITMAPINFOHEADER lpbiIn,  const void* pSrc, void* pDst);

	BOOL DecompressRGB24(LPBITMAPINFOHEADER lpbiOutput, const void* pSrc, void* pDst);
	BOOL DecompressRGB32(LPBITMAPINFOHEADER lpbiOutput, const void* pSrc, void* pDst);
	BOOL DecompressYUY2(LPBITMAPINFOHEADER  lpbiOutput, const void* pSrc, void* pDst);

private:
	// Using libbz2.dll
	HMODULE m_hBZLib;

	fBZ2_bzBuffToBuffCompress   BZ2_bzBuffToBuffCompress;
	fBZ2_bzBuffToBuffDecompress BZ2_bzBuffToBuffDecompress;

	void* m_pCFrame;
	void* m_pDFrame;

	// No copies do not implement
	CY800VFW(const CY800VFW &rhs);
	CY800VFW &operator=(const CY800VFW &rhs);
};

#endif // Y800VFW_H__INCLUDED_
