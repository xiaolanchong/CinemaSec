// DllModule.cpp: implementation of the CDllModule class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "DllModule.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

CDllModule _Module;

//////////////////////////////////////////////////////////////////////
// 

CDllModule::CDllModule()
{
}

CDllModule::~CDllModule()
{
}

HRESULT CDllModule::RegisterServer(UINT nDataID, BOOL bRegTypeLib)
{
	HRESULT hr = CoInitialize(NULL);
	if(FAILED(hr))
		return hr;

	// Add data entries
	UpdateRegistryFromResource(nDataID, TRUE);
	
	// Add object entries
	hr = CComModule::RegisterServer(bRegTypeLib);
	
	CoUninitialize();
	return hr;
}

HRESULT CDllModule::UnregisterServer(UINT nDataID)
{
	HRESULT hr = CoInitialize(NULL);
	if(FAILED(hr))
		return hr;

	// Remove data entries
	UpdateRegistryFromResource(nDataID, TRUE);
	
	// Remove object entries
	CComModule::UnregisterServer(TRUE);
	
	CoUninitialize();
	return S_OK;
}

void CDllModule::Init(_ATL_OBJMAP_ENTRY* p,	HINSTANCE hInstance, const GUID* plibid)
{
	CComModule::Init(p, hInstance, plibid);
}
	
LONG CDllModule::Lock()
{
	return CComModule::Lock();
}

LONG CDllModule::Unlock()
{
	LONG lRefCount = CComModule::Unlock();

	if(lRefCount == 0)
	{
		DEBUG_ONLY(Elvees::Output(Elvees::TTrace, TEXT("Module chcsva unlocked")));

		// модуль будет выгружен..
	}

	return lRefCount;
}

CDllModule::Owner::Owner()
{
	_Module.Lock();
}

CDllModule::Owner::~Owner()
{
	_Module.Unlock();
}