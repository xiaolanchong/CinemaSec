/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CommandManager.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-19
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: CCommandManager handles all command processing in the app
*
*
*/
#pragma once
#ifndef __COMMANDMANAGER_HPP__
#define __COMMANDMANAGER_HPP__

class CCommand;
class ICommandExecutor;

typedef std::map< uint, CCommand* > CmdMap;
typedef std::map< uint, CCommand* >::iterator CmdMapIt;

//typedef std::stack< CCommand* > CmdStack;
//typedef std::stack< CCommand* >::iterator CmdStackIt;


class CCommandManager
{
	friend class CSingletonDestroyer;

public:
	static CCommandManager* GetInstance();

public:
	void UpdateCommands();
	bool ProcessCommand( uint nID );

	void AddCommandExecutor( uint nID, ICommandExecutor* pCommandExecutor );
	void RemoveCommandExecutor( uint nID, ICommandExecutor* pCommandExecutor );
	void RegisterCommand( CCommand* pCommand );

public:
	//void UndoLastCommand();
	//void RedoLastCommand();
	//void PurgeHistory();

protected:
    CCommandManager();
	virtual ~CCommandManager();

private:
	//CmdMap is the bridge between MFC's nID and concrete CCommand instance
	CmdMap m_CommandMap;
	
private:
	static CCommandManager* m_pSelf;
	static CSingletonDestroyer<CCommandManager> m_destroyer;
};


#endif //__COMMANDMANAGER_HPP__
