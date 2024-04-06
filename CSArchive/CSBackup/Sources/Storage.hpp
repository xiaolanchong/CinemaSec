/*
*	CSBackup 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Storage.hpp
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
#ifndef __STORAGE_HPP__
#define __STORAGE_HPP__

class CCinemaStorage
{
	static boost::wregex m_PartialPathExpression;
public:
	CCinemaStorage( const std::wstring& sPath, std::vector<int>& nCamIds, IDebugOutput* pDebug ); //Start-stop condition
	virtual ~CCinemaStorage(){};
public:
	class CFileProxy
	{
	public:
		CFileProxy(){};
		CFileProxy( const std::wstring& sPath );
	public:
		void GetPartialFilePath( std::wstring& sPath ); 
		void GetPartialPath( std::wstring& sPath );
		DATE GetDate();
		COleDateTime GetOleDateTime();
	public:
		bool operator<( const CFileProxy& rhs );
		bool operator==( const CFileProxy& rhs );
		bool operator!=( const CFileProxy& rhs );
	private:
		short m_nCamId;
        short m_nYear, m_nMonth, m_nDay;
		short m_nHour, m_nMinute, m_nSecond;
	};

	typedef std::list<CCinemaStorage::CFileProxy>::iterator CFileProxyIt;

public:
	bool StorageModified();
	void RemoveDirectories();
	void Synchronize();
	void Sort();
	void GetPath( std::wstring& sPath );
	CFileProxyIt Delete( CFileProxyIt whereIt );
	CFileProxyIt FakeDelete( CFileProxyIt whereIt );

	size_t GetCount();
	CFileProxyIt Begin();
	CFileProxyIt End();

public:
	UINT64 GetTotalSpace();
	UINT64 GetAvailableSpace();
	double GetAvailableSpaceBalance();

private:
	bool m_bStorageModified;
	IDebugOutput* m_pDebug;
	std::wstring m_sPath;
	std::vector<int> m_nCamIds;
	std::list<std::wstring> m_Dirs;
	std::list<CCinemaStorage::CFileProxy> m_Files;
};



#endif //__STORAGE_HPP__
