//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Создает временный файл html-отчета сообщений обмена
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 13.09.2005
//                                                                                      //
//======================================================================================//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "ExchangeReport.h"
#include <atlenc.h>
//======================================================================================//
//                                 class ExchangeReport                                 //
//======================================================================================//
ExchangeReport::ExchangeReport()
{
	TCHAR	szPathBuf[MAX_PATH], 
			szBuf[MAX_PATH];
	GetTempPath(MAX_PATH, szPathBuf);
	PathCombine( szBuf, szPathBuf, _T("exchange_cache_{83DCE86C-3291-40a2-8035-592B5D512428}.html") );
	m_sFileName = szBuf;

	m_File.Open( szBuf, CFile::modeCreate | CFile::modeWrite )  ;
	AddHeader();
}

ExchangeReport::~ExchangeReport()
{
	AddFooter();
}

static std::wstring ReplaceSpecialChars( LPCWSTR _s, bool bSpace )
{
	CStringW s(_s);
	s.Replace( L"&",	L"&amp;"	);
	s.Replace( L"<",	L"&lt;"		);
	s.Replace( L">",	L"&gt;"		);
	s.Replace( L"\"",	L"&quot;"	);
	s.Replace( L"'",	L"&#39;"	);
	if(bSpace)
	{
		s.Replace( L"\n",	L"<br>"	);
	}
	return std::wstring(s);
}

void			ExchangeReport::AddMessage( const DBProtocolTable::Message_t& msg )
{
	CStringW sType;
	LPCSTR szType;
	switch( boost::get<0>( msg ) )
	{
	case DBProtocolTable::BO_Timetable :	sType.LoadString( IDS_MSG_BO_TIMETABLE );	szType = "bo_timetable";break;
	case DBProtocolTable::BO_Response :		sType.LoadString( IDS_MSG_BO_RESPONSE );	szType = "bo_response";break;
	case DBProtocolTable::CCS_Statistics :	sType.LoadString( IDS_MSG_CCS_STATISTICS );	szType = "ccs_statistics";break;
	case DBProtocolTable::CCS_Response :	sType.LoadString( IDS_MSG_CCS_RESPONSE );	szType = "ccs_response";break;
	default :								sType.LoadString( IDS_MSG_UNKNOWN );		szType = "unknown";break;
	}


	__time64_t t = boost::get<1>( msg ).GetTime();
	struct tm * ptm =  _localtime64( &t );
	
	CHAR szTimeBuf[255];
	strftime( szTimeBuf, 255, "%d.%m.%y %H:%M", ptm );

	std::string sUtfType = ToUTF8( sType );

	CHAR szBuffer[512];
	StringCchPrintfA( szBuffer, 512, 
		"<tr class=\"%s\">\n"
		"	<td>%s</td>\n"
		"	<td>%s</td>\n"
		"	<td>",
		szType, szTimeBuf, sUtfType.c_str() );
	WriteText( szBuffer );

	WriteText( boost::get<2>( msg ).c_str() );

	WriteText("</td>\n</tr>\n");
}

void ExchangeReport::AddHeader()
{
	LPCSTR szHeader = 
	"<html>\n"
	"<head>\n"
	"<title>Exchange Message Log</title>\n"
	"<meta HTTP-EQUIV=\"Content-Type\" Content=\"text/html; charset=UTF-8\">\n"
	"<style>\n"
	"\n"
	"body"
	"{\n"
//	"	margin: 2px 10px 0px 10px;\n"
	"	margin: 0px 0px 0px 0px;\n"
	"	padding: 0px 0px 0px 0px;\n"
	"	background: #ffffff;\n"
	"	color: #000000;\n"
	"	font-family: Verdana, Arial, Helvetica, sans-serif;\n"
	"	font-size: 70%;\n"
	"	width: 100%;\n"
	"}\n"
	"table\n"
	"{\n"
	"	width: 100%;\n" 
	"	borderWidth  : 0;\n"
//	"	margin-top: .6em;\n" 
//	"	margin-bottom: .3em;\n" 
	"	font-size: 100%;\n"
	"	background-color: #999999;\n"
	"	border-width: 1px 1px 0px 0px;\n" 
	"	border-style: solid;\n" 
	"	border-color: #999999;\n" 
	"}\n" 
	"th, td\n" 
	"{\n" 
	"	border-style: solid;\n"
	"	border-width: 0px 0px 1px 1px;\n" 
	"	border-style: solid;\n" 
	"	border-color: #999999;\n" 
	"	padding: 4px 6px;\n" 
	"	text-align: left;\n" 
	"}\n" 
	"th\n"	
	"{\n" 
	"	background: #cccccc;\n" 
	"	vertical-align: bottom;\n" 
	"}\n" 
	"td\n"	
	"{\n" 
	"	background: #ffffff;\n" 
	"	vertical-align: top;\n" 
	"}\n" 
	".bo_timetable	{ color: green 		}\n" 
	".bo_response	{ color: limegreen 	}\n" 
	".ccs_statistics	{ color: blueviolet }\n" 
	".ccs_response	{ color: navy 		}\n" 
	".unknown		{ color: red ; font-weight : bold }\n" 
	"</style>\n"
	"</head>\n"
	"<body>\n"
	/*	<h2>Main, Aug10 18:26</h2>*/
	"<table cellpadding=\"0\" cellspacing=\"0\">\n"
	"<tr>\n"
	"<th width=\"15%\"><b>Р’СЂРµРјСЏ</b></th>\n" 
	"<th width=\"10%\"><b>РўРёРї</b></th>\n" 
	"<th><b>РўРµРєСЃС‚</b></th>\n" 
	"</tr>\n";

	WriteText( szHeader );

}

void ExchangeReport::AddFooter()
{
	LPCSTR szFooter = 
		"</table>\n"
		"</body>\n"
		"</html>";

	WriteText( szFooter );
}

void	 ExchangeReport::WriteText( LPCSTR szText)
{
	m_File.Write( szText, lstrlenA(szText) );
}

void	ExchangeReport::WriteText( LPCWSTR szText)
{
	std::string s = ToUTF8( szText );
	WriteText( s.c_str() );
}

std::string ExchangeReport::ToUTF8( LPCWSTR szText )
{
	std::wstring sNewValue = ReplaceSpecialChars( szText, true );
	int nSize = (int)sNewValue.size();

	int nCharNumber = ATL::AtlUnicodeToUTF8( sNewValue.c_str(), nSize, 0, 0 );
	std::vector<char> Cache( nCharNumber + 1, '\0' );
	ATL::AtlUnicodeToUTF8( sNewValue.c_str(), nSize, &Cache[0],(int) nCharNumber );
	return std::string( &Cache[0], &Cache[0] + Cache.size() );
}