#ifndef __TSTRING_H__
#define __TSTRING_H__

#include <string>
#if (defined(_UNICODE) || defined (UNICODE) )
#define tstring wstring
#else	
#define tstring string
#endif

#endif //__TSTRING_H__