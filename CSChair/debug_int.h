//! \author	Eugene Gorbachev
//! \date	2004-11-23
//	moified	2005-10-06

#ifndef __DEBUG_INT_H__
#define __DEBUG_INT_H__

#include "../CSUtility/utility/debug_interface.h"

/*
struct IDebugOutput
{
	enum MessageType
	{
		mt_debug_info = 0,
		mt_info,
		mt_warning,
		mt_error
	};	
	
	virtual void PrintW( unsigned int mt, LPCWSTR szMessage) = 0;
	virtual void PrintA( unsigned int mt, LPCSTR szMessage) = 0;
};
*/

//! шаблон для автоматического вызова удаления интерфейса в boost::shared_ptr
template<class T> struct ReleaseInterface
{
	void operator () (  T* p) 
	{
		if(p) p->Release();
	}
};

//! обертка над IDebugOutput
//! создает форматированную строку в соответствии с параметрами

struct MyDebugOutputImpl /*: public IDebugOutput*/
{
	//! обернутый интерфейс
	IDebugOutput* m_pInt; 
public:
	MyDebugOutputImpl(IDebugOutput* p = NULL):m_pInt(p){}

	//! атомарная установка
	void			Set( IDebugOutput* pInt )	{ InterlockedExchangePointer( (void**)&m_pInt ,pInt);	}
	//! атомарное считывание
	IDebugOutput*	Get( )  const				{ return m_pInt;	}
	void			Release()					{ InterlockedExchangePointer( (void**)&m_pInt ,NULL);	}

	//! форматировние для ANSI-строки
	//!	\param	mt			тип сообщения (см. IDebugOutput)
	//! \param	szMessage	строка формата, см. _vsnprintf
	void PrintA( int mt, LPCSTR szMessage, ... )
	{
		va_list		argptr;
		char		text[1024];

		va_start (argptr, szMessage);
		_vsnprintf (text, 1024, szMessage, argptr);
		va_end (argptr);

		if(m_pInt) 
		{
			m_pInt->PrintA(mt, text);
		}
		else
		{
			OutputDebugStringA( text );
			OutputDebugStringA( "\n" );
		}
	}

	//! форматировние для UNICODE-строки
	//!	\param	mt			тип сообщения (см. IDebugOutput)
	//! \param	szMessage	строка формата, см. _vsnwprintf
	void PrintW( int mt, LPCWSTR szMessage, ... )
	{
		va_list		argptr;
		wchar_t		text[1024];

		va_start (argptr, szMessage);
		_vsnwprintf (text, 1024, szMessage, argptr);
		va_end (argptr);

		if(m_pInt)
		{
			m_pInt->PrintW(mt, text);
		}
		else
		{
			OutputDebugStringW( text );
			OutputDebugStringW( L"\n" );
		}
	}
};

#endif //__DEBUG_INT_H__