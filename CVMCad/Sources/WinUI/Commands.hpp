/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Commands.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-15
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: Command's hive
*
*
*/
//#pragma once
#ifndef __COMMANDS_HPP__
#define __COMMANDS_HPP__

class CCamera;
class CElement;
class ICommandExecutor;

typedef std::list<ICommandExecutor*> CCmdExecsPtrList;
typedef std::list<ICommandExecutor*>::iterator CCmdExecsPtrListIt;


class CCommand
{
public:
	CCommand(){};
	virtual ~CCommand(){};

public:
	virtual void Execute() = 0;
	virtual void Unexecute() = 0;

	virtual void ProcessUpdate() = 0;
	virtual uint GetId();

	virtual void AddExecutor( ICommandExecutor* pExecutor );
	virtual void RemoveExecutor( ICommandExecutor* pExecutor );

protected:
	CCmdExecsPtrList m_CmdExecsList;
	uint m_nId;
	bool m_bEnabled;
	bool m_bChecked;
	bool m_bReversible;
};

//////////////////////////////////////////////////////////////////////////
class CFileNewCommand : public CCommand
{
public:
	CFileNewCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};

//////////////////////////////////////////////////////////////////////////
class CFileCloseCommand : public CCommand
{
public:
	CFileCloseCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};

//////////////////////////////////////////////////////////////////////////
class CFileOpenCommand : public CCommand
{
public:
	CFileOpenCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};

//////////////////////////////////////////////////////////////////////////
class CFileSaveCommand : public CCommand
{
public:
	CFileSaveCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};

//////////////////////////////////////////////////////////////////////////
class CFileExitCommand : public CCommand
{
public:
	CFileExitCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};

//////////////////////////////////////////////////////////////////////////
class CAddCameraCommand : public CCommand
{
public:
	CAddCameraCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};

//////////////////////////////////////////////////////////////////////////
class CRemoveCameraCommand : public CCommand
{
public:
	CRemoveCameraCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};
//////////////////////////////////////////////////////////////////////////
class CAddElementCommand : public CCommand
{
public:
	CAddElementCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};

//////////////////////////////////////////////////////////////////////////
class CRemoveElementCommand : public CCommand
{
public:
	CRemoveElementCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};

//////////////////////////////////////////////////////////////////////////
class CMakeActiveElementCommand : public CCommand
{
public:
	CMakeActiveElementCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};

//////////////////////////////////////////////////////////////////////////
class CMakeActiveCameraCommand : public CCommand
{
public:
	CMakeActiveCameraCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};


//////////////////////////////////////////////////////////////////////////
class CAddPointCommand : public CCommand
{
public:
	CAddPointCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};

//////////////////////////////////////////////////////////////////////////
class CRemovePointsCommand : public CCommand
{
public:
	CRemovePointsCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};
//////////////////////////////////////////////////////////////////////////
class CTriangulateCommand : public CCommand
{
public:
	CTriangulateCommand();
public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};

//////////////////////////////////////////////////////////////////////////
class CApplyNormalMappingCommand : public CCommand
{
public:
	CApplyNormalMappingCommand();
public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};


//////////////////////////////////////////////////////////////////////////
class CSelectCameraSourceCommand : public CCommand
{
public:
	CSelectCameraSourceCommand();
public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};



//////////////////////////////////////////////////////////////////////////
class CGVSelectTool : public CCommand
{
public:
	CGVSelectTool();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};
//////////////////////////////////////////////////////////////////////////

class CGVPanTool : public CCommand
{
public:
	CGVPanTool();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};
//////////////////////////////////////////////////////////////////////////

class CGVZoomTool : public CCommand
{
public:
	CGVZoomTool();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};
//////////////////////////////////////////////////////////////////////////

class CGVHorizontalAlignCommand : public CCommand
{
public:
	CGVHorizontalAlignCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};
//////////////////////////////////////////////////////////////////////////

class CGVVerticalAlignCommand : public CCommand
{
public:
	CGVVerticalAlignCommand();

public:
	void Execute();
	void Unexecute();
	void ProcessUpdate();
};
//////////////////////////////////////////////////////////////////////////



#endif //__COMMANDS_HPP__
