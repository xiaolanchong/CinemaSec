#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN    // Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER          // Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0500   // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0500 // Change this to the appropriate value to target Windows 2000 or later.
#endif            

#ifndef _WIN32_WINDOWS        // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0500 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE         // Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400  // Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <commctrl.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>      // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>     // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>   // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <mmsystem.h>
#include <afxmt.h>
#include <conio.h>
#include <direct.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <shlwapi.h> 

#include "../../CSUtility/csutility.h"
#include "../../CSInterfaces/common_interface_settings.h"
#include "../../CSInterfaces/info_query_interface.h"
#include "../../CSInterfaces/debug_interface.h"
#include "../../CSInterfaces/data_transfer_types.h"
#include "../../CSInterfaces/learning_algo_interface.h"
#include "../interfaces/camera_analyzer_interface.h"
#include "../interfaces/hall_analyzer_interface.h"
#include "../interfaces/background_accum_interface.h"
#include "../interfaces/parameter_dialog.h" 
#include "../../CSLearningAlgo/CSOnlineWriter/CSOnlineWriter.h"

#include "CSAlgo1.h"
#include "algorithm/algo_types.h"
#include "algorithm/algo_data_transfer_types.h"
#include "CSAlgo1Dll.h"
