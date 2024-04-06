// BoxOfficeRequest.cpp : Implementation of CBoxOfficeRequest

#include "stdafx.h"
#include "BoxOfficeRequest.h"
#include ".\boxofficerequest.h"
#include "IInputMessage.h"


// CBoxOfficeRequest


STDMETHODIMP CBoxOfficeRequest::Request(BSTR strXML)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

//	ASSERT(FALSE);
//	return E_PENDING;
	// TODO: Add your implementation code here

	//OutputDebugStringW( strXML );
	//OutputDebugStringW( L"\n" );
	//CComBSTR s(strXML);
	std::wstring s( SysStringLen( strXML ) ? strXML : L"" );
	return GetInputBuffer().PutIncomingMessage( s ) ? S_OK : E_FAIL;
}
