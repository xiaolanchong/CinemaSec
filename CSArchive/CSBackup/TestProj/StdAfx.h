#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlhost.h>
#include <atldbcli.h>

// Windows Header Files:
#include <windows.h>
#include <shlwapi.h>

// Boost support
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>
#include <boost/bind.hpp>


//Just shutup the cl.exe
#pragma warning(disable : 4267 4244)
#include <boost/format.hpp>
#pragma warning(default : 4267 4244)

//STL support
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>