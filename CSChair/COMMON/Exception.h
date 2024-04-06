#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <exception>
#include <string>

#define MACRO_EXCEPTION(Child, Parent) \
class Child : public Parent \
{ \
public: \
	Child(const std::string& Cause): \
		Parent(Cause){}  \
	Child(const char* Cause): \
		Parent(Cause){} \
	Child(const Child& ex): Parent(ex){} \
};

class CommonException : public std::exception
{
	std::string m_Cause;
public:
	CommonException( const std::string& cause):
	  m_Cause( cause )
	  {
//#define EXCEPTION_INT3 
#ifdef EXCEPTION_INT3 
		  _asm int 3
#endif
	  }

	  const char* what() const { return m_Cause.c_str();}
};
/*
MACRO_EXCEPTION	(FileException, CommonException);
MACRO_EXCEPTION	(PhysicalFileException, FileException); // error with file
MACRO_EXCEPTION	(FormatFileException, FileException);	// error with format*/

#if (defined _MSC_VER) // <= 1200
#define THROW_HEADER(x) 
#else
#define THROW_HEADER(x) throw (x)
#endif // _MSC_VER <= 1200

#endif	_EXCEPTION_H_