/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ApplicationConfig.hpp
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
#ifndef __APPLICATIONCONFIG_HPP__
#define __APPLICATIONCONFIG_HPP__

using namespace std;
using namespace boost;

class CApplication;
class CMainWindow;
class CMainStatusBar;
class CMainToolBar;

class CApplicationConfig
{
	friend class CSingletonDestroyer<CApplicationConfig>;

public:
	static CApplicationConfig* GetInstance();

public:
	void LoadConfig( const wstring& sFileName );
	void SaveConfig( const wstring& sFileName );

//Methods
public:
	void GetAppTitle( wstring& s );
	void GetAppName( wstring& s );
	void GetDefFileName( wstring& s );


protected:
	CApplicationConfig();
	virtual ~CApplicationConfig();

private:
	static CApplicationConfig* m_pSelf;
	static CSingletonDestroyer<CApplicationConfig> m_destroyer;

private:
	wstring m_sAppName;
	wstring m_sAppTitle;
	wstring m_sDefFileName;

};

#endif //__APPLICATIONCONFIG_HPP__
