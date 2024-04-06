// exchange\Request.h : Declaration of the CRequest

#pragma once
#include "../res/resource.h"       // main symbols

#include "../CSLauncher.h"
#include "../../CSEngine/Exchange/dual.h"

// CRequest

class ATL_NO_VTABLE CRequest : 
	public CComObjectRootEx<CComMultiThreadModel >,
	public CComCoClass<CRequest, &CLSID_CCSRequest>
#ifdef DUAL_CCS
	,public IDispatchImpl<ICCSRequest, &IID_ICCSRequest, &LIBID_CSLauncherLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
#endif
{
public:
	CRequest()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_REQUEST)


BEGIN_COM_MAP(CRequest)
	COM_INTERFACE_ENTRY(ICCSRequest)
#ifdef DUAL_CCS
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

OBJECT_ENTRY_AUTO(__uuidof(CCSRequest), CRequest)
