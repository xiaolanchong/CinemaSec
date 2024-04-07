////////////////////////////////////////////////////////////////
// PixieLib(TM) Copyright 1997-2005 Paul DiLascia
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio.NET 7.1 or greater. Set tabsize=3.
// 
// NOTE: PixieLib is NOT freeware! 
// If you didn't pay for your copy, you're violating my copyright!
//
#include "stdafx.h"
#include "FindWinList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////// CWinList ////////////////////////////////

//////////////////
// Create window list: enumerate to fill STL vector of HWNDs.
//
CWinList::CWinList(HWND hwndParent, UINT nReserve) : m_hwndParent(hwndParent)
{
	reserve(nReserve);
}

//////////////////
// Create window list: enumerate to fill STL vector of HWNDs.
//
UINT CWinList::Fill()
{
	if (m_hwndParent) {
		::EnumChildWindows(m_hwndParent, EnumProc, (LPARAM)this);
	} else {
		::EnumWindows(EnumProc, (LPARAM)this);
	}
	return size();
}

//////////////////
// Enumerator fn calls virtual OnWindow method, which tells whether to include
// window in list.
//
BOOL CALLBACK CWinList::EnumProc(HWND hwnd, LPARAM lp)
{
	CWinList* pwl = (CWinList*)lp;
	if (pwl->OnWindow(hwnd)) {
		pwl->push_back(hwnd);
	}
	return TRUE; // keep iterating
}

//////////////////////////////// CFindWinList ////////////////////////////////

//////////////////
// Filter for windows with matching class name.
//
BOOL CFindWinList::OnWindow(HWND hwnd)
{
	TCHAR classname[_MAX_PATH];
	GetClassName(hwnd, classname, sizeof(classname)/sizeof(TCHAR));
	return _tcscmp(classname, m_lpClassName)==0;
}

