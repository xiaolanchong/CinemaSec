//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Helper for description extraction from system & user DLLs
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 01.06.2005
//                                                                                      //
//======================================================================================//
#ifndef _RESULT_DESC_7184928407142800_
#define _RESULT_DESC_7184928407142800_

#include <boost/shared_ptr.hpp>
#include <vector>
//======================================================================================//
//                                   class ResultDesc                                   //
//======================================================================================//

class ResultDesc
{
	std::vector< boost::shared_ptr<HMODULE> >	m_DynLibraries;
public:
	ResultDesc( LPCWSTR* ppDllNames, size_t nCount );
	std::wstring GetErrorDesc( DWORD dwRes );
	virtual ~ResultDesc();
};

#endif // _RESULT_DESC_7184928407142800_