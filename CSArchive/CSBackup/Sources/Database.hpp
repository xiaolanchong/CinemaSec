/*
*	CSBackup 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Database.hpp
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
#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

class CCinemaDatabase
{
public:
	//Accessors
	class CCamerasAccessor
	{
	public:
		LONG m_CameraID;

		BEGIN_COLUMN_MAP(CCamerasAccessor)
			COLUMN_ENTRY(1, m_CameraID)
		END_COLUMN_MAP()
	};

	class CCamerasAmAcc
	{
	public:
		LONG m_CamerasAmount;

		BEGIN_COLUMN_MAP(CCamerasAmAcc)
			COLUMN_ENTRY(1, m_CamerasAmount)
		END_COLUMN_MAP()
	};

	class CCinemaAcc
	{
	public:
		double m_BackupStart;
		double m_BackupStop;
		unsigned int   m_BackupDate;  

		BEGIN_COLUMN_MAP(CCinemaAcc)
			COLUMN_ENTRY(1, m_BackupStart)
			COLUMN_ENTRY(2, m_BackupStop)
			COLUMN_ENTRY(3, m_BackupDate)
		END_COLUMN_MAP()
	};
	//
public:
	CCinemaDatabase( const std::wstring& sConnectionString );
	virtual ~CCinemaDatabase();
public:
	void Connect();
	void Disconnect();

public:
	void GetCameraIDs( std::wstring& sHostIP, std::vector<int>& ids );
	void GetCleaningParams( const std::vector<DWORD>& roomIds, double& dBackupStart, double& dBackupStop, DATE& LastDate  );
	void UpdateStatistics( const std::vector<DWORD>& roomIds, DATE& upToDate );


private:
	boost::shared_ptr<CDataSource> m_pDataSource;
	std::wstring m_sConnectionString;
};



#endif //__DATABASE_HPP__
