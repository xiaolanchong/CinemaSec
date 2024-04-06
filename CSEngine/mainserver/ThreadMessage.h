//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   01.03.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __THREAD_MESSAGE_H_INCLUDED_2186836720206347__
#define __THREAD_MESSAGE_H_INCLUDED_2186836720206347__

#include <queue>
#include <boost/any.hpp>
//=====================================================================================//
//                                 class ThreadMessage                                 //
//=====================================================================================//

class ThreadMessageAutoEnter;

class ThreadMessage
{
	friend class ThreadMessageAutoEnter;
	CRITICAL_SECTION	m_cs;
	typedef  std::pair< int , boost::any > Record_t;

	std::queue< Record_t >		m_Buffer;
	size_t						m_nMaxSize;
public:

	void Push( ThreadMessageAutoEnter& /*me*/, int Id, const boost::any&	Data )
	{
//		UNREFERENCED_PARAMETER(me);
		if( m_nMaxSize < m_Buffer.size() )
		{
			m_Buffer.pop();
		}
		// comment if simulate no send
		m_Buffer.push( std::make_pair(Id, Data) );
	}
	void Pop(ThreadMessageAutoEnter& /*me*/) 
	{
//		UNREFERENCED_PARAMETER(me);
		m_Buffer.pop();
	};
	bool IsEmpty(ThreadMessageAutoEnter& /*me*/) const { return m_Buffer.empty();}
	const  boost::any& Front( ThreadMessageAutoEnter& /*me*/, int& Id ) const 
	{ 
	//	UNREFERENCED_PARAMETER(me);
		Id = m_Buffer.front().first; 
		return m_Buffer.front().second;
	};

	ThreadMessage(size_t nMaxSize = 100);
	virtual ~ThreadMessage();
};

class ThreadMessageAutoEnter
{
	bool				m_bIsIn;
	CRITICAL_SECTION&	m_cs;
public:
	ThreadMessageAutoEnter( ThreadMessage& tm, bool bWait ):
		m_cs( tm.m_cs )
	{
		if( bWait )
		{
			EnterCriticalSection(&m_cs);
			m_bIsIn = true;
		}
		else
			m_bIsIn = (TryEnterCriticalSection( &m_cs ) == TRUE);
	}
	~ThreadMessageAutoEnter()
	{
		if(  m_bIsIn )
		{
			LeaveCriticalSection( &m_cs );
		}
	}

	bool IsEntered() const { return m_bIsIn;}
};

#endif //__THREAD_MESSAGE_H_INCLUDED_2186836720206347__