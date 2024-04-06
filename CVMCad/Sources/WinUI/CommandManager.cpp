/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CommandManager.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-19
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Commands.hpp"
#include "CommandManager.hpp"


CCommandManager* CCommandManager::m_pSelf = 0;
CSingletonDestroyer<CCommandManager> CCommandManager::m_destroyer;

CCommandManager::CCommandManager()
{


}

CCommandManager::~CCommandManager()
{
	CmdMapIt it;
	CmdMapIt begin_it = m_CommandMap.begin();
	CmdMapIt end_it = m_CommandMap.end();

	for( it = begin_it; it != end_it; ++it )
	{
		delete (CCommand*)(it -> second);
	}

	m_CommandMap.clear();
}

CCommandManager* CCommandManager::GetInstance()
{
	if( m_pSelf == 0 )
	{
		m_pSelf = new CCommandManager();
		m_destroyer.SetDestroyable( m_pSelf );
	}
	return m_pSelf;
}

//////////////////////////////////////////////////////////////////////////
void CCommandManager::UpdateCommands()
{
	CmdMapIt it;
	CmdMapIt begin_it = m_CommandMap.begin();
	CmdMapIt end_it = m_CommandMap.end();
    
	for( it = begin_it; it != end_it; ++it )
	{
		(it -> second)->ProcessUpdate();
	}
}

bool CCommandManager::ProcessCommand( uint nID )
{
	CmdMapIt it = m_CommandMap.find( nID );

	if( it != m_CommandMap.end() )
	{
		//Invoke command with specified ID
		(it -> second)->Execute();
		return true;
	}

	return false;
}

void CCommandManager::RegisterCommand( CCommand* pCommand )
{
	ASSERT( pCommand != NULL );
	m_CommandMap.insert( std::make_pair( pCommand->GetId(), pCommand ) );
}
//////////////////////////////////////////////////////////////////////////
void CCommandManager::AddCommandExecutor( uint nID, ICommandExecutor* pCommandExecutor )
{
	CmdMapIt it = m_CommandMap.find( nID );

	if( it != m_CommandMap.end() )
	{
		//Invoke command with specified ID
		(it -> second) -> AddExecutor( pCommandExecutor );
	}

}

void CCommandManager::RemoveCommandExecutor( uint nID, ICommandExecutor* pCommandExecutor )
{
	CmdMapIt it = m_CommandMap.find( nID );

	if( it != m_CommandMap.end() )
	{
		//Invoke command with specified ID
		(it -> second) -> RemoveExecutor( pCommandExecutor );
	}
}