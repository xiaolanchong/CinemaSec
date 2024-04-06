//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		Add code for tooltips and message as CFrameWnd normally does. Use it for non-CFrameWnd parent windows
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   26.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __CONTROL_HELPER_H_INCLUDED_7302780804326437__
#define __CONTROL_HELPER_H_INCLUDED_7302780804326437__

#include "../gui/Subclass.h"

//=====================================================================================//
//                                 class ControlHelper                                 //
//=====================================================================================//
class ControlHelper :  protected CSubclassWnd
{
protected:
	BOOL Attach( CWnd* pWnd);

	virtual LRESULT WindowProc(UINT msg, WPARAM wParam, LPARAM lParam);

	virtual LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT	OnToolTipText(WPARAM wParam, LPARAM lParam) = 0;

	ControlHelper();
	virtual ~ControlHelper();
};

class ControlHelperWithFrame : protected ControlHelper
{
	CFrameWnd*	m_pFrameWnd;
public:
	ControlHelperWithFrame() : m_pFrameWnd(0){}
	void Attach( CWnd* pWnd, CFrameWnd* pFrameWnd );
	virtual LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam) ;
	virtual LRESULT	OnToolTipText(WPARAM wParam, LPARAM lParam);
};

#endif //__CONTROL_HELPER_H_INCLUDED_7302780804326437__
