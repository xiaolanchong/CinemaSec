// drawstream.cpp

#include "chcs.h"

//////////////////////////////////////////////////////////////////////
// Namespace: CHCS

namespace CHCS {

//////////////////////////////////////////////////////////////////////////
// CStreamDrawThread
//////////////////////////////////////////////////////////////////////////

class CStreamDrawThread : public Elvees::CThread
{
public:
	explicit CStreamDrawThread(IStream* pStream, HWND hDrawWnd);
	virtual ~CStreamDrawThread();

	using Elvees::CThread::Start;

	void Shutdown();

protected:
	virtual int Run();

private:
	IStream* m_pStream;
	HWND     m_hDrawWnd;

	Elvees::CManualResetEvent m_shutdownEvent;

	// No copies do not implement
	CStreamDrawThread(const CStreamDrawThread &rhs);
	CStreamDrawThread &operator=(const CStreamDrawThread &rhs);
};

CStreamDrawThread::CStreamDrawThread(IStream* pStream, HWND hDrawWnd)
	: m_pStream(pStream)
	, m_hDrawWnd(hDrawWnd)
{
	if(m_pStream)
		m_pStream->AddRef();
}

CStreamDrawThread::~CStreamDrawThread()
{
	if(m_pStream)
		m_pStream->Release();
}

int CStreamDrawThread::Run()
{
	HDC hDC;
	RECT rectClient;

	DWORD dwRet;
	IFrame *pFrame;

	while(1)
	{
		dwRet = WaitForSingleObject(m_shutdownEvent.GetEvent(), 500);

		if(dwRet == WAIT_OBJECT_0) // Shutdown event
		{
			break;
		}
		else if(dwRet == WAIT_TIMEOUT)
		{
			if(!IsWindow(m_hDrawWnd))
				break;

			if(m_pStream->GetNextFrame(&pFrame, 200))
			{
				hDC = ::GetDC(m_hDrawWnd);
				::GetClientRect(m_hDrawWnd, &rectClient);

				::SetStretchBltMode(hDC, STRETCH_HALFTONE);
				::SetBkMode(hDC, TRANSPARENT);
				::SetTextColor(hDC, 0xFF8000);

				if(!pFrame->Draw(hDC, 0, 0, rectClient.right, rectClient.bottom))
				{
					::FillRect(hDC, &rectClient, (HBRUSH) (COLOR_WINDOW+1));

					LPCTSTR stNoDraw =  TEXT("Draw failed");
					::TextOut(hDC, 5, 5, stNoDraw, lstrlen(stNoDraw));
				}

				::ReleaseDC(m_hDrawWnd, hDC);

				pFrame->Release();
			}
		}
		else
		{
			break;
		}
	}

	return 0;
}

void CStreamDrawThread::Shutdown()
{
	m_shutdownEvent.Set();
	Wait();
}

//////////////////////////////////////////////////////////////////////////
// Set/Kill draw thread
//////////////////////////////////////////////////////////////////////////

CHCSVA_API DWORD __stdcall SetStreamDraw(LPCSTR stStream, HWND hWndOwner)
{
	if(!stStream || !hWndOwner)
		return false;

	UUID StreamUID;
	IStream* pStream = NULL;

#ifdef _UNICODE
	WCHAR szStream[56];

	MultiByteToWideChar(CP_ACP, 0, stStream, (int)strlen(stStream) + 1,
		szStream, countof(szStream));

	if(StringToUuid(szStream, &StreamUID))
#else
	if(StringToUuid(const_cast<LPTSTR>(stStream), &StreamUID))
#endif
	{
		if(GetStreamByUID(&pStream, &StreamUID))
		{
			CStreamDrawThread* pDraw = new CStreamDrawThread(pStream, hWndOwner);

			// Stream does not need anymore..
			pStream->Release();
			
			if(pDraw)
			{
				pDraw->Start();
				return (DWORD)(LONG_PTR)pDraw;
			}
		}
	}

	return 0;
}

CHCSVA_API void __stdcall KillStreamDraw(DWORD dwDraw)
{
	CStreamDrawThread* pDraw = (CStreamDrawThread*)(LONG_PTR)(dwDraw);

	if(pDraw)
	{
		pDraw->Shutdown();
		delete pDraw;
	}
}

} // End of namespace CHCS