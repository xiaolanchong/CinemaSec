/*
*	CSBackup 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Utility.hpp
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
#ifndef __UTILITY_HPP__
#define __UTILITY_HPP__

class CUtility
{
public:
	//Registry tools
	static void GetDatabaseConnectionString( std::wstring& sConnectionString );
	static void GetArchivePath( std::wstring& sPath );
	static void GetLocalHostIP( std::wstring& sHostIP );
	static void GetRoomIDs( std::vector<DWORD>& roomIds );
	static void GetCinemaID( DWORD& dwCinemaID  );

	//Misc. tools
	static void BuildPath( std::wstring& sPath, const std::wstring& sFileExt, short m_nCamId, 
		short m_nYear, short m_nMonth, short m_nDay, short m_nHour, short m_nMinute, short m_nSecond );
	static void ParsePath( const std::wstring& sPath, std::wstring& sFileExt, short& m_nCamId, 
		short& m_nYear, short& m_nMonth, short& m_nDay, short& m_nHour, short& m_nMinute, short& m_nSecond );

};
#endif //__UTILITY_HPP__
