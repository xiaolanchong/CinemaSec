//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		CSBackup client code
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   28.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __BACKUP_SERVER_H_INCLUDED_4705331154283507__
#define __BACKUP_SERVER_H_INCLUDED_4705331154283507__

#include "../../CSChair/debug_int.h"
#include "../../CSArchive/CSBackup/backup_interface.h"
#include <boost/shared_ptr.hpp>

//=====================================================================================//
//                                 class BackupServer                                  //
//=====================================================================================//
class BackupServer
{
	MyDebugOutputImpl				m_Debug;
	boost::shared_ptr<ICSBackUp>	m_pBackup;
	HANDLE							m_hStartThread;

	void	CreateBackupInterface();
	void	CreateBackupInterfaceInThread();
	static unsigned int __stdcall CreateBackupProc(void* pParam);
	static unsigned int __stdcall CreateBackupProc_Handled(void* pParam);
public:
	BackupServer();
	virtual ~BackupServer();

	void	Start(IDebugOutput* pInt);
	void	Stop();
};

#endif //__BACKUP_SERVER_H_INCLUDED_4705331154283507__