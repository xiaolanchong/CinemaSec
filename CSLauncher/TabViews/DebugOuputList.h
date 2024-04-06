//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	IDebugOutput implementation for OXTreeCtrl list log, 
//  console-like log but looks much better
// Using:
// 1) define list dialog resource 
// 2) implement DebugOutputListCB
// 3) inherit or include into class DebugOutputList
// 4) call DebugOutputList::Attach
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 20.03.2005
//                                                                                      //
//======================================================================================//
#ifndef _DEBUG_OUPUT_LIST_4592332876835992_
#define _DEBUG_OUPUT_LIST_4592332876835992_

#include <afxmt.h>
#include <list>
#include <mmsystem.h>
#include "../../CSUtility/utility/debug_interface.h"
#include "../UT/OXTreeCtrl.h"
#include "../ut/OXHookWnd.h"

#define WM_DBGMESSAGE WM_USER + 0x0F
//======================================================================================//
//                                 struct DebugOuputListCB                              //
//======================================================================================//
struct DebugOutputListCB
{
	virtual bool	OnReceiveMessage( int nSeverity, LPCWSTR szMessage ) = 0;
	virtual int		GetMaxLineNumber() = 0;
};

//======================================================================================//
//                                 class DebugOuputList                                 //
//======================================================================================//
class DebugOuputList : public IDebugOutput
{
	friend class DebugOuputListHook;
	class DebugOuputListHook : public COXHookWnd	
	{
		DebugOuputList* m_pParent;
		LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp)
		{
			if( msg == WM_DBGMESSAGE )
			{
				m_pParent->OnDebugMessage();
			}
			return Default();
		}
	public:
		DebugOuputListHook( DebugOuputList* p ) : m_pParent(p){} 
	} m_Hook;
	
	void	OnDebugMessage();
protected:
	void	PrintA( __int32 mt, LPCSTR szMessage);
	void	PrintW( __int32 mt, LPCWSTR szMessage);
private:
	CCriticalSection cs;
	typedef std::list< std::pair< unsigned int, std::wstring> > MsgBuffer_t ;
	MsgBuffer_t m_MsgBuffer;

	DebugOutputListCB*	m_pCB;
	CWnd*				m_pWnd;
protected:
	COXTreeCtrl			m_wndDebugMessage;
public:
	DebugOuputList();
	void	Attach( UINT nListID, CWnd* pWnd, DebugOutputListCB* pCB); 
	virtual ~DebugOuputList();
};

#endif // _DEBUG_OUPUT_LIST_4592332876835992_