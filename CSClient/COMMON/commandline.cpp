#pragma	  warning( disable : 4995 )		//deprecated functions

#include "StdAfx.h"
#include "ICommandLine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _LINUX
#include <limits.h>
#define _MAX_PATH PATH_MAX
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MAX_PARAMETER_LEN = 128;

//-----------------------------------------------------------------------------
// Purpose: Implements ICommandLine
//-----------------------------------------------------------------------------
class CCommandLine : public ICommandLine
{
public:
	// Construction
						CCommandLine( void );
	virtual 			~CCommandLine( void );

	// Implements ICommandLine
	virtual void		CreateCmdLine( const TCHAR *commandline  );
	virtual void		CreateCmdLine( int argc, TCHAR **argv );
	virtual const TCHAR	*GetCmdLine( void ) const;
	virtual	const TCHAR	*CheckParm( const TCHAR *psz, const TCHAR **ppszValue = 0 ) const;

	virtual void		RemoveParm( const TCHAR *parm );
	virtual void		AppendParm( const TCHAR *pszParm, const TCHAR *pszValues );

	virtual int			ParmCount() const;
	virtual int			FindParm( const TCHAR *psz ) const;
	virtual const TCHAR* GetParm( int nIndex ) const;

	virtual const TCHAR	*ParmValue( const TCHAR *psz, const TCHAR *pDefaultVal = NULL ) const;
	virtual int			ParmValue( const TCHAR *psz, int nDefaultVal ) const;
	virtual float		ParmValue( const TCHAR *psz, float flDefaultVal ) const;

private:
	enum
	{
		MAX_PARAMETER_LEN = 128,
		MAX_PARAMETERS = 256,
	};

	// When the commandline contains @name, it reads the parameters from that file
	void LoadParametersFromFile( TCHAR *&pSrc, TCHAR *&pDst );

	// Parse command line...
	void ParseCommandLine();

	// Frees the command line arguments
	void CleanUpParms();

	// Adds an argument..
	void AddArgument( const TCHAR *pFirst, const TCHAR *pLast );

	// Copy of actual command line
	TCHAR *m_pszCmdLine;

	// Pointers to each argument...
	int m_nParmCount;
	TCHAR *m_ppParms[MAX_PARAMETERS];
};


//-----------------------------------------------------------------------------
// Instance singleton and expose interface to rest of code
//-----------------------------------------------------------------------------
static CCommandLine g_CmdLine;
ICommandLine *CommandLine()
{
	return &g_CmdLine;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CCommandLine::CCommandLine( void )
{
	m_pszCmdLine = NULL;
	m_nParmCount = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CCommandLine::~CCommandLine( void )
{
	CleanUpParms();
	delete[] m_pszCmdLine;
}


//-----------------------------------------------------------------------------
// Read commandline from file instead...
//-----------------------------------------------------------------------------
void CCommandLine::LoadParametersFromFile( TCHAR *&pSrc, TCHAR *&pDst )
{
	// Suck out the file name
	TCHAR szFileName[ _MAX_PATH ];
	TCHAR *pOut;

	// Skip the @ sign
	pSrc++;

	pOut = szFileName;

	while ( *pSrc && *pSrc != _T(' ') )
	{
		*pOut++ = *pSrc++;
	}

	*pOut = _T('\0');

	// Skip the space after the file name
	if ( *pSrc )
		pSrc++;

	// Now read in parameters from file
	FILE *fp = _tfopen( szFileName, _T("r") );
	if ( fp )
	{
		TCHAR c;
		c = (TCHAR) _fgettc( fp );
		while ( c != _TEOF )
		{
			// Turn return characters into spaces
			if ( c == _T('\n') )
				c = _T(' ');

			*pDst++ = c;

			// Get the next character, if there are more
			c = (TCHAR)_fgettc( fp );
		}
	
		// Add a terminating space character
		*pDst++ = _T(' ');

		fclose( fp );
	}
	else
	{
		_tprintf( _T("Parameter file '%s' not found, skipping..."), szFileName );
	}
}


//-----------------------------------------------------------------------------
// Creates a command line from the arguments passed in
//-----------------------------------------------------------------------------
void CCommandLine::CreateCmdLine( int argc, TCHAR **argv )
{
	TCHAR cmdline[2048];
	cmdline[0] = _T('\0');
	for ( int i = 0; i < argc; ++i )
	{
		_tcscat( cmdline, argv[i] );
		_tcscat( cmdline, _T(" ") );
	}

	CreateCmdLine( cmdline );
}


//-----------------------------------------------------------------------------
// Purpose: Create a command line from the passed in string
//  Note that if you pass in a @filename, then the routine will read settings
//  from a file instead of the command line
//-----------------------------------------------------------------------------
void CCommandLine::CreateCmdLine( const TCHAR *commandline )
{
	if (m_pszCmdLine)
		delete[] m_pszCmdLine;

	TCHAR szFull[ 4096 ];

	size_t len = _tcslen( commandline ) + 1;
	TCHAR *pOrig = (TCHAR*)_alloca( len * sizeof(TCHAR) );
	_tcscpy( pOrig, commandline );

	TCHAR *pSrc, *pDst;

	pDst = szFull;
	pSrc = pOrig;

	while ( *pSrc )
	{
		if ( *pSrc == _T('@') )
		{
			LoadParametersFromFile( pSrc, pDst );
			continue;
		}

		*pDst++ = *pSrc++;
	}

	*pDst = _T('\0');

	len = _tcslen( szFull ) + 1;
	m_pszCmdLine = new TCHAR[len];
	_tcscpy( m_pszCmdLine, szFull );

	ParseCommandLine();
}


//-----------------------------------------------------------------------------
// Purpose: Remove specified string ( and any args attached to it ) from command line
// Input  : *pszParm - 
//-----------------------------------------------------------------------------
void CCommandLine::RemoveParm( const TCHAR *pszParm )
{
	if ( !m_pszCmdLine )
		return;

	// Search for first occurrence of pszParm
	TCHAR *p, *found;
	TCHAR *pnextparam;
	int n;
	int curlen;
	int curpos;

	while ( 1 )
	{
		p = m_pszCmdLine;
		curlen = _tcslen( p );

		found = _tcsstr( p, pszParm );
		if ( !found )
			break;

		curpos = found - p;

		pnextparam = found + 1;
		while ( pnextparam && *pnextparam && (*pnextparam != _T('-')) && (*pnextparam != _T('+')) )
			pnextparam++;

		if ( pnextparam && *pnextparam )
		{
			// We are either at the end of the string, or at the next param.  Just chop out the current param.
			n = curlen - ( pnextparam - p ); // # of characters after this param.
		
			memcpy( found, pnextparam, n * sizeof( TCHAR ));

			found[n] = _T('\0');
		}
		else
		{
			// Clear out rest of string.
			n = pnextparam - found;
			memset( found, 0, n * sizeof(TCHAR));
		}
	}

	// Strip and trailing ' ' characters left over.
	while ( m_pszCmdLine[ _tcslen( m_pszCmdLine ) - 1 ] == _T(' ') )
	{
		m_pszCmdLine[ _tcslen( m_pszCmdLine ) - 1 ] = _T('\0');
	}

	ParseCommandLine();
}


//-----------------------------------------------------------------------------
// Purpose: Append parameter and argument values to command line
// Input  : *pszParm - 
//			*pszValues - 
//-----------------------------------------------------------------------------
void CCommandLine::AppendParm( const TCHAR *pszParm, const TCHAR *pszValues )
{
	int nNewLength = 0;
	TCHAR *pCmdString;

	nNewLength = _tcslen( pszParm );            // Parameter.
	if ( pszValues )
		nNewLength += _tcslen( pszValues ) + 1;  // Values + leading space character.
	nNewLength++; // Terminal 0;

	if ( !m_pszCmdLine )
	{
		m_pszCmdLine = new TCHAR[ nNewLength ];
		_tcscpy( m_pszCmdLine, pszParm );
		if ( pszValues )
		{
			_tcscat( m_pszCmdLine, _T(" ") );
			_tcscat( m_pszCmdLine, pszValues );
		}

		ParseCommandLine();
		return;
	}

	// Remove any remnants from the current Cmd Line.
	RemoveParm( pszParm );

	nNewLength += _tcslen( m_pszCmdLine ) + 1 + 1;

	pCmdString = new TCHAR[ nNewLength ];
	memset( pCmdString, 0, nNewLength * sizeof(TCHAR) );

	_tcscpy ( pCmdString, m_pszCmdLine ); // Copy old command line.
	_tcscat ( pCmdString, _T(" ") ); // Put in a space
	_tcscat ( pCmdString, pszParm );
	if ( pszValues )
	{
		_tcscat( pCmdString, _T(" ") );
		_tcscat( pCmdString, pszValues );
	}

	// Kill off the old one
	delete[] m_pszCmdLine;

	// Point at the new command line.
	m_pszCmdLine = pCmdString;

	ParseCommandLine();
}


//-----------------------------------------------------------------------------
// Purpose: Return current command line
// Output : const TCHAR
//-----------------------------------------------------------------------------
const TCHAR *CCommandLine::GetCmdLine( void ) const
{
	return m_pszCmdLine;
}


//-----------------------------------------------------------------------------
// Purpose: Search for the parameter in the current commandline
// Input  : *psz - 
//			**ppszValue - 
// Output : TCHAR
//-----------------------------------------------------------------------------
const TCHAR *CCommandLine::CheckParm( const TCHAR *psz, const TCHAR **ppszValue ) const
{
	static TCHAR sz[ MAX_PARAMETER_LEN ];
	TCHAR *pret;

	if ( !m_pszCmdLine )
	{
		return NULL;
	}

	// Point to first occurrence
	pret = _tcsstr( m_pszCmdLine, psz );

	// should we return a pointer to the value?
	if ( pret && ppszValue )
	{
		TCHAR *p1 = pret;
		*ppszValue = NULL;

		while ( *p1 && ( *p1 != 32 ) )
		{
			p1++;
		}

		if ( ( *p1 != 0 ) && ( *p1 != _T('+') ) && ( *p1 != _T('-') ) )
		{
			TCHAR *p2 = ++p1;

			int i;
			for ( i = 0; i < MAX_PARAMETER_LEN ; i++ )
			{
				if ( !*p2 || ( *p2 == _T(' ') ) )
					break;
				sz[i] = *p2++;
			}

			sz[i] = _T('\0');
			*ppszValue = &sz[0];		
		}	
	}

	return pret;
}


//-----------------------------------------------------------------------------
// Adds an argument..
//-----------------------------------------------------------------------------
void CCommandLine::AddArgument( const TCHAR *pFirst, const TCHAR *pLast )
{
	if ( pLast == pFirst )
		return;

	ASSERT( m_nParmCount < MAX_PARAMETERS );

	int nLen = (pLast - pFirst) + 1;
	m_ppParms[m_nParmCount] = new TCHAR[nLen];
	memcpy( m_ppParms[m_nParmCount], pFirst, ( nLen - 1) * sizeof(TCHAR) );
	m_ppParms[m_nParmCount][nLen - 1] = 0;

	++m_nParmCount;
}


//-----------------------------------------------------------------------------
// Parse command line...
//-----------------------------------------------------------------------------
void CCommandLine::ParseCommandLine()
{
	CleanUpParms();
	if (!m_pszCmdLine)
		return;

	const TCHAR *pChar = m_pszCmdLine;
	while ( *pChar && _istspace(*pChar) )
	{
		++pChar;
	}

	bool bInQuotes = false;
	const TCHAR *pFirstLetter = NULL;
	for ( ; *pChar; ++pChar )
	{
		if ( bInQuotes )
		{
			if ( *pChar != _T('\"') )
				continue;

			AddArgument( pFirstLetter, pChar );
			pFirstLetter = NULL;
			bInQuotes = false;
			continue;
		}

		// Haven't started a word yet...
		if ( !pFirstLetter )
		{
			if ( *pChar == _T('\"') )
			{
				bInQuotes = true;
				pFirstLetter = pChar + 1;
				continue;
			}

			if ( _istspace( *pChar ) )
				continue;

			pFirstLetter = pChar;
			continue;
		}

		// Here, we're in the middle of a word. Look for the end of it.
		if ( _istspace( *pChar ) )
		{
			AddArgument( pFirstLetter, pChar );
			pFirstLetter = NULL;
		}
	}

	if ( pFirstLetter )
	{
		AddArgument( pFirstLetter, pChar );
	}
}


//-----------------------------------------------------------------------------
// Individual command line arguments
//-----------------------------------------------------------------------------
void CCommandLine::CleanUpParms()
{
	for ( int i = 0; i < m_nParmCount; ++i )
	{
		delete [] m_ppParms[i];
		m_ppParms[i] = NULL;
	}
	m_nParmCount = 0;
}


//-----------------------------------------------------------------------------
// Returns individual command line arguments
//-----------------------------------------------------------------------------
int CCommandLine::ParmCount() const
{
	return m_nParmCount;
}

int CCommandLine::FindParm( const TCHAR *psz ) const
{
	// Start at 1 so as to not search the exe name
	for ( int i = 1; i < m_nParmCount; ++i )
	{
		if ( !_tcsicmp( psz, m_ppParms[i] ) )
			return i;
	}
	return 0;
}

const TCHAR* CCommandLine::GetParm( int nIndex ) const
{
	ASSERT( (nIndex >= 0) && (nIndex < m_nParmCount) );
	if ( (nIndex < 0) || (nIndex >= m_nParmCount) )
		return _T("");
	return m_ppParms[nIndex];
}


//-----------------------------------------------------------------------------
// Returns the argument after the one specified, or the default if not found
//-----------------------------------------------------------------------------
const TCHAR *CCommandLine::ParmValue( const TCHAR *psz, const TCHAR *pDefaultVal ) const
{
	int nIndex = FindParm( psz );
	if (( nIndex == 0 ) || (nIndex == m_nParmCount - 1))
		return pDefaultVal;

	// Probably another cmdline parameter instead of a valid arg if it starts with '+' or '-'
	if ( m_ppParms[nIndex + 1][0] == _T('-') || m_ppParms[nIndex + 1][0] == _T('+') )
		return pDefaultVal;

	return m_ppParms[nIndex + 1];
}

int	CCommandLine::ParmValue( const TCHAR *psz, int nDefaultVal ) const
{
	int nIndex = FindParm( psz );
	if (( nIndex == 0 ) || (nIndex == m_nParmCount - 1))
		return nDefaultVal;

	// Probably another cmdline parameter instead of a valid arg if it starts with '+' or '-'
	if ( m_ppParms[nIndex + 1][0] == _T('-') || m_ppParms[nIndex + 1][0] == _T('+') )
		return nDefaultVal;

	return _ttoi( m_ppParms[nIndex + 1] );
}

float CCommandLine::ParmValue( const TCHAR *psz, float flDefaultVal ) const
{
	int nIndex = FindParm( psz );
	if (( nIndex == 0 ) || (nIndex == m_nParmCount - 1))
		return flDefaultVal;

	// Probably another cmdline parameter instead of a valid arg if it starts with '+' or '-'
	if ( m_ppParms[nIndex + 1][0] == _T('-') || m_ppParms[nIndex + 1][0] == _T('+') )
		return flDefaultVal;

	return (float)_tstof( m_ppParms[nIndex + 1] );
}
