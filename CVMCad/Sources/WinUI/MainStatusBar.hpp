/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: MainStatusBar.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-05
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __MAINSTATUSBAR_HPP__
#define __MAINSTATUSBAR_HPP__

class CMainStatusBar: public CStatusBar
{
public:
	CMainStatusBar();
	virtual ~CMainStatusBar();

public:
	BOOL Create( CWnd* pParent );
	CSize GetDefSize();

protected:

private:
	CSize m_DefSize;
};



#endif //__MAINSTATUSBAR_HPP__
