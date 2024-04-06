#include "stdafx.h"
#include <stdio.h>
#include "../InterfaceEx.h"
#include <string.h>
#include <stdlib.h>
#include <exception>

//last header
#include "memleak.h"
// ------------------------------------------------------------------------------------ //
// InterfaceReg.
// ------------------------------------------------------------------------------------ //
InterfaceReg *InterfaceReg::s_pInterfaceRegs = NULL;


InterfaceReg::InterfaceReg( InstantiateInterfaceFn fn, const wchar_t *pName ) :
	m_pName(pName)
{
	m_CreateFn = fn;
	m_pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}



// ------------------------------------------------------------------------------------ //
// CreateInterface.
// ------------------------------------------------------------------------------------ //

DWORD WINAPI CreateInterface( const wchar_t *pName, void **ppInterface )
{
	InterfaceReg *pCur;

	if( !ppInterface/* || !*ppInterface*/ ) return IFACE_FAILED;
	
	for(pCur=InterfaceReg::s_pInterfaceRegs; pCur; pCur=pCur->m_pNext)
	{
		if(wcscmp(pCur->m_pName, pName) == 0)
		{
			try
			{
				// mb throw exceptions
				*ppInterface = pCur->m_CreateFn();
				return IFACE_OK;
			}
			catch(std::exception&)
			{
				break;
			}
		}
	}

	*ppInterface = NULL;
	return IFACE_FAILED;	
}


#ifdef _LINUX
// Linux doesn't have this function so this emulates its functionality
//
//
void *GetModuleHandle(const char *name)
{
        void *handle;


        if( name == NULL )
        {
                // hmm, how can this be handled under linux....
                // is it even needed?
                return NULL;
        }

        if( (handle=dlopen(name, RTLD_NOW))==NULL)
        {
                //printf("Error:%s\n",dlerror());
                // couldn't open this file
                return NULL;
        }

        // read "man dlopen" for details
        // in short dlopen() inc a ref count
        // so dec the ref count by performing the close
        dlclose(handle);
       return handle;
}

#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

//-----------------------------------------------------------------------------
// Purpose: returns a pointer to a function, given a module
// Input  : pModuleName - module name
//			*pName - proc name
//-----------------------------------------------------------------------------
static void *Sys_GetProcAddress( const wchar_t *pModuleName, const char *pName )
{
	return GetProcAddress( GetModuleHandleW(pModuleName), pName );
}

//-----------------------------------------------------------------------------
// Purpose: Loads a DLL/component from disk and returns a handle to it
// Input  : *pModuleName - filename of the component
// Output : opaque handle to the module (hides system dependency)
//-----------------------------------------------------------------------------
CSysModule	*Sys_LoadModule( const wchar_t *pModuleName )
{
	// If using the Steam filesystem, either the DLL must be a minimum footprint
	// file in the depot (MFP) or a filesystem GetLocalCopy() call must be made
	// prior to the call to this routine.
#ifdef _WIN32
	HMODULE hDLL = LoadLibraryW( pModuleName );
#elif _LINUX
	char szCwd[1024];
        char szAbsoluteModuleName[1024];

        getcwd( szCwd, sizeof( szCwd ) );
        if ( szCwd[ strlen( szCwd ) - 1 ] == '/' )
                szCwd[ strlen( szCwd ) - 1 ] = 0;

        _snprintf( szAbsoluteModuleName, sizeof(szAbsoluteModuleName),"%s/%s", szCwd, pModuleName );

        HMODULE hDLL = dlopen( szAbsoluteModuleName, RTLD_NOW );
#endif

	if( !hDLL )
	{
		wchar_t str[512];
#ifdef _WIN32
		_snwprintf( str, sizeof(str), L"%s.dll", pModuleName );
		hDLL = LoadLibraryW( str );
#elif _LINUX
		_snprintf( str, sizeof(str), "%s_486.so", szAbsoluteModuleName );
                hDLL = dlopen(str, RTLD_NOW);
#endif
// So you can see what the error is in the debugger...
#ifdef _DEBUG
		if ( !hDLL )
		{
			char *lpMsgBuf;
			
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);

			LocalFree( (HLOCAL)lpMsgBuf );
		}
#endif
	}

	return reinterpret_cast<CSysModule *>(hDLL);
}

//-----------------------------------------------------------------------------
// Purpose: Unloads a DLL/component from
// Input  : *pModuleName - filename of the component
// Output : opaque handle to the module (hides system dependency)
//-----------------------------------------------------------------------------
void Sys_UnloadModule( CSysModule *pModule )
{
	if ( !pModule )
		return;

	HMODULE	hDLL = reinterpret_cast<HMODULE>(pModule);

#ifdef _WIN32
	FreeLibrary( hDLL );
#elif _LINUX
	dlclose((void *)hDLL);
#endif
}

//-----------------------------------------------------------------------------
// Purpose: returns a pointer to a function, given a module
// Input  : module - windows HMODULE from Sys_LoadModule() 
//			*pName - proc name
// Output : factory for this module
//-----------------------------------------------------------------------------
CreateInterfaceFn Sys_GetFactory( CSysModule *pModule )
{
	if ( !pModule )
		return NULL;

	HMODULE	hDLL = reinterpret_cast<HMODULE>(pModule);
#ifdef _WIN32
	return reinterpret_cast<CreateInterfaceFn>(GetProcAddress( hDLL, CREATEINTERFACE_PROCNAME ));
#elif _LINUX
// Linux gives this error:
//../public/interface.cpp: In function `IBaseInterface *(*Sys_GetFactory
//(CSysModule *)) (const char *, int *)':
//../public/interface.cpp:154: ISO C++ forbids casting between
//pointer-to-function and pointer-to-object
//
// so lets get around it :)
        return (CreateInterfaceFn)(GetProcAddress( hDLL, CREATEINTERFACE_PROCNAME ));
#endif

}

//-----------------------------------------------------------------------------
// Purpose: returns the instance of this module
// Output : interface_instance_t
//-----------------------------------------------------------------------------
CreateInterfaceFn Sys_GetFactoryThis( void )
{
	return CreateInterface;
}

#ifdef USE_OBSOLETE_FUNCTION
//-----------------------------------------------------------------------------
// Purpose: returns the instance of the named module
// Input  : *pModuleName - name of the module
// Output : interface_instance_t - instance of that module
//-----------------------------------------------------------------------------
CreateInterfaceFn Sys_GetFactory( const char *pModuleName )
{
#ifdef _WIN32
	return static_cast<CreateInterfaceFn>( Sys_GetProcAddress( pModuleName, CREATEINTERFACE_PROCNAME ) );
#elif _LINUX
	// see Sys_GetFactory( CSysModule *pModule ) for an explanation
	return (CreateInterfaceFn)( Sys_GetProcAddress( pModuleName, CREATEINTERFACE_PROCNAME ) );
#endif
}
#endif // USE_OBSOLETE_FUNCTION



