#pragma once

#include "../common/Exception.h"
#include "../common/tstring.h"

//! cracks URL on to schema, name etc.
class UrlName
{
	//! source URL
	std::tstring	m_sUrl;
	//! internal crack function
	std::tstring	GetPart(DWORD dwPart) const;
public:
	UrlName( LPCTSTR szUrl) : m_sUrl(szUrl){}

	//! \return	schema name, e.g. http from http://myserver:port
	std::tstring GetSchema()	const;
	//! \return	server name, e.g. server from http://myserver:port
	std::tstring GetHostName()	const;
	//! \return	port, e.g. server from http://myserver:port
	std::tstring GetPort()		const;
	std::tstring GetPassword()	const;
	std::tstring GetQuery()		const;
	std::tstring GetUserName()	const;
};

MACRO_EXCEPTION(SourceTypeException, CommonException);

//! video type source
enum VideoType
{
	//! error source
	vidt_error,
	//! video file source
	vidt_file,
	//! camera image source
	vidt_camera
};

typedef std::pair<VideoType, std::tstring> VideoSource_t;

//! detect source from the name
class NameParse
{
	VideoType		m_vt;
	std::tstring	m_sSource;
public:
	NameParse( LPCTSTR szVideoSource );
	VideoType		GetType()	const { return m_vt;		}
	std::tstring	GetSource()	const { return m_sSource;	}
};