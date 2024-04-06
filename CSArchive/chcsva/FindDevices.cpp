// FindDevices.cpp
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <streams.h>

#include <Rpcdce.h>
#pragma comment(lib, "Rpcrt4.lib")

#ifdef UNICODE
typedef	WCHAR* UuidString;
#else
typedef	UCHAR* UuidString;
#endif

void FindDevices()
{
    HRESULT hr;
    ULONG cFetched;

	CComPtr<ICreateDevEnum> pDevEnum = NULL;
	CComPtr<IEnumMoniker> pClassEnum = NULL;
	CComPtr<IMalloc> pMalloc = NULL;

	IMoniker* pMoniker = NULL;

	do
	{
		// Create the system device enumerator
		hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
			IID_ICreateDevEnum, (void **)&pDevEnum);
		if(FAILED(hr))
		{
			OutputDebugString(TEXT("Fail to create system device enumerator\n"));
			break;
		}
		
		// Create an enumerator for the video compressors
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
		if(FAILED(hr))
		{
			OutputDebugString(TEXT("Fail to create an enumerator for the video input devices\n"));
			break;
		}
    
		// If there are no enumerators for the requested type, then 
		// CreateClassEnumerator will succeed, but pClassEnum will be NULL.
		if(pClassEnum == NULL)
		{
			OutputDebugString(TEXT("There are no enumerator for the video input devices\n"));
			break;
		}
	
		CoGetMalloc(1, &pMalloc);

		bool bInited;
					
		TCHAR stUuidName[MAX_PATH];
		TCHAR stDisplayName[MAX_PATH];
		TCHAR stFriendlyName[MAX_PATH];

		while(S_OK == (pClassEnum->Next(1, &pMoniker, &cFetched)))
		{
			IPropertyBag *pBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pBag);
			if(SUCCEEDED(hr))
			{
				VARIANT varName;
				VariantInit(&varName);
				
				hr = pBag->Read(L"FriendlyName", &varName, NULL);
				if(SUCCEEDED(hr))
				{
					bInited = false;
					wsprintf(stFriendlyName, TEXT("%ls"), varName.bstrVal);

					do
					{
						UUID uuidName;
						RPC_STATUS lStatus = UuidCreate(&uuidName);

						if(lStatus == RPC_S_OK ||
							lStatus == RPC_S_UUID_LOCAL_ONLY ||
							lStatus == RPC_S_UUID_NO_ADDRESS)
						{
							UuidString pszUuid = 0;
							
							if(UuidToString(&uuidName, &pszUuid) == RPC_S_OK && pszUuid)
							{
								wsprintf(stUuidName, TEXT("{%s}"), (LPCTSTR)pszUuid);
								RpcStringFree(&pszUuid);
							}
							else
							{
								OutputDebugString(TEXT("UuidToString failed\n"));
								break;
							}
						}
						else
						{
							OutputDebugString(TEXT("Fail to create new UUID\n"));
							break;
						}

						//-------------------------------------

						BSTR szDisplayName = NULL;
						hr = pMoniker->GetDisplayName(NULL, NULL, &szDisplayName);

						if(SUCCEEDED(hr) && szDisplayName)
						{
							wsprintf(stDisplayName, TEXT("%ls"), szDisplayName);
							pMalloc->Free(szDisplayName);
						}
						else
						{
							OutputDebugString(TEXT("Fail to get display name\n"));
							break;
						}

						bInited = true;
					}
					while(false);

					if(bInited)
					{
						CRegKey keyDevice;

						keyDevice.Open(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Elvees\\CHCS\\Devices"));
						keyDevice.SetKeyValue(stUuidName, stFriendlyName);

						CRegKey keyUUID;

						keyUUID.Open(keyDevice.m_hKey, stUuidName);

					#if _MSC_VER >= 1020
						keyUUID.SetDWORDValue(TEXT("Width"), (DWORD)352);
						keyUUID.SetDWORDValue(TEXT("Height"), (DWORD)288);
						keyUUID.SetDWORDValue(TEXT("Input"), (DWORD)2);
						keyUUID.SetDWORDValue(TEXT("BWMode"), (DWORD)0);
						keyUUID.SetDWORDValue(TEXT("Standart"), (DWORD)16);

						keyUUID.SetStringValue(TEXT("DisplayName"), stDisplayName);
					#else
						keyUUID.SetValue((DWORD)352 ,TEXT("Width"));
						keyUUID.SetValue((DWORD)288 ,TEXT("Height"));
						keyUUID.SetValue((DWORD)2   ,TEXT("Input"));
						keyUUID.SetValue((DWORD)0   ,TEXT("BWMode"));
						keyUUID.SetValue((DWORD)16 ,TEXT("Standart"));

						keyUUID.SetValue(stDisplayName  ,TEXT("DisplayName"));
					#endif
					}

					SysFreeString(varName.bstrVal);
				}
				else
				{
					OutputDebugString(TEXT("Fail to get device FriendlyName\n"));
				}

				pBag->Release();
			}
			if(pMoniker)
			{
				pMoniker->Release();
				pMoniker = NULL;
			}
		}
	}
	while(0);
}

//  AnalogVideo_None	= 0,
//	AnalogVideo_NTSC_M	= 0x1,
//	AnalogVideo_NTSC_M_J= 0x2,
//	AnalogVideo_NTSC_433= 0x4,
//	AnalogVideo_PAL_B	= 0x10,
//	AnalogVideo_PAL_D	= 0x20,
//	AnalogVideo_PAL_G	= 0x40,
//	AnalogVideo_PAL_H	= 0x80,
//	AnalogVideo_PAL_I	= 0x100,
//	AnalogVideo_PAL_M	= 0x200,
//	AnalogVideo_PAL_N	= 0x400,
//	AnalogVideo_PAL_60	= 0x800,
//	AnalogVideo_SECAM_B	= 0x1000,
//	AnalogVideo_SECAM_D	= 0x2000,
//	AnalogVideo_SECAM_G	= 0x4000,
//	AnalogVideo_SECAM_H	= 0x8000,
//	AnalogVideo_SECAM_K	= 0x10000,
//	AnalogVideo_SECAM_K1= 0x20000,
//	AnalogVideo_SECAM_L	= 0x40000,
//	AnalogVideo_SECAM_L1= 0x80000,
//	AnalogVideo_PAL_N_COMBO	= 0x100000

