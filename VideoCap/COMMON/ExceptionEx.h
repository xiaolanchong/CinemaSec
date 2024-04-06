#ifndef _EXCEPTION_EX_H_
#define _EXCEPTION_EX_H_

#include <exception>
#include <string>

#define MACRO_EXCEPTION_EX(Child, Parent) \
class Child : public Parent \
{ \
public: \
	Child(const std::wstring& Cause): \
		Parent(Cause){}  \
	Child(const wchar_t* Cause): \
		Parent(Cause){} \
	Child(const Child& ex): Parent(ex){} \
};

class CommonExceptionEx : public std::exception
{
	std::wstring m_Cause;
protected:
	CommonExceptionEx( const std::wstring& cause):
	  m_Cause( cause )
	  {
	  }
	  CommonExceptionEx( const wchar_t* cause):
	  m_Cause( cause )
	  {
	  }
public:
	~CommonExceptionEx(){}
		
	  const char* 		what() const { return "";}
	  const wchar_t*	whatW() const { return m_Cause.c_str();}
};

#endif	_EXCEPTION_EX_H_