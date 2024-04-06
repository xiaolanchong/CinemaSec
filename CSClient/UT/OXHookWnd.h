#ifndef _HOOKWND_H
#define _HOOKWND_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


#ifndef HWM_APP
#define HWM_APP						WM_APP
#endif	// HWM_APP

// sent to hooked window when mouse is over this window for first time (entered it)
#define HWM_MOUSEENTER				HWM_APP+1
// sent to hooked window when mouse has left this window
#define HWM_MOUSELEAVE				HWM_APP+2

// timer ID event. Every time when it happens we call CheckMousePos function to 
// check whether mouse id over the hooked window or not
#define ID_CHECKMOUSETIMER			1
// default delay
#ifndef DFLT_CHECKMOUSEDELAY
#define DFLT_CHECKMOUSEDELAY		100
#endif	// DFLT_CHECKMOUSEDELAY

//////////////////
// Generic class to hook messages on behalf of a CWnd.
// Once hooked, all messages go to COXHookWnd::WindowProc before going
// to the window. Specific subclasses can trap messages and do something.
// To use:
//
// * Derive a class from COXHookWnd.
//
// * Override COXHookWnd::WindowProc to handle messages. Make sure you call
//   COXHookWnd::WindowProc if you don't handle the message, or your window will
//   never get messages. If you write separate message handlers, you can call
//   Default() to pass the message to the window.
//
// * Instantiate your derived class somewhere and call HookWindow(pWnd)
//   to hook your window, AFTER it has been created.
//	 To unhook, call UnhookWindow().
//
class OX_CLASS_DECL COXHookWnd : public CObject 
{
protected:
	DECLARE_DYNAMIC(COXHookWnd);

	// the window hooked
	CWnd* m_pWndHooked;	
	// original window proc
	// we call it to handle all unhandled messages
	WNDPROC	m_pOldWndProc;		
	// This class is generic so we cannot rule out the situation
	// when more than one object of COXHookWnd derived class hooked 
	// the same window. We have to make sure that all these objects 
	// eventually will get chance to handle messages that wasn't 
	// handled by the other objects that hooked the window later.
	// Thus always call COXHookWnd::WindowProc in your WindowProc on
	// all messages you don't handle
	//
	// next in chain of hooks for this window
	COXHookWnd*	m_pNext;		

	UINT m_nCheckMouseTimerID;
	BOOL m_bMouseIsOver;

	// Override this to handle messages in specific handlers
	virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);
	// call this at the end of handler functions
	LRESULT Default();				

	// check if mouse is over the hooked window or not and notifies the hooked
	// window if the state has changed (HWM_ENTERWINDOW and HWM_LEAVEWINDOW messages 
	// will be sent)
	void CheckMousePos();

public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXHookWnd();

	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destroys the object
	~COXHookWnd();


	// --- In  :	pRealWnd	-	pointer to created window that is 
	//								supposed to be hooked
	// --- Out : 
	// --- Returns:
	// --- Effect : Hook a window
	void HookWindow(CWnd* pRealWnd);

	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Unhook a window. Automatic on WM_NCDESTROY
	void UnhookWindow();

	// --- In  :
	// --- Out : 
	// --- Returns:	TRUE if any window was hooked
	// --- Effect : 
	inline BOOL IsHooked() const { 
		return m_pWndHooked!=NULL; }

	
	// --- In  :
	// --- Out : 
	// --- Returns:	pointer to hooked window
	// --- Effect : 
	inline CWnd* GetHookedWnd() { return m_pWndHooked; }


	// --- In  :
	// --- Out : 
	// --- Returns:	TRUE if success, or FALSE otherwise
	// --- Effect : installs spy timer that will notify the hooked window when mouse
	//				enter and leave the window
	BOOL InstallSpy();

	// --- In  :
	// --- Out : 
	// --- Returns:	
	// --- Effect : removes spy timer that notifies the hooked window when mouse
	//				enter and leave the window(HWM_ENTERWINDOW and HWM_LEAVEWINDOW 
	//				messages will be sent)
	void RemoveSpy();


	// --- In  :
	// --- Out : 
	// --- Returns:	pointer to hooked window
	// --- Effect : 
	inline BOOL IsMouseOver() const { return m_bMouseIsOver; }


	friend LRESULT CALLBACK HookWndProc(HWND, UINT, WPARAM, LPARAM);
	friend class COXHookWndMap;
};

//////////////////
// The message hook map is derived from CMapPtrToPtr, which associates
// a pointer with another pointer. It maps an HWND to a COXHookWnd, like
// the way MFC's internal maps map HWND's to CWnd's. The first hook
// attached to a window is stored in the map; all other hooks for that
// window are then chained via COXHookWnd::m_pNext.
//
class OX_CLASS_DECL COXHookWndMap : private CMapPtrToPtr 
{
public:
	COXHookWndMap() {} ; 
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	~COXHookWndMap() ;
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destroys the object
	static COXHookWndMap& GetHookMap();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Trick to instantiate object only when it is really needed
	void Add(HWND hwnd, COXHookWnd* pMsgHook);
	// --- In  :	hwnd	-	handle of window
	//				pMsgHook-	pointer to COXHookWnd object
	// --- Out : 
	// --- Returns:
	// --- Effect : adds a pair of window and its hook into the map
	void Remove(COXHookWnd* pMsgHook);
	// --- In  :	pMsgHook-	pointer to COXHookWnd object
	// --- Out : 
	// --- Returns:
	// --- Effect : removes specified hook from the map (remember that one COXHookWnd
	//				object could be used to hook only one CWnd object)
	void RemoveAll(HWND hwnd);
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : cleans up the map
	COXHookWnd* Lookup(HWND hwnd);
	// --- In  :	hwnd	-	handle of window
	// --- Out : 
	// --- Returns:	pointer to corresponding COXHookWnd object,
	//				or NULL if it doesn't exist
	// --- Effect : looks for COXHookWnd object used to hook hwnd window
};

#endif // #ifndef _HOOKWND_H
