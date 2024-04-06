#pragma once

//#include <wstring>

class LongOperation
{
	enum
	{
		MAX_DESCRIPTION_LENGTH = 256
	};
	static DWORD	WINAPI  ThreadProc(void*);
	
	RECT					m_rcMainWnd;
	HANDLE					m_hSyncEvent;
	HANDLE					m_hThread;
	HWND					m_hAnimWnd;
	LONG					m_lTimer;

	void	Start( HWND hMainWnd );
	void	Stop();
	HWND	Create(const RECT& rc);
	static LongOperation* GetThis( HWND hWnd );

	UINT					m_nIDDescription;
	UINT					m_nIDAnimation;
	WCHAR					m_szDescription[ MAX_DESCRIPTION_LENGTH ];
	HINSTANCE				m_hResInst;

	static	LRESULT	WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam  );
public:
	LongOperation(HWND hMainWnd, HINSTANCE hResInst, UINT nIDDescription, UINT nIDAnimation);
	~LongOperation(void);


};
