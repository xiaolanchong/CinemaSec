// BoxOfficeRequest.h : Declaration of the CBoxOfficeRequest

#pragma once
#include "resource.h"       // main symbols

#include "CSExchangeTest.h"

#include "..\CSEngine\Exchange\dual.h"

// CBoxOfficeRequest

class ATL_NO_VTABLE CBoxOfficeRequest : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CBoxOfficeRequest, &CLSID_BoxOfficeRequest>
#ifdef DUAL_BOXOFFICE
	,public IDispatchImpl<IBoxOfficeRequest, &IID_IBoxOfficeRequest, &LIBID_CSExchangeTestLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
#endif
{
public:
	CBoxOfficeRequest()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_BOXOFFICEREQUEST)


BEGIN_COM_MAP(CBoxOfficeRequest)
	COM_INTERFACE_ENTRY(IBoxOfficeRequest)
#ifdef DUAL_BOXOFFICE
	COM_INTERFACE_ENTRY(IDispatch)
#endif
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:

	STDMETHOD(Request)(BSTR strXML);
};

OBJECT_ENTRY_AUTO(__uuidof(BoxOfficeRequest), CBoxOfficeRequest)
