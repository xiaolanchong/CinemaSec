/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Application.hpp
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
#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

class CApplication : public CWinApp
{

public:
	//
	CApplication();
	virtual ~CApplication();

	//Overrides
	BOOL InitInstance();
	BOOL OnIdle( LONG lCount );
	void RegisterCommands();


protected:

private:
	CMainWindow* m_pMainWindow;
	ULONG_PTR gdiplusToken;

};



#endif //__APPLICATION_HPP__
