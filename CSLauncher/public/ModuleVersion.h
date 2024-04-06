// ModuleVersion.h: interface for the CModuleVersion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODULEVERSION_H__6BF3B13B_B149_4196_96B5_91A2795331FC__INCLUDED_)
#define AFX_MODULEVERSION_H__6BF3B13B_B149_4196_96B5_91A2795331FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CModuleVersion : public VS_FIXEDFILEINFO {
protected:
   BYTE* m_pVersionInfo;   // all version info

   struct TRANSLATION {
      WORD langID;         // language ID
      WORD charset;        // character set (code page)
   } m_translation;

public:
   CModuleVersion();
   virtual ~CModuleVersion();

   BOOL     GetFileVersionInfo(LPCTSTR modulename);
   CString  GetValue(LPCTSTR lpKeyName);
};

CModuleVersion::CModuleVersion()
{
   m_pVersionInfo = NULL;           // raw version info data 
}

//////////////////
// Destroy: delete version info
//
CModuleVersion::~CModuleVersion()
{
   delete [] m_pVersionInfo;
}

//////////////////
// Get file version info for a given module
// Allocates storage for all info, fills "this" with
// VS_FIXEDFILEINFO, and sets codepage.
//
BOOL CModuleVersion::GetFileVersionInfo(LPCTSTR modulename)
{
   m_translation.charset = 1252;    // default = ANSI code page
   memset((VS_FIXEDFILEINFO*)this, 0, sizeof(VS_FIXEDFILEINFO));

   // get module handle
   TCHAR filename[_MAX_PATH];
   HMODULE hModule = ::GetModuleHandle(modulename);
   if (hModule==NULL && modulename!=NULL)
      return FALSE;

   // get module file name
   DWORD len = GetModuleFileName(hModule, filename,
      sizeof(filename)/sizeof(filename[0]));
   if (len <= 0)
      return FALSE;

   // read file version info
   DWORD dwDummyHandle; // will always be set to zero
   len = GetFileVersionInfoSize(filename, &dwDummyHandle);
   if (len <= 0)
      return FALSE;

   m_pVersionInfo = new BYTE[len]; // allocate version info
   if (!::GetFileVersionInfo(filename, 0, len, m_pVersionInfo))
      return FALSE;

   LPVOID lpvi;
   UINT iLen;
   if (!VerQueryValue(m_pVersionInfo, _T("\\"), &lpvi, &iLen))
      return FALSE;

   // copy fixed info to myself, which am derived from VS_FIXEDFILEINFO
   *(VS_FIXEDFILEINFO*)this = *(VS_FIXEDFILEINFO*)lpvi;

   // Get translation info
   if (VerQueryValue(m_pVersionInfo,
      _T("\\VarFileInfo\\Translation"), &lpvi, &iLen) && iLen >= 4) {
      m_translation = *(TRANSLATION*)lpvi;
      TRACE(_T("code page = %d\n"), m_translation.charset);
   }

   return dwSignature == VS_FFI_SIGNATURE;
}

//////////////////
// Get string file info.
// Key name is something like "CompanyName".
// returns the value as a CString.
//
CString CModuleVersion::GetValue(LPCTSTR lpKeyName)
{
   CString sVal;
   if (m_pVersionInfo) {

      // To get a string value must pass query in the form
      //
      //    "\StringFileInfo\<langID><codepage>\keyname"
      //
      // where <langID><codepage> is the languageID concatenated with the
      // code page, in hex. Wow.
      //
      CString query;
      query.Format(_T("\\StringFileInfo\\%04x%04x\\%s"),
                   m_translation.langID,
                   m_translation.charset,
                   lpKeyName);

      LPCTSTR pVal;
      UINT iLenVal;
      if (VerQueryValue(m_pVersionInfo, (LPTSTR)(LPCTSTR)query,
          (LPVOID*)&pVal, &iLenVal)) {

         sVal = pVal;
      }
   }
   return sVal;
}

#endif // !defined(AFX_MODULEVERSION_H__6BF3B13B_B149_4196_96B5_91A2795331FC__INCLUDED_)
