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
#ifndef _EXCHANGE_REPORT_3169731773927978_
#define _EXCHANGE_REPORT_3169731773927978_

#include "../../CSEngine/DBFacet/DBRows.h"
//======================================================================================//
//                                 class ExchangeReport                                 //
//======================================================================================//

class ExchangeReport
{
	CString m_sFileName;
	CFile	m_File;

	void	AddHeader();
	void	AddFooter();
	void	WriteText( LPCSTR szText);
	void	WriteText( LPCWSTR szText);
	std::string ToUTF8( LPCWSTR szText );
public:
	ExchangeReport();
	virtual ~ExchangeReport();

	void	AddMessage( const DBProtocolTable::Message_t& msg );
	CString GetFileName() const { return m_sFileName; };
};

#endif // _EXCHANGE_REPORT_3169731773927978_