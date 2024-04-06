#ifndef __WV_MESSAGE_H__
#define __WV_MESSAGE_H__

#include "../../CSChair/public/NameParse.h"

struct VideoWindowInfo
{
	VideoType				m_Type;
	int						m_X, m_Y;
	int						m_nID;
	CString					m_strPath;

	VideoWindowInfo() : 
	m_Type(vidt_error),
		m_X(0), m_Y(0), m_nID(-1)
	{}
		VideoWindowInfo( const VideoWindowInfo& wi):
	m_Type(wi.m_Type),
		m_X(wi.m_X), 
		m_Y(wi.m_Y),
		m_nID(wi.m_nID),
		m_strPath(wi.m_strPath){}		
};

typedef std::vector<VideoWindowInfo> WindowArray_t;

#endif //__WV_MESSAGE_H__