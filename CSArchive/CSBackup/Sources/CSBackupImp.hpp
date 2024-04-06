/*
*	CSBackup 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: CSBackupImp.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-07-29
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __CSBACKUPIMP_HPP__
#define __CSBACKUPIMP_HPP__

struct ICSBackUp;
struct IDebugOutput;

class CSBackupImp : public ICSBackUp
{
public:
	//Default waiting time for the stop event
	static const int m_nOuterLoopWaitingTime = 15000;
	static const int m_nInnerLoopWaitingTime = 0;
public:
	CSBackupImp( IDebugOutput* pDebug );
	virtual ~CSBackupImp();
	HRESULT Release();
public:
	void Cleanup();
	IDebugOutput* GetDebugOutput(){ return m_pDebug; }
	// 2005-08-11 E. Gorbachev enchancement, add dump protected function
	static DWORD WINAPI ThreadProc_Handled( void* pParam );
	static DWORD WINAPI ThreadProc( void* pParam );
private:
	DWORD m_dwThreadId;
	HANDLE m_hThread;
	HANDLE m_nThreadStartedEvent;
	HANDLE m_hStoppingEvent;
	IDebugOutput* m_pDebug;
};


#endif //__CSBACKUPIMP_HPP__
