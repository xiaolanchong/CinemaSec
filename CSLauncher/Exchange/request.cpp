#include "StdAfx.h"
#include ".\request.h"
#include "..\Exchange\Exchange.h"

STDMETHODIMP CRequest::Request(BSTR strXML)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// TODO: Add your implementation code here
	return RequestExchange( strXML );
}
