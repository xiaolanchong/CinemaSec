#pragma once

class CfgDB
{
	friend CfgDB& GetCfgDB();
	CfgDB();
	
	bool NonNullConfigValue( LPCTSTR szParamName, LPCTSTR szCmdName );
public:

	int			GetRoomID();
	int			GetCinemaID();

	// db connection string
	CString	GetConnectionString();
	void	SetConnectionString( CString strCS );
	// is local? for algorithm developers & local use.
	bool	IsDeveloper();
	// is use the database? for algorithm developers & settings actions
	bool	UseDB();
	// install mode ( configuration )
	bool	IsInstall();
	// has the user administration's rigths?
	bool	IsUserAnAdmin();
	// use log file?
	bool	UseLogFile();

	// from sun=1 to sat=7
	bool	EnableFilmAfterMidnight( int nDay );

	virtual ~CfgDB();
};

CfgDB& GetCfgDB();