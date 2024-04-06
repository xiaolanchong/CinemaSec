//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Helper for loading xml/text resources from resources
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 06.02.2005
//                                                                                      //
//======================================================================================//
#ifndef _HTML_RESOURCE_HELPER_1861197617697799_
#define _HTML_RESOURCE_HELPER_1861197617697799_

#include <string>
//======================================================================================//
//                              struct HtmlResourceHelper                               //
//======================================================================================//

struct HtmlResourceHelper
{
public:
	HtmlResourceHelper();
	~HtmlResourceHelper();

	IStream*		LoadToStream( LPCTSTR szName, LPCTSTR szResourceType );
	bool			InjectCSSIntoHTML(std::wstring& sHtml, const std::wstring& sCss);
	std::wstring	LoadTextResource( LPCTSTR szName, LPCTSTR szResourceType );
};

#endif // _HTML_RESOURCE_HELPER_1861197617697799_