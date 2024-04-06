#pragma	  warning( disable : 4995 )		//deprecated functions

#include "StdAfx.h"
#include <tchar.h>
#include <windows.h>
#include ".\htmllog.h"
#include <atlenc.h>

static LPCSTR szCSSTable = 
"\n\
body\n\
{\n\
margin: 2px 10px 0px 10px;\n\
padding: 0px 0px 0px 0px;\n\
background: #ffffff;\n\
color: #000000;\n\
	font-family: Verdana, Arial, Helvetica, sans-serif;\n\
	font-size: 70%;\n\
width: 100%;\n\
}\n\
table\n\
{\n\
	width: 100%; \n\
	borderWidth  : 0; \n\
	margin-top: .6em; \n\
	margin-bottom: .3em; \n\
	font-size: 100%; \n\
	background-color: #999999; \n\
	border-width: 1px 1px 0px 0px; \n\
	border-style: solid; \n\
	border-color: #999999; \n\
} \n\
th, td \n\
{ \n\
	border-style: solid; \n\
	border-width: 0px 0px 1px 1px; \n\
	border-style: solid; \n\
	border-color: #999999; \n\
padding: 4px 6px; \n\
	text-align: left; \n\
} \n\
th	\n\
{ \n\
background: #cccccc; \n\
	vertical-align: bottom; \n\
} \n\
td	\n\
{ \n\
background: #ffffff; \n\
	vertical-align: top; \n\
} \n\
.debug		{ color: silver } \n\
.info 		{ color: black } \n\
.warning	{ color: gold } \n\
.error 		{ color: red ; font-weight : bold } \n\
.unknown	{ color: blue ; font-weight } \n\
";

HtmlLog::HtmlLog(const wchar_t* szFileName, const wchar_t* szSystem, __time64_t  timeStart)

{
	int nSize = lstrlenW( szFileName );
	std::vector<char> BufName( (nSize + 1)* sizeof(wchar_t), 0 );
	size_t TrueSize = wcstombs( &BufName[0],  szFileName, nSize  );
	TrueSize;

	m_File.open( &BufName[0], std::ios::binary  /*| std::ios::ate*/ );

	nSize = lstrlenW( szSystem );
	m_Cache.resize( sizeof( WCHAR ) * nSize );
	int res = ATL::AtlUnicodeToUTF8( szSystem, nSize, &m_Cache[0],(int) m_Cache.size() );

	char szBuffer[256];
//	strftime( szBuf, 256, "%b%d %H:%M", timeStart );

	struct tm* ptmTemp = _localtime64(&timeStart);
	if (ptmTemp == NULL ||
		!strftime(szBuffer, 256, "%b%d %H:%M", ptmTemp))
		szBuffer[0] = '\0';

	m_File 
	<< "<html>" << std::endl
	<< "<head>"	 << std::endl
	<< "<title>";
	m_File.write( &m_Cache[0], res );
	m_File << ", " << szBuffer << "</title>" << std::endl
	<< "<meta HTTP-EQUIV=\"Content-Type\" Content=\"text/html; charset=UTF-8\">" << std::endl
	<< "<style>" << std::endl
	<< szCSSTable << std::endl
	<< "</style>" << std::endl
	<< "</head>" << std::endl
	<< "<body>"	<< std::endl
	<< "<h2>" ;
	m_File.write( &m_Cache[0], res );
	m_File << ", " << szBuffer << "</h2>"<< std::endl
	<< "<table cellpadding=\"0\" cellspacing=\"0\">" << std::endl
	<< "<tr>" << std::endl
	<< "<th align=\"center\" width=\"10%\"><b>Date</b></td> \n\
		<th align=\"center\" width=\"10%\"><b>Time</b></td> \n\
		<th align=\"center\"><b>Contents</b></td> \n\
		</tr>"
	;
}

HtmlLog::~HtmlLog(void)
{
	m_File 
	<< "</table>"	<< std::endl 
	<< "</body>"	<< std::endl 
	<< "</html>"	<< std::endl;
}

static std::wstring ReplaceSpecialChars( LPCWSTR _s )
{
	CStringW s(_s);
	s.Replace( L"&",	L"&amp;"	);
	s.Replace( L"<",	L"&lt;"		);
	s.Replace( L">",	L"&gt;"		);
	s.Replace( L"\"",	L"&quot;"	);
	s.Replace( L"'",	L"&#39;"	);
	return std::wstring(s);
}

bool	HtmlLog::AddRecord( HtmlLog::Severity sev, __time64_t time, LPCWSTR szValue )
{

	m_File
	<< "<tr class=\"";
	switch( sev) 
	{
	case sev_debug:	m_File << "debug";
		break;
	case  sev_info:	m_File << "info";
		break;
	case  sev_warning:	m_File << "warning";
		break;
	case  sev_error:	m_File << "error";
		break;
	default:	m_File << "unknown";
	}
	m_File << "\">";

	char szBuffer[256];
	struct tm* ptmTemp = _localtime64(&time);
	if (ptmTemp == NULL ||
		!strftime(szBuffer, 256, "%b %d", ptmTemp))
		szBuffer[0] = '\0';
	
	m_File << "<td>" << szBuffer << "</td>" << std::endl;
	
	if (ptmTemp == NULL ||
		!strftime(szBuffer, 256, "%H:%M:%S", ptmTemp))
		szBuffer[0] = '\0';
	
	m_File << "<td>" << szBuffer << "</td>" << std::endl;

#if 0
	int nSize = lstrlenW( szValue );
	m_Cache.resize( sizeof( WCHAR ) * nSize );
	int res = ATL::AtlUnicodeToUTF8( szValue, nSize, &m_Cache[0],(int) m_Cache.size() );
#else
	std::wstring sNewValue = ReplaceSpecialChars( szValue );
	int nSize = (int)sNewValue.size();
	m_Cache.resize( sizeof( WCHAR ) * nSize );
	int res = ATL::AtlUnicodeToUTF8( sNewValue.c_str(), nSize, &m_Cache[0],(int) m_Cache.size() );
#endif
	m_File << "<td>";
	m_File.write( &m_Cache[0], res );
	m_File << "</td>" << std::endl
	<< "</tr>" << std::endl;
	return true;
}