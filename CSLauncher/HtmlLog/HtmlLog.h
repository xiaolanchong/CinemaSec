#pragma once

//#include <atlbase.h>
#include <time.h>
#include <fstream>
#include <vector>

class HtmlLog
{
	mutable std::vector<char>	m_Cache;
	std::ofstream	m_File;
public:
	enum Severity
	{
		sev_debug,
		sev_info,
		sev_warning,
		sev_error,
		sev_unknown
	};

	HtmlLog		(	const wchar_t*	szFileName, 
					const wchar_t*	szSystem, 
					__time64_t		timeStart = _time64(NULL) );
	bool	AddRecord( 
						Severity sev, 
						__time64_t time, 
						const wchar_t* szValue );

	long	GetSize() { return m_File.tellp(); }
	~HtmlLog(void);
};
