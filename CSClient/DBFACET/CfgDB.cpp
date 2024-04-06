#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include ".\cfgdb.h"
#include "..\common\ICommandLine.h"

#define CMD_PARAM(x) (_T("-") x)
#define szDB_Sec_Settings			 _T("Settings")
#define szDB_Par_ConnectionString	 _T("DBConnectionString")
#define szDB_Par_DBStub				 _T("DBStub")
#define szDB_Par_DeveloperMode		 _T("Developer")
#define szDB_Par_Midnight			 _T("Midnight")

#define szDB_CmdPar_DeveloperMode		CMD_PARAM(szDB_Par_DeveloperMode)
#define szDB_CmdPar_InstallMode			CMD_PARAM(_T("install"))
#define szDB_CmdPar_UseLogFile			CMD_PARAM(_T("log"))

#define szMain_Subkey					 _T("Software\\ElVEES\\CinemaSec")

CfgDB::CfgDB(void)
{
}

CfgDB::~CfgDB(void)
{
}

int GetRegistryID( LPCTSTR szSection, LPCTSTR szValue, int defValue )
{
//	const int c_nInvalidID = -1;
	DWORD dwType;
	DWORD dwData;
	DWORD dwDataLen = sizeof(DWORD);
	LONG res = SHGetValue( HKEY_LOCAL_MACHINE, szSection , szValue, &dwType, &dwData, &dwDataLen );
	if(res != ERROR_SUCCESS || dwType != REG_DWORD || dwDataLen != sizeof(DWORD))
	{
		return defValue;
	}
	else
		return dwData;
}

int		CfgDB::GetRoomID()
{
	return GetRegistryID(  szMain_Subkey , _T("RoomID"), -1 );
}

int		CfgDB::GetCinemaID()
{
	return GetRegistryID(  szMain_Subkey , _T("CinemaID"), -1 );
}

CString	CfgDB::GetConnectionString()
{
#define DB_SUBKEY	szMain_Subkey
#define DB_VALUE	_T("DBConnectionString")
	TCHAR szCS[1024];
	DWORD	dwData = sizeof(szCS),
	dwType = REG_SZ;
	SHGetValue(HKEY_LOCAL_MACHINE, DB_SUBKEY, DB_VALUE, &dwType, szCS, &dwData);
	return CString( szCS );
}

void	CfgDB::SetConnectionString( CString strCS )
{
	AfxGetApp()->WriteProfileString(szDB_Sec_Settings,  szDB_Par_ConnectionString, strCS );
}

bool CfgDB::NonNullConfigValue( LPCTSTR szParamName, LPCTSTR szCmdName )
{
	int par = 0;
	int Pos = CommandLine()->FindParm(szCmdName);
	if( Pos == 0 )
		par = AfxGetApp()->GetProfileInt( szDB_Sec_Settings, szParamName, 0 );
	else
		par = CommandLine()->ParmValue( szCmdName, 0 ) ;
	return par != 0;
}

bool	CfgDB::UseDB()
{
	// if no param then use DB, else do not use
	int nMode =  GetRegistryID(  szMain_Subkey , szDB_Par_DBStub, 0 );
	return nMode == 0;
}


bool	CfgDB::IsDeveloper()
{
	bool par =  NonNullConfigValue( szDB_Par_DeveloperMode, szDB_CmdPar_DeveloperMode );
	if( !par )
	{
		return GetRegistryID(  szMain_Subkey , szDB_Par_DeveloperMode, 0 ) != 0;
	}
	else return par;
}

bool	CfgDB::IsInstall()
{
	LPCTSTR pParam = CommandLine()->CheckParm( szDB_CmdPar_InstallMode );
	return pParam != NULL;
}

bool	CfgDB::IsUserAnAdmin()
{
#if 1
	return ::IsUserAnAdmin() == TRUE;
#else
	return false;
#endif
}

bool	CfgDB::UseLogFile()
{
	// 2005-07-11 use log always
#if 0
	LPCTSTR pParam = CommandLine()->CheckParm( szDB_CmdPar_UseLogFile );
	return pParam != NULL;
#else
	return TRUE;
#endif 
}

bool	CfgDB::EnableFilmAfterMidnight( int nDay )
{
	// 1- sun
	// 2- mon
	// 3- tue
	// 4- wen
	// 5- thu
	// 6- fri
	// 7- sat
	const DWORD c_nDefValue = (1<<6)|(1<<0)|(1<<1);
	DWORD dwType = REG_DWORD;
	DWORD dwData;
	DWORD dwSize = sizeof(DWORD);
	DWORD dwRes = SHGetValue( HKEY_LOCAL_MACHINE, szMain_Subkey, szDB_Par_Midnight, &dwType, &dwData, &dwSize );
	if(dwRes != ERROR_SUCCESS) dwData = c_nDefValue;
	return ( dwData & (1 << (nDay - 1)) ) != 0;
}

/////// singleton ////////////////

CfgDB& GetCfgDB()
{
	static bool bInit = false;
	if(!bInit)
	{
		CommandLine()->CreateCmdLine(GetCommandLine());
		bInit = true;
	}
	static CfgDB cfg;
	return cfg;
}

