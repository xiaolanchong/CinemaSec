/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: MainToolbar.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-04-27
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __MAINTOOLBAR_HPP__
#define __MAINTOOLBAR_HPP__


class CMainToolBar : public CReBar, public ICommandExecutor
{
public:
	CMainToolBar();
	virtual ~CMainToolBar();

public:
	BOOL Create( CWnd* pParent );
	void Enable( uint nID, bool bEnable );
	void Check( uint nID, bool bCheck );


private:
	CImageList m_TBImageListEnabled;
	CImageList m_TBImageListDisabled;

	CToolBarCtrl m_wndTB;

};



#endif //__MAINTOOLBAR_HPP__
