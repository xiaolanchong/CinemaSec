#ifndef _ICOMMANDLINE_H
#define _ICOMMANDLINE_H
#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Interface to engine command line
//-----------------------------------------------------------------------------
class ICommandLine
{
public:
	virtual void		CreateCmdLine( const TCHAR *commandline ) = 0;
	virtual void		CreateCmdLine( int argc, TCHAR **argv ) = 0;
	virtual const TCHAR	*GetCmdLine( void ) const = 0;

	// Check whether a particular parameter exists
	virtual	const TCHAR	*CheckParm( const TCHAR *psz, const TCHAR **ppszValue = 0 ) const = 0;
	virtual void		RemoveParm( const TCHAR *parm ) = 0;
	virtual void		AppendParm( const TCHAR *pszParm, const TCHAR *pszValues ) = 0;

	// Returns the argument after the one specified, or the default if not found
	virtual const TCHAR	*ParmValue( const TCHAR *psz, const TCHAR *pDefaultVal = 0 ) const = 0;
	virtual int			ParmValue( const TCHAR *psz, int nDefaultVal ) const = 0;
	virtual float		ParmValue( const TCHAR *psz, float flDefaultVal ) const = 0;

	// Gets at particular parameters
	virtual int			ParmCount() const = 0;
	virtual int			FindParm( const TCHAR *psz ) const = 0;
	virtual const TCHAR* GetParm( int nIndex ) const = 0;
};


ICommandLine *CommandLine();

#endif // _ICOMMANDLINE_H