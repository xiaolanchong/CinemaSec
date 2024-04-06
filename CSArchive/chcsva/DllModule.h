// CHCSModule.h: interface for the CDllModule class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEES_CHCS_DLLMODULE_H__INCLUDED_
#define ELVEES_CHCS_DLLMODULE_H__INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

// вынесенно в отдельный файл для удобства
// подключать нужно stdafx.h
//

//////////////////////////////////////////////////////////////////////
// CDllModule
//////////////////////////////////////////////////////////////////////

class CDllModule : public CComModule
{
public:
	class Owner
	{
	public:
		Owner();
		~Owner();

	private:
		// No copies do not implement
		Owner(const Owner &rhs);
		Owner &operator=(const Owner &rhs);
	};

	CDllModule();
	virtual ~CDllModule();

	HRESULT RegisterServer(UINT nDataID, BOOL bRegTypeLib = FALSE);
	HRESULT UnregisterServer(UINT nDataID);

	void Init(_ATL_OBJMAP_ENTRY* p,
		HINSTANCE hInstance,
		const GUID* plibid = NULL);
	
	LONG Lock();   // Increments the module lock count
	LONG Unlock(); // Decrements the module lock count

private:

	// No copies do not implement
	CDllModule(const CDllModule &rhs);
	CDllModule &operator=(const CDllModule &rhs);	
};

//////////////////////////////////////////////////////////////////////
// CDllModule global object
//////////////////////////////////////////////////////////////////////

extern CDllModule _Module;

#endif // ELVEES_CHCS_DLLMODULE_H__INCLUDED_
