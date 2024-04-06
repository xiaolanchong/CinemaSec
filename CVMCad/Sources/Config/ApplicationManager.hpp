/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ApplicationManager.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-09
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __APPLICATIONMANAGER_HPP__
#define __APPLICATIONMANAGER_HPP__

using namespace std;


class IBaseView;
class ICommand;
class CScene;
class CGeometryView;
class CMainMenu;
class CPropertiesView;

class CApplicationManager
{
	friend class CSingletonDestroyer;
public:
	static CApplicationManager* GetInstance();
public:
	enum ToolType
	{
		SelectTool = 0,
		PanTool = 1,
		ZoomTool = 2,
		MoveTool = 3,
		RotateTool = 4,
		ScaleTool = 5
	};

//Operations with Doc&Viewers
public:
	void AddViewer( IBaseView* pViewer );
	void RemoveViewer( IBaseView* pViewer );
	void NotifyViewers();
	
	void SetActiveScene( CScene* pDocument );
	CScene* GetActiveScene();

public:
	CApplication*	GetApplication();
	CMainWindow*	GetMainWindow();
	CMainStatusBar* GetMainStatusBar();
	CMainToolBar*	GetMainToolbar();
	CMainMenu*		GetMainMenu();
	CGeometryView*	GetGeometryView();
	CPropertiesView* GetPropertiesView();

protected:
	CApplicationManager();
	virtual ~CApplicationManager();


private:
	static CApplicationManager* m_pSelf;
	static CSingletonDestroyer<CApplicationManager> m_destroyer;

	vector<IBaseView*>		m_Listeners;
	stack<ICommand*>	m_CommandHistory;
	CScene*	m_pActiveScene;
};

#endif //__APPLICATIONMANAGER_HPP__
