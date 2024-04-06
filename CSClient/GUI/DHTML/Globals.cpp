// Globals.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
// Global Helper Functions & Macros
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include <mshtmdid.h>		// Standard IE/MSHTML DISPID Definitions
#include <idispids.h>		// More IE/MSHTML DISPID Definitions
#include <exdispid.h>		// DISPIDs for IExplorer Events

#include "Globals.h"


/////////////////////////////////////////////////////////////////////////////
// Helper for reliable and small Release calls
// Taken from OLEIMPL2.H & OLEUNK.CPP
/////////////////////////////////////////////////////////////////////////////

DWORD AFXAPI _AfxRelease(LPUNKNOWN* plpUnknown)
{
	ASSERT(plpUnknown != NULL);
	if (*plpUnknown != NULL)
	{
		DWORD dwRef = (*plpUnknown)->Release();
		*plpUnknown = NULL;
		return dwRef;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Diagnostic helper to discover what properties MSHTML 
// asks of the host view
//
// Originally a debug helper for OLE Controls
// If you want to add more... see "OleCtl.h"
/////////////////////////////////////////////////////////////////////////////

#ifdef _TRACE_AMB_PROPRERIES
CString StringDISPID(DISPID dispidMember) 
{ 
	CString strReturn;
	 
	typedef struct { 
		DISPID dispid; 
		LPCTSTR szDesc; 
	} DISPIDDESC; 

	#define MAKE_DISPIDDESC_ENTRY(dispid) { dispid, _T(#dispid)}

	static DISPIDDESC aDISPIDS[] = {
		//  Standard dispatch ID constants
		MAKE_DISPIDDESC_ENTRY(DISPID_READYSTATE),											// (-525)
		// ... big chunk removed !
		MAKE_DISPIDDESC_ENTRY(DISPID_CLICK),													// (-600)
		MAKE_DISPIDDESC_ENTRY(DISPID_DBLCLICK),												// (-601)
		MAKE_DISPIDDESC_ENTRY(DISPID_KEYDOWN),												// (-602)
		MAKE_DISPIDDESC_ENTRY(DISPID_KEYPRESS),												// (-603)
		MAKE_DISPIDDESC_ENTRY(DISPID_KEYUP),													// (-604)
		MAKE_DISPIDDESC_ENTRY(DISPID_MOUSEDOWN),											// (-605)
		MAKE_DISPIDDESC_ENTRY(DISPID_MOUSEMOVE),											// (-606)
		MAKE_DISPIDDESC_ENTRY(DISPID_MOUSEUP),												// (-607)
		MAKE_DISPIDDESC_ENTRY(DISPID_ERROREVENT),											// (-608)
		MAKE_DISPIDDESC_ENTRY(DISPID_READYSTATECHANGE),								// (-609)
		MAKE_DISPIDDESC_ENTRY(DISPID_CLICK_VALUE),										// (-610)
		MAKE_DISPIDDESC_ENTRY(DISPID_RIGHTTOLEFT),										// (-611)
		MAKE_DISPIDDESC_ENTRY(DISPID_TOPTOBOTTOM),										// (-612)
		MAKE_DISPIDDESC_ENTRY(DISPID_THIS),														// (-613)

		// Standard Ambient Properties
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_BACKCOLOR),  						// (-701) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_DISPLAYNAME),  					// (-702) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_FONT),  									// (-703) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_FORECOLOR),  						// (-704) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_LOCALEID),  							// (-705) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_MESSAGEREFLECT),  				// (-706) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_SCALEUNITS),  						// (-707) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_TEXTALIGN),  						// (-708) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_USERMODE),  							// (-709) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_UIDEAD),  								// (-710) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_SHOWGRABHANDLES),  			// (-711) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_SHOWHATCHING),  					// (-712) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_DISPLAYASDEFAULT),  			// (-713) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_SUPPORTSMNEMONICS), 			// (-714) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_AUTOCLIP),  							// (-715) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_APPEARANCE),  						// (-716)
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_CODEPAGE),								// (-725)
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_PALETTE),  							// (-726) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_CHARSET),								// (-727)
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_TRANSFERPRIORITY),  			// (-728) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_RIGHTTOLEFT),  					// (-732) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_TOPTOBOTTOM),  					// (-733) 

		// IE4/MSHTML Specific Ambient Properties
		MAKE_DISPIDDESC_ENTRY(DISPID_WINDOWOBJECT),  									// (-5500) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_OFFLINEIFNOTCONNECTED),  // (-5501) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_SILENT),  								// (-5502) 
		MAKE_DISPIDDESC_ENTRY(DISPID_LOCATIONOBJECT),  								// (-5506) 
		MAKE_DISPIDDESC_ENTRY(DISPID_HISTORYOBJECT),  								// (-5507) 
		MAKE_DISPIDDESC_ENTRY(DISPID_NAVIGATOROBJECT), 								// (-5508) 
		MAKE_DISPIDDESC_ENTRY(DISPID_SECURITYCTX), 										// (-5511) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_DLCONTROL), 							// (-5512) 
		MAKE_DISPIDDESC_ENTRY(DISPID_AMBIENT_USERAGENT), 							// (-5513) 
		MAKE_DISPIDDESC_ENTRY(DISPID_SECURITYDOMAIN),									// (-5514)

		// Only here to finish off the list...
		MAKE_DISPIDDESC_ENTRY(DISPID_UNKNOWN)													// (-1)
	}; 
	 
	for (int i = 0; i < _countof(aDISPIDS); i++) 
	{ 
		if (dispidMember == aDISPIDS[i].dispid) 
		{ // If we find it in the table, return it
			strReturn.Format("%s", aDISPIDS[i].szDesc); 
			return strReturn; 
		} 
	} 

	// Otherwise, kick back the number as a string (Hex/Dec)
	strReturn.Format("UNKNOWN $%X (%d)", dispidMember, dispidMember);
	return strReturn; 

	#undef MAKE_DISPIDDESC_ENTRY
}
#endif // _TRACE_AMB_PROPRERIES

//////////////////////////////////////////////////////////////////////
// UNICODE <-> ANSI Helper functions (Globals)
//////////////////////////////////////////////////////////////////////

#ifndef UNICODE 
char* ConvertToAnsi(OLECHAR FAR* szW) 
{ 
  static char achA[STRCONVERT_MAXLEN];  
   
  WideCharToMultiByte(CP_ACP, 0, szW, -1, achA, STRCONVERT_MAXLEN, NULL, NULL);   
  return achA;  
}  
 
OLECHAR* ConvertToUnicode(char FAR* szA) 
{ 
  static OLECHAR achW[STRCONVERT_MAXLEN];  
 
  MultiByteToWideChar(CP_ACP, 0, szA, -1, achW, STRCONVERT_MAXLEN);
  return achW;
} 
#endif 

CString IID_DbgName(REFIID iid)
{
	CString s;

	LPOLESTR lp=NULL;
	StringFromCLSID(iid, &lp);
	s = lp;

	return s;
}

/////////////////////////////////////////////////////////////////////////////
// END
/////////////////////////////////////////////////////////////////////////////

