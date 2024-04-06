/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ApplicationManager.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-09
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Configuration.hpp"
#include "Model.hpp"
#include "Graphics.hpp"
#include "WinUI.hpp"
#include "ApplicationManager.hpp"

CApplicationManager* CApplicationManager::m_pSelf = 0;
CSingletonDestroyer<CApplicationManager> CApplicationManager::m_destroyer;

CApplicationManager::CApplicationManager()
{
	m_pActiveScene = NULL;
}

CApplicationManager::~CApplicationManager()
{
	if( m_pActiveScene != NULL ){
		delete m_pActiveScene;
		m_pActiveScene = NULL;
	}

}

CApplicationManager* CApplicationManager::GetInstance()
{
	if( m_pSelf == 0 )
	{
		m_pSelf = new CApplicationManager();
		m_destroyer.SetDestroyable( m_pSelf );
	}
	return m_pSelf;
}

//////////////////////////////////////////////////////////////////////////
CApplication* CApplicationManager::GetApplication()
{
	return reinterpret_cast<CApplication*>(AfxGetApp());
}

CMainWindow* CApplicationManager::GetMainWindow()
{
	return reinterpret_cast<CMainWindow*>(GetApplication()->m_pMainWnd);
}

CMainStatusBar* CApplicationManager::GetMainStatusBar()
{
	return GetMainWindow()->GetMainStatusBar();
}

CMainToolBar* CApplicationManager::GetMainToolbar()
{
	return GetMainWindow()->GetMainToolbar();
}

CMainMenu* CApplicationManager::GetMainMenu()
{
	return GetMainWindow()->GetMainMenu();
}

CGeometryView* CApplicationManager::GetGeometryView()
{
	return GetMainWindow()->GetGeometryView();
}

CPropertiesView* CApplicationManager::GetPropertiesView()
{
	return GetMainWindow()->GetPropertiesView();
}
//////////////////////////////////////////////////////////////////////////
void CApplicationManager::AddViewer( IBaseView* pViewer )
{
	m_Listeners.push_back(pViewer);
}
void CApplicationManager::RemoveViewer( IBaseView* pViewer )
{
	vector<IBaseView*>::iterator i = 
		find( m_Listeners.begin(), m_Listeners.end(), pViewer );

	if( i != m_Listeners.end())
	{
		(*i) = NULL;
		m_Listeners.erase(i);

	}
}

void CApplicationManager::NotifyViewers( )
{
	for( vector<IBaseView*>::iterator i = m_Listeners.begin(); i != m_Listeners.end(); ++i )
	{
		if( (*i) != NULL )
		{
			(*i)->OnUpdate();
		}
	}
}



void CApplicationManager::SetActiveScene( CScene* pDocument )
{
	m_pActiveScene = pDocument;
}

 CScene* CApplicationManager::GetActiveScene()
{
	return m_pActiveScene;
}


