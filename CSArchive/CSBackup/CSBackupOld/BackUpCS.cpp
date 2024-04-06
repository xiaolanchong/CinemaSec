// BackUpCS.cpp: implementation of the CBackUpCS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\backupcs.h"

#include "Cameras.h"
#include "backup_interface.h"
#include "../../CSUtility/utility/debug_interface.h"

#include <strsafe.h>
#include <process.h>

//////////////////////////////////////////////////////////////////////////
// CBackUpCS
//////////////////////////////////////////////////////////////////////////

CBackUpCS::CBackUpCS(IDebugOutput* apDebugOutput)
{
  /////////////////////////////////////////////////////////////////////
  /////////////Создание сообщения об остановке
  /////////////////////////////////////////////////////////////////////
  m_pDebugOutput=apDebugOutput;
  m_lpArchive=NULL;
  m_lRoomIDNum=-1;
  m_plRoomIDs = NULL;
  m_lCinemaID=-1;
  m_lDeleteCount=0;
  m_lCameraAmount=0;
  m_plCameraIDs=NULL;

  m_hThread=NULL;
  m_ulStartPer=0;
  m_ulStopPer=0;
  m_daLast=0;
  
//  TCHAR stTemp[5000];
  HRESULT hr=S_OK;
  
  m_heStop=CreateEvent(NULL,FALSE,FALSE,NULL);
  if(INVALID_HANDLE_VALUE==m_heStop)
  {
    m_pDebugOutput->PrintW(IDebugOutput::mt_error, _T("Не смог создать управление циклическим архивом!"));
  }
  InitializeCriticalSection(&m_csdbLast);
  InitializeCriticalSection(&m_csdwStart);
  InitializeCriticalSection(&m_csdwStop);
  m_pDebugOutput->PrintW(IDebugOutput::mt_info, _T("Создано управление циклическим архивом."));
  hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
  //if (FAILED(hr))
  //{
  //  m_pDebugOutput->PrintW(IDebugOutput::mt_error, _T("Не могу инициализировать COM!"));
  //  m_hrLast=hr;
  //}
  //else
  //{
    hr = Create();
    //if (FAILED(hr))
    //{
    //  _stprintf(stTemp, _T("Ошибка при получении параметров архивирования. HR=%08X"), hr); 
    //  m_pDebugOutput->PrintW(IDebugOutput::mt_error, stTemp);
    //}
    //else
    //{
    //  GetChanges(); 
    //}
  //}
}

CBackUpCS::~CBackUpCS()
{
  if(m_plRoomIDs)
    delete []m_plRoomIDs;

  if(m_plCameraIDs)
    delete []m_plCameraIDs;

  if(m_lpArchive)
	delete []m_lpArchive;

  DeleteCriticalSection(&m_csdbLast);
  DeleteCriticalSection(&m_csdwStart);
  DeleteCriticalSection(&m_csdwStop);
  //if (!FAILED(m_hrLast))
  //{
  //  CoUninitialize();
  //}
}
HRESULT CBackUpCS::Create()
{

  if (m_hThread)
  {
    m_pDebugOutput->PrintW(IDebugOutput::mt_error,_T("Уже существует рабочий поток"));
    return E_FAIL;
  }
  m_hThread = (HANDLE)_beginthreadex(NULL, CREATE_SUSPENDED,
    &ThreadMain, this, 0, reinterpret_cast<unsigned* >(&m_dwThreadID));
  if(INVALID_HANDLE_VALUE == m_hThread)
  {
    m_pDebugOutput->PrintW(IDebugOutput::mt_error,_T("Не создан рабочий поток"));
    return E_FAIL;
  }
  if(!SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL))
  {
    TerminateThread(m_hThread, 1);
    m_pDebugOutput->PrintW(IDebugOutput::mt_error,_T("Не установлен приоритет рабочего потока"));
    return E_FAIL;
  }
  if(0xFFFFFFFF == ResumeThread(m_hThread))
  {
    TerminateThread(m_hThread, 1);
    m_pDebugOutput->PrintW(IDebugOutput::mt_error,_T("Рабочий поток не стартовал"));
    return E_FAIL;
  }
  return S_OK;
}

// Checking free disk space
bool CBackUpCS::CheckDiskSpace(void)
{
  __int64 i64FreeBytesToCaller;
  __int64 i64TotalBytes;
  __int64 i64FreeBytes;
//  long i = 0;
  BOOL finish=FALSE;
  TCHAR tcTemp[5000];

  if(!GetDiskFreeSpaceEx(m_lpArchive, (PULARGE_INTEGER)&i64FreeBytesToCaller,
    (PULARGE_INTEGER)&i64TotalBytes, (PULARGE_INTEGER)&i64FreeBytes))
  {
    DWORD lError=GetLastError();
    if((lError==ERROR_FILE_NOT_FOUND)||(lError==ERROR_SHARING_VIOLATION))
    {
      _stprintf(tcTemp, _T("Не могу получить размер свободного места на диске: %s. Ошибка:0x%08X"),
        m_lpArchive, lError);
      m_pDebugOutput->PrintW(IDebugOutput::mt_error, tcTemp);
    }
  }
  while((!GetDiskFreeSpaceEx(m_lpArchive,
    (PULARGE_INTEGER)&i64FreeBytesToCaller,
    (PULARGE_INTEGER)&i64TotalBytes, (PULARGE_INTEGER)&i64FreeBytes))
    &&(WaitForSingleObject(m_heStop, 0)!=WAIT_OBJECT_0))
  {
    Sleep(DOLBTIME/4);
  }
  if((i64FreeBytes/(double)i64TotalBytes)<(m_ulStartPer/(double)10000))
  {
    while((!finish)&&(WaitForSingleObject(m_heStop, 0)!=WAIT_OBJECT_0))
    {
      FreeDiskForPer();
      if(!GetDiskFreeSpaceEx(m_lpArchive, (PULARGE_INTEGER)&i64FreeBytesToCaller,
        (PULARGE_INTEGER)&i64TotalBytes, (PULARGE_INTEGER)&i64FreeBytes))
      {
        DWORD lError=GetLastError();
        if((lError==ERROR_FILE_NOT_FOUND)||(lError==ERROR_SHARING_VIOLATION))
        {
          _stprintf(tcTemp, _T("Не могу получить размер свободного места на диске: %s. Ошибка:0x%08X"),
            m_lpArchive, lError);
           m_pDebugOutput->PrintW(IDebugOutput::mt_error, tcTemp);
        }
      }
      while((!GetDiskFreeSpaceEx(m_lpArchive,
        (PULARGE_INTEGER)&i64FreeBytesToCaller,
        (PULARGE_INTEGER)&i64TotalBytes, (PULARGE_INTEGER)&i64FreeBytes))
        &&(WaitForSingleObject(m_heStop, 0)!=WAIT_OBJECT_0))
      {
        Sleep(DOLBTIME/4);
      }
      if((i64FreeBytes/(double)i64TotalBytes)>(m_ulStopPer/(double)10000))
      {
        finish=TRUE;
      }
    }
  }
  return true;
}

// Check for latest folder is up to time
bool CBackUpCS::CheckLastFolder(void)
{
  WIN32_FIND_DATA FindFileData;
  WIN32_FIND_DATA FindFileDataIn;
  HANDLE hFind;
  HANDLE hFindIn;
  LONG i=0;
  FILETIME ftEarliest;
  DATE dbLAST=m_daLast;
  if(!OTToFT(dbLAST, ftEarliest))
  {
    return false;
  }

  TCHAR tcTemp[5000];

  DWORD dwError;
  BOOL bEndFind = FALSE;
  TCHAR pTempName[MAX_PATH+1];
  //TCHAR pFileName[MAX_PATH+1];
  TCHAR pDirName[MAX_PATH+1];
  TCHAR pDeleteName[MAX_PATH+1];


  ZeroMemory(pDeleteName,sizeof(TCHAR)*(MAX_PATH+1));
  CheckForSlash(m_lpArchive);
  ZeroMemory(pTempName,sizeof(TCHAR)*(MAX_PATH+1));
  ZeroMemory(pDirName,sizeof(TCHAR)*(MAX_PATH+1));
  _stprintf(pTempName, _T("%s*.*"), m_lpArchive);
  for (i=0; i<m_lCameraAmount; i++)
  {
    _stprintf(pTempName, _T("%s%d\\*.*"), m_lpArchive, m_plCameraIDs[i]);
    hFind = FindFirstFile(pTempName, &FindFileData);

    bEndFind=false;
    if(INVALID_HANDLE_VALUE == hFind)
    {
    }
    else
    {
      WHILEM((FindNextFile(hFind,&FindFileData))&(!bEndFind))
      {
        if(FindFileData.cFileName[0]==_T('.'))
        {
          continue;
        }
        else
        {
          ZeroMemory(pDirName,sizeof(TCHAR)*(MAX_PATH+1));
          _stprintf(pDirName, _T("%s%d\\%s\\*.avi"), m_lpArchive, m_plCameraIDs[i], FindFileData.cFileName);

          ZeroMemory(pTempName,sizeof(TCHAR)*(MAX_PATH+1));
          _stprintf(pTempName, _T("%s%d\\%s"), m_lpArchive, m_plCameraIDs[i], FindFileData.cFileName);

          hFindIn = FindFirstFile(pDirName, &FindFileDataIn);

          if(INVALID_HANDLE_VALUE == hFindIn)
          {
            if(!RemoveNEDir(pTempName), _T("avi"))
            {
              _stprintf(tcTemp, _T("Не смог удалить директорию:%s."), pTempName);
              m_pDebugOutput->PrintW(IDebugOutput::mt_error,tcTemp);
              break;
            }
          }
          else
          {
            do
            {
              if(FindFileDataIn.cFileName[0]==_T('.'))
              {
                continue;
              }
              if(CompareFileTime(&ftEarliest,&(FindFileDataIn.ftCreationTime))==1)
              {
                ZeroMemory(pDeleteName,sizeof(TCHAR)*(MAX_PATH+1));
                _stprintf(pDeleteName,_T("%s\\%s"), pTempName, FindFileDataIn.cFileName);
//                ftEarliest=FindFileDataIn.ftLastWriteTime;
                if(pDeleteName)
                {
                  if(_tcslen(pDeleteName))
                  {
                    if(!DeleteFile(pDeleteName))
                    {
                      dwError=GetLastError();
                      _stprintf(tcTemp, _T("Не смог удалить файл:%s. ошибка: 0x%08X"), pDeleteName, dwError);
                      m_pDebugOutput->PrintW(IDebugOutput::mt_error,tcTemp);
                    }
                    if(m_lDeleteCount<FILESCOUNT)
                    {
                      m_lDeleteCount++;
                    }
                    else
                    {
                      if(FTToOT(ftEarliest, dbLAST))
                      {
                        if(FreeBaseUTD(dbLAST))
                        {
                          m_lDeleteCount=0;
                        }
                      }
                    }
                  }
                }
              }
              else
              {
                bEndFind=true;
              }
            }
            WHILEM((FindNextFile(hFindIn,&FindFileDataIn))&(!bEndFind));
            if(!FindClose(hFindIn))
            {
              dwError=GetLastError();
              if((dwError==ERROR_FILE_NOT_FOUND)||(dwError==ERROR_SHARING_VIOLATION))
              {
              }
            }
            if(!bEndFind)
            {
              if(!RemoveNEDir(pTempName), _T("avi"))
              {
                _stprintf(tcTemp, _T("Не смог удалить директорию:%s."), pTempName);
                m_pDebugOutput->PrintW(IDebugOutput::mt_error,tcTemp);
                break;
              }
            }
          }
        }
      }
      dwError=GetLastError();
      if((ERROR_NO_MORE_FILES != dwError)&&(ERROR_SUCCESS!=dwError)&&(ERROR_FILE_NOT_FOUND!=dwError)&&(!bEndFind))
      {
        _stprintf(tcTemp, _T("Не могу найти файлы поданному пути: %s. Ошибка:0x%08X"),
          pTempName, dwError);
        m_pDebugOutput->PrintW(IDebugOutput::mt_error, tcTemp);
      }
      if(!FindClose(hFind))
      {
        dwError=GetLastError();
        if((dwError==ERROR_FILE_NOT_FOUND)||(dwError==ERROR_SHARING_VIOLATION))
        {
        }
      }
    }
  }
//  WIN32_FIND_DATA FindFileData;
//  WIN32_FIND_DATA FindFileDataIn;
//  HANDLE hFind;
//  HANDLE hFindIn;
////  LONG i=0;
//  DATE dbLAST=m_daLast;
////  bool bRet=true;
//  DWORD dwError;
//  BOOL bEndFind = FALSE;
//  TCHAR pTempName[MAX_PATH+1];
//  //TCHAR pFileName[MAX_PATH+1];
//  TCHAR pDirName[MAX_PATH+1];
//  TCHAR pDeleteName[MAX_PATH+1];
//  ZeroMemory(pDeleteName,sizeof(TCHAR)*(MAX_PATH+1));
//  CheckForSlash(m_lpArchive);
//  ZeroMemory(pTempName,sizeof(TCHAR)*(MAX_PATH+1));
//  ZeroMemory(pDirName,sizeof(TCHAR)*(MAX_PATH+1));
//  _stprintf(pTempName, _T("%s*.*"), m_lpArchive);
//  hFind = FindFirstFile(pTempName, &FindFileData);
//
//  if(INVALID_HANDLE_VALUE == hFind)
//  {
//  }
//  else
//  {
//    WHILEM((FindNextFile(hFind,&FindFileData))&(!bEndFind))
//    {
//      if(!pDeleteName)
//      {
//        if(_tcslen(pDeleteName))
//        {
//          if(!RemoveNEDir(pTempName))
//          {
//          }
//        }
//      }
//      if(FindFileData.cFileName[0]==_T('.'))
//      {
//        continue;
//      }
//      else
//      {
//        ZeroMemory(pDirName,sizeof(TCHAR)*(MAX_PATH+1));
//        _stprintf(pDirName, _T("%s%s\\*.*"), m_lpArchive, FindFileData.cFileName);
//        ZeroMemory(pTempName,sizeof(TCHAR)*(MAX_PATH+1));
//        _stprintf(pTempName, _T("%s%s"), m_lpArchive, FindFileData.cFileName);
//        hFindIn = FindFirstFile(pDirName, &FindFileDataIn);
//
//        if(INVALID_HANDLE_VALUE == hFindIn)
//        {
//        }
//        else
//        {
//          WHILEM((FindNextFile(hFindIn,&FindFileDataIn))&(!bEndFind))
//          {
//            if(FindFileDataIn.cFileName[0]==_T('.'))
//            {
//              continue;
//            }
//            if(!pDeleteName)
//            {
//              if(_tcslen(pDeleteName))
//              {
//                if(!RemoveNEDir(pDeleteName))
//                {
//                  SetLastResult(E_FAIL);
//                }
//                if(!FreeBaseUTD(dbLAST))
//                {
//                }
//              }
//            }
//            if(CheckFileTime(FindFileDataIn.ftLastWriteTime))
//            {
//              if(!FTToOT(FindFileDataIn.ftLastWriteTime, dbLAST))
//              {
//                if(!FindClose(hFindIn))
//                {
//                  dwError=GetLastError();
//                  if((dwError==ERROR_FILE_NOT_FOUND)||(dwError==ERROR_SHARING_VIOLATION))
//                  {
//                  }
//                }
//                break;
//              }
//              ZeroMemory(pDeleteName,sizeof(TCHAR)*(MAX_PATH+1));
//              _stprintf(pDeleteName,_T("%s\\%s"), pTempName, FindFileDataIn.cFileName);
//            }
//            else
//            {
//              bEndFind = TRUE;
//            }
//          }
//          if(!FindClose(hFindIn))
//          {
//            dwError=GetLastError();
//            if((dwError==ERROR_FILE_NOT_FOUND)||(dwError==ERROR_SHARING_VIOLATION))
//            {
//            }
//          }
//          if(!bEndFind)
//          {
//            if(!RemoveNEDir(pDeleteName))
//            {
//              SetLastResult(E_FAIL);
//            }
//            if(!FreeBaseUTD(dbLAST))
//            {
//            }
//          }
//        }
//      }
//    }
//    dwError=GetLastError();
//    if(ERROR_NO_MORE_FILES != dwError)
//    {
/////      return true;
//    }
//    if(!FindClose(hFind))
//    {
//      return false;
//    }
//    if(!pDeleteName)
//    {
//      if(_tcslen(pDeleteName))
//      {
//        if(!RemoveNEDir(pTempName))
//        {
//        }
//      }
//    }
//    bEndFind = TRUE;
//  }
  return true;
}

// Delete all files up to date
HRESULT CBackUpCS::FreeDiskUpToDate(void)
{
 
  return E_NOTIMPL;
}

// Free disk for need space in percentage
HRESULT CBackUpCS::FreeDiskForPer(void)
{
  WIN32_FIND_DATA FindFileData;
  WIN32_FIND_DATA FindFileDataIn;
  HANDLE hFind;
  HANDLE hFindIn;
  LONG i=0;
  FILETIME ftEarliest;
  DATE dbLAST=m_daLast;
  GetSystemTimeAsFileTime((LPFILETIME)&ftEarliest);

  TCHAR tcTemp[5000];

  DWORD dwError;
  BOOL bEndFind = FALSE;
  TCHAR pTempName[MAX_PATH+1];
  //TCHAR pFileName[MAX_PATH+1];
  TCHAR pDirName[MAX_PATH+1];
  TCHAR pDeleteName[MAX_PATH+1];
  
  
  ZeroMemory(pDeleteName,sizeof(TCHAR)*(MAX_PATH+1));
  CheckForSlash(m_lpArchive);
  ZeroMemory(pTempName,sizeof(TCHAR)*(MAX_PATH+1));
  ZeroMemory(pDirName,sizeof(TCHAR)*(MAX_PATH+1));
  _stprintf(pTempName, _T("%s*.*"), m_lpArchive);
  for (i=0; i<m_lCameraAmount; i++)
  {
    _stprintf(pTempName, _T("%s%d\\*.*"), m_lpArchive, m_plCameraIDs[i]);
    hFind = FindFirstFile(pTempName, &FindFileData);

    bEndFind=false;
    if(INVALID_HANDLE_VALUE == hFind)
    {
    }
    else
    {
      WHILEM((FindNextFile(hFind,&FindFileData))&(!bEndFind))
      {
        if(FindFileData.cFileName[0]==_T('.'))
        {
          continue;
        }
        else
        {
          ZeroMemory(pDirName,sizeof(TCHAR)*(MAX_PATH+1));
          _stprintf(pDirName, _T("%s%d\\%s\\*.avi"), m_lpArchive, m_plCameraIDs[i], FindFileData.cFileName);
          
          ZeroMemory(pTempName,sizeof(TCHAR)*(MAX_PATH+1));
          _stprintf(pTempName, _T("%s%d\\%s"), m_lpArchive, m_plCameraIDs[i], FindFileData.cFileName);
          
          hFindIn = FindFirstFile(pDirName, &FindFileDataIn);

          if(INVALID_HANDLE_VALUE == hFindIn)
          {
            if(!RemoveNEDir(pTempName), _T("avi"))
            {
              _stprintf(tcTemp, _T("Не смог удалить директорию:%s."), pTempName);
              m_pDebugOutput->PrintW(IDebugOutput::mt_error,tcTemp);
              break;
            }
          }
          else
          {
            do
            {
              if(FindFileDataIn.cFileName[0]==_T('.'))
              {
                continue;
              }
              if(CompareFileTime(&ftEarliest,&(FindFileDataIn.ftLastWriteTime))==1)
              {
                ZeroMemory(pDeleteName,sizeof(TCHAR)*(MAX_PATH+1));
                _stprintf(pDeleteName,_T("%s\\%s"), pTempName, FindFileDataIn.cFileName);
                ftEarliest=FindFileDataIn.ftLastWriteTime;
                bEndFind = TRUE;
              }
            }
            WHILEM((FindNextFile(hFindIn,&FindFileDataIn))&(!bEndFind));
            if(!FindClose(hFindIn))
            {
              dwError=GetLastError();
              if((dwError==ERROR_FILE_NOT_FOUND)||(dwError==ERROR_SHARING_VIOLATION))
              {
              }
            }
            if(!bEndFind)
            {
              if(!RemoveNEDir(pTempName), _T("avi"))
              {
                _stprintf(tcTemp, _T("Не смог удалить директорию:%s."), pTempName);
                m_pDebugOutput->PrintW(IDebugOutput::mt_error,tcTemp);
                break;
              }
            }
          }
        }
      }
      dwError=GetLastError();
      if((ERROR_NO_MORE_FILES != dwError)&&(ERROR_SUCCESS!=dwError))
      {
        _stprintf(tcTemp, _T("Не могу найти файлы поданному пути: %s. Ошибка:0x%08X"),
          pTempName, dwError);
        m_pDebugOutput->PrintW(IDebugOutput::mt_error, tcTemp);
      }
      if(!FindClose(hFind))
      {
        dwError=GetLastError();
        if((dwError==ERROR_FILE_NOT_FOUND)||(dwError==ERROR_SHARING_VIOLATION))
        {
        }
      }
    }

  }
  if(pDeleteName)
  {
    if(_tcslen(pDeleteName))
    {
      if(!DeleteFile(pDeleteName))
      {
        dwError=GetLastError();
        _stprintf(tcTemp, _T("Не смог удалить файл:%s. ошибка: 0x%08X"), pDeleteName, dwError);
        m_pDebugOutput->PrintW(IDebugOutput::mt_error,tcTemp);
      }
      if(m_lDeleteCount<FILESCOUNT)
      {
        m_lDeleteCount++;
      }
      else
      {
        if(FTToOT(ftEarliest, dbLAST))
        {
          if(FreeBaseUTD(dbLAST))
          {
            m_lDeleteCount=0;
          }
        }
      }
    }
  }
  
  return S_OK;
}

/*
// Setting last unsuccessful result
void CBackUpCS::SetLastResult(HRESULT ahrLast)
{
  if(FAILED(m_hrLast))
  {
    return;
  }
  m_hrLast=ahrLast;
}
*/
// checking for slash at the end
void CBackUpCS::CheckForSlash(LPTSTR input)
{
  if(!input)
    return;
  TCHAR tcTemp[5000];
  size_t len = _tcslen(input);
  LPTSTR test=0;
  if (len>=0)
  {
    if(input[len-1]==_T('\\'))
    {
      return;
    }
    else
    {
      test=_tcscat(input, _T("\\"));
      if(!test)
      {
        _stprintf(tcTemp, _T("Не могу дополнить следующий путь: %s."),
          input);
        m_pDebugOutput->PrintW(IDebugOutput::mt_error, tcTemp);
      }
    }
  }
}

// Compare with the time parameter if Input < m_daLast return true else false
bool CBackUpCS::CheckFileTime(FILETIME aftInput)
{
  DATE dbEndWrite;
  TCHAR tcTemp[5000];
  if(!FTToOT(aftInput, dbEndWrite))
  {
    _stprintf(tcTemp, _T("Не могу преобразовать время."));
    m_pDebugOutput->PrintW(IDebugOutput::mt_error, tcTemp);
    return false;
  }
  if(dbEndWrite<m_daLast)
  {
    return true;
  }
  return false;
}

// Convert FILETIME to VariantType
BOOL CBackUpCS::FTToOT(FILETIME aftInput, DATE& adaOutput)
{
  SYSTEMTIME stEndWrite;
  FILETIME ftEndWrite;
  DATE dbEndWrite;
  if(!FileTimeToLocalFileTime(&aftInput, &ftEndWrite))
  {
    return FALSE;
  }
  if(!FileTimeToSystemTime(&ftEndWrite, &stEndWrite))
  {
    return FALSE;
  }

  if(!SystemTimeToVariantTime(&stEndWrite, &dbEndWrite))
  {
    return FALSE;
  }
  adaOutput=dbEndWrite;
  return TRUE;
}
// Convert VariantType to FILETIME 
BOOL CBackUpCS::OTToFT(DATE adaInput, FILETIME& aftOutput)
{
  SYSTEMTIME stEndWrite;
  if(!VariantTimeToSystemTime(adaInput, &stEndWrite))
  {
    return FALSE;
  }
  if(!SystemTimeToFileTime(&stEndWrite, &aftOutput))
  {
    return FALSE;
  }
  
  return TRUE;
}

// Remove all files in the dir and the dir one
bool CBackUpCS::RemoveNEDir(LPCTSTR alpDir, LPCTSTR alpExt)
{
  bool bRet=true;
  HANDLE hFind;
  WIN32_FIND_DATA FindFileData;
  TCHAR pTempName[MAX_PATH+1];
  TCHAR pPathName[MAX_PATH+1];
  TCHAR pDirName[MAX_PATH+1];
  DWORD dwError;
  ZeroMemory(pDirName,sizeof(TCHAR)*(MAX_PATH+1));
  _stprintf(pDirName, _T("%s"), alpDir);
  CheckForSlash(pDirName);
  if(alpExt)
  {
    _stprintf(pTempName, _T("%s*.%s"), pDirName, alpExt);
    hFind = FindFirstFile(pTempName, &FindFileData);
    if(INVALID_HANDLE_VALUE != hFind)
    {
      return bRet;
    }
  }
  _stprintf(pTempName, _T("%s*.*"), pDirName);
  hFind = FindFirstFile(pTempName, &FindFileData);
  if(INVALID_HANDLE_VALUE == hFind)
  {
  }
  else
  {
    WHILEM(FindNextFile(hFind,&FindFileData))
    {
      _stprintf(pPathName, _T("%s%s"), pDirName, FindFileData.cFileName);
      if(FindFileData.cFileName[0]==_T('.'))
      {
        continue;
      }
      else
      {
        if(SetFileAttribForDel(pPathName))
        {
          if(!DeleteFile(pPathName))
          {
            bRet=false;
            break;
          }
        }
        else
        {
          bRet=false;
          break;
        }
      }
    }
    dwError=GetLastError();
    if((ERROR_NO_MORE_FILES != dwError)&&(ERROR_SUCCESS!=dwError))
    {
      bRet=false;
    }
    if(!FindClose(hFind))
    {
      bRet=false;
    }
  }
  
  if(SetFileAttribForDel(pDirName))
  {
    if(!RemoveDirectory(pDirName))
    {
      dwError=::GetLastError();
      if((dwError!=ERROR_DIR_NOT_EMPTY)&&(dwError!=ERROR_FILE_NOT_FOUND)&&(dwError!=ERROR_PATH_NOT_FOUND))
      {
        bRet=false;
      }
    }
  }
  else
  {
    bRet=false;
  }
  return bRet;
}

bool CBackUpCS::SetFileAttribForDel(LPCTSTR aFile)
{
  DWORD dwAttrs; 
  dwAttrs = GetFileAttributes(aFile); 
  if(dwAttrs & FILE_ATTRIBUTE_READONLY) 
  { 
    if(SetFileAttributes(aFile, dwAttrs & ~FILE_ATTRIBUTE_READONLY))
    {
    }
    else
    {
      return false;
    }
  }
  return true;
}

// Free statistics up to date
bool CBackUpCS::FreeBaseUTD(DATE daLast)
{
  HRESULT	 hr;
  TCHAR  stSQL[5000];
  TCHAR  stTemp[5000];
  ///////////////////////////////////////////////////////////////
  /////Открытие сессии
  ///////////////////////////////////////////////////////////////
  daLast=daLast-2;
  CDBPropSet	propset(DBPROPSET_ROWSET);
  propset.AddProperty(DBPROP_IRowsetChange, true);
  propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE | DBPROPVAL_UP_INSERT | DBPROPVAL_UP_DELETE);
  propset.AddProperty(DBPROP_SERVERDATAONINSERT, true);

  CSession session; // database session
  CCommand< CNoAccessor, CNoRowset > cmDelObjects;
  long i=0;
  for(i=0;i<m_lRoomIDNum; i++)
  {
    ReopenSession(session);
    _stprintf(stSQL,_T("DELETE FROM [Statistics] \
                       WHERE (StatTime<%f) AND (RoomId=%d);"), daLast, m_plRoomIDs[i]);
    hr=cmDelObjects.Open(session,stSQL);
    if (FAILED(hr))
    {
      _stprintf(stTemp, _T("Не выполнен запрос на удаление: %s. ошибка: 0x%08X"), stSQL, hr);
      m_pDebugOutput->PrintW(IDebugOutput::mt_error,stTemp);
    }
    WHILEM(FAILED(hr))
    {
      session.Close();
      Sleep(DOLBTIME);
      ReopenSession(session);
      hr=cmDelObjects.Open(session,stSQL);
    }
    cmDelObjects.Close();

//    ReopenSession(session);
    _stprintf(stSQL,_T("DELETE FROM [Timetable] \
                       WHERE (EndTime<%f) AND (RoomId=%d);"), daLast, m_plRoomIDs[i]);
    hr=cmDelObjects.Open(session,stSQL);
    if (FAILED(hr))
    {
      _stprintf(stTemp, _T("Не выполнен запрос на удаление из Timetable: %s. ошибка: 0x%08X"), stSQL, hr);
      m_pDebugOutput->PrintW(IDebugOutput::mt_error,stTemp);
    }
    WHILEM(FAILED(hr))
    {
      session.Close();
      Sleep(DOLBTIME);
      ReopenSession(session);
      hr=cmDelObjects.Open(session,stSQL);
    }
    cmDelObjects.Close();
    session.Close();

  }
  return true;
}

// Reopen database session
void CBackUpCS::ReopenSession(CSession& aSession)
{
  HRESULT hr = S_OK;
  TCHAR  tcTemp[5000];
  hr=OpenSession(aSession);
  if(FAILED(hr))
  {
    _stprintf(tcTemp, _T("Не смог заново открыть сессию работы с базой данных. ошибка: 0x%08X"), hr);
    m_pDebugOutput->PrintW(IDebugOutput::mt_error,tcTemp);
  }
  WHILEM(FAILED(hr))
  {
    Sleep(DOLBTIME/4);
    hr=OpenSession(aSession);
  }
}

// Open session to database
HRESULT CBackUpCS::OpenSession(CSession& aSession)
{
  HRESULT hr = S_OK;

  m_ds.Close();
  long lError=ERROR_SUCCESS;
  TCHAR  tcTemp[5000];
  TCHAR szValue[5000];
  DWORD dwLen = 5000;
  /////////////////////////////////////////////////////////////////
  /////////////// Init DataBase source.
  /////////////////////////////////////////////////////////////////
  CRegKey keyCinemaSec;
  ///////////////////////////////////////////////////////
  ///////////Создание раздела базы данных
  ///////////////////////////////////////////////////////

  lError=keyCinemaSec.Create(HKEY_LOCAL_MACHINE,_T("Software\\ElVEES\\CinemaSec"));
  if(ERROR_SUCCESS!=lError)
  {
  }
  WHILEM(ERROR_SUCCESS!=lError)
  {
    Sleep(DOLBTIME);
    lError=keyCinemaSec.Create(HKEY_LOCAL_MACHINE,_T("Software\\ElVEES\\CinemaSec"));
  }
  ///////////////////////////////////////////////////////
  ///////////Строка инициализации базы
  ///////////////////////////////////////////////////////
  lError=keyCinemaSec.QueryValue(_T("DBConnectionString"), NULL, szValue, &dwLen);
  if(ERROR_SUCCESS!=lError)
  {
    _stprintf(tcTemp, _T("Не смог получить строку для работы с базой данных. ошибка: 0x%08X"), lError);
    m_pDebugOutput->PrintW(IDebugOutput::mt_error,tcTemp);
  }
  else
  {
    /////////////////////////////////////////////////////////
    //////////////Открытие постоянной базы
    /////////////////////////////////////////////////////////
    CComBSTR bsTemp(szValue);
    hr = m_ds.OpenFromInitializationString(bsTemp.m_str);
    if(FAILED(hr))
    {
      _stprintf(tcTemp, _T("Не смог открыть сессию работы с базой данных. ошибка: 0x%08X"), hr);
      m_pDebugOutput->PrintW(IDebugOutput::mt_error,tcTemp);
    }
    WHILEM(FAILED(hr))
    {
      Sleep(DOLBTIME);
      hr = m_ds.OpenFromInitializationString(bsTemp.m_str);
    }

  }
  hr = aSession.Open(m_ds);
  return hr;
}

// getting CameraIDs for the local computer
bool CBackUpCS::GetCameraIDs(void)
{
  LPVOID lpMsgBuf;
  long i=0;
  TCHAR tcTemp[5000];
  /////////////////////////////////////////////////////////////
  /////// Получение IP адреса машины
  /////////////////////////////////////////////////////////////
  WSADATA  wsadata;
  HOSTENT *lpHost=NULL;
  char   hostname[MAXLEN];
  TCHAR    tcIP_adr[128];
  int      ret;
  struct   sockaddr_in retaddr;


  if((ret = WSAStartup(0x0101, &wsadata)) !=0)
  {
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL);
//    SetLastResult(AtlReportError(GetObjectCLSID(), (LPTSTR)lpMsgBuf));
    LocalFree(lpMsgBuf);
  }
  WHILEM(WSAStartup(0x0101, &wsadata))
  {
    Sleep(DOLBTIME);
  }

  if((ret = gethostname(hostname, MAXLEN)) != 0)
  {
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL);
//    SetLastResult(AtlReportError(GetObjectCLSID(), (LPTSTR)lpMsgBuf));
    LocalFree(lpMsgBuf);
  }
  WHILEM(gethostname(hostname, MAXLEN))
  {
    Sleep(DOLBTIME);
  }

  lpHost = gethostbyname(hostname);
  if(lpHost == NULL)
  {
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL);
//    SetLastResult(AtlReportError(GetObjectCLSID(), (LPTSTR)lpMsgBuf));
    LocalFree(lpMsgBuf);
  }
  WHILEM(lpHost == NULL)
  {
    Sleep(DOLBTIME);
    lpHost = gethostbyname(hostname);
  }
  for(i=0; lpHost->h_addr_list[i] != NULL ;i++)
  {
    memcpy(&(retaddr.sin_addr), lpHost->h_addr_list[i], lpHost->h_length);
    _stprintf(tcIP_adr, _T("%S"), inet_ntoa(retaddr.sin_addr));
  }

////////////// for debug only
//  _stprintf(tcIP_adr, _T("%s"), _T("192.168.1.104"));
  
  HRESULT	 hr;
  TCHAR  stSQL[5000];
  ///////////////////////////////////////////////////////////////
  /////Открытие сессии
  ///////////////////////////////////////////////////////////////
  CDBPropSet	propset(DBPROPSET_ROWSET);
  propset.AddProperty(DBPROP_IRowsetChange, true);
  propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE | DBPROPVAL_UP_INSERT | DBPROPVAL_UP_DELETE);
  propset.AddProperty(DBPROP_SERVERDATAONINSERT, true);

  CSession session; // database session
  CCommand<CAccessor< CCamerasAmAcc > > AccCamsAmount;
  ReopenSession(session);
  _stprintf(stSQL,_T("SELECT COUNT(CameraID) FROM Cameras WHERE CameraIP=\'%s\'"), tcIP_adr);
  hr=AccCamsAmount.Open(session,stSQL);
  if (FAILED(hr))
  {
    _stprintf(tcTemp, _T("Не смог получить количество камер. ошибка: 0x%08X"), hr);
    m_pDebugOutput->PrintW(IDebugOutput::mt_error,tcTemp);
  }
  WHILEM(FAILED(hr))
  {
    session.Close();
    Sleep(DOLBTIME);
    ReopenSession(session);
    hr=AccCamsAmount.Open(session,stSQL);
  }
  hr=AccCamsAmount.MoveFirst();
  if (FAILED(hr))
  {
    _stprintf(tcTemp, _T("Не смог получить количество камер. ошибка: 0x%08X"), hr);
    m_pDebugOutput->PrintW(IDebugOutput::mt_error,tcTemp);
    return false;
  }
  else
  {
    m_lCameraAmount=AccCamsAmount.m_CamerasAmount; 
    _stprintf(tcTemp, _T("количество камер: %d"), m_lCameraAmount);
    m_pDebugOutput->PrintW(IDebugOutput::mt_debug_info,tcTemp);
  }
  AccCamsAmount.Close();
  if(m_plCameraIDs)
  {
    delete []m_plCameraIDs;
  }
  m_plCameraIDs = new LONG[m_lCameraAmount];
  
  
  CCommand<CAccessor< CCamerasAccessor > > AccCams;
//  ReopenSession(session);
  _stprintf(stSQL,_T("SELECT CameraID FROM Cameras WHERE CameraIP=\'%s\'"), tcIP_adr);
  hr=AccCams.Open(session,stSQL);
  i=0;
  if (FAILED(hr))
  {
    _stprintf(tcTemp, _T("Не смог получить идентефикторы камер. ошибка: 0x%08X"), hr);
    m_pDebugOutput->PrintW(IDebugOutput::mt_error,tcTemp);
  }
  WHILEM(FAILED(hr))
  {
    session.Close();
    Sleep(DOLBTIME);
    ReopenSession(session);
    hr=AccCams.Open(session,stSQL);
  }
  hr=AccCams.MoveFirst();
  WHILEM(S_OK==hr)
  {
    m_plCameraIDs[i]=AccCams.m_CameraID;
    _stprintf(tcTemp, _T("Номер камеры: %d"), m_plCameraIDs[i]);
    m_pDebugOutput->PrintW(IDebugOutput::mt_debug_info,tcTemp);
    i++;
    hr=AccCams.MoveNext();
  }
  AccCams.Close();
  session.Close();
  if(i!=m_lCameraAmount)
  {
    return false;
  }
  return true;
}

// получение изменяющихся параметров
HRESULT CBackUpCS::GetChanges(void)
{
  HRESULT	 hr;
  TCHAR  stSQL[5000];
  TCHAR  stTemp[5000];
  
  ///////////////////////////////////////////////////////////////
  /////Открытие сессии
  ///////////////////////////////////////////////////////////////
  CDBPropSet	propset(DBPROPSET_ROWSET);
  propset.AddProperty(DBPROP_IRowsetChange, true);
  propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE | DBPROPVAL_UP_INSERT | DBPROPVAL_UP_DELETE);
  propset.AddProperty(DBPROP_SERVERDATAONINSERT, true);

  CSession session; // database session
  CCommand<CAccessor< CCinemaAcc > > AccCinemas;
  ReopenSession(session);
  _stprintf(stSQL,_T("SELECT BackupStart, BackupStop, BackupDate \
    FROM Cinemas INNER JOIN Rooms ON Cinemas.CinemaID=Rooms.CinemaID \
    WHERE Rooms.RoomID=%d"), m_plRoomIDs[0]);
  hr=AccCinemas.Open(session,stSQL);
  if (FAILED(hr))
  {
    _stprintf(stTemp, _T("Ошибка при получении параметров архивирования. HR=%08X"), hr); 
    m_pDebugOutput->PrintW(IDebugOutput::mt_error, stTemp);
  }
  WHILEM(FAILED(hr))
  {
    session.Close();
    Sleep(DOLBTIME);
    ReopenSession(session);
    hr=AccCinemas.Open(session,stSQL);
  }
  hr=AccCinemas.MoveFirst();
  if (FAILED(hr))
  {
    _stprintf(stTemp, _T("Ошибка при получении параметров архивирования. HR=%08X"), hr); 
    m_pDebugOutput->PrintW(IDebugOutput::mt_error, stTemp);
    return false;
  }
  else
  {
    m_ulStartPer=(ULONG)(AccCinemas.m_BackupStart*100.0);
    m_ulStopPer=(ULONG)(AccCinemas.m_BackupStop*100.0);
    SYSTEMTIME tSysTime;
    GetLocalTime(&tSysTime);
    SystemTimeToVariantTime(&tSysTime, &m_daLast);
    m_daLast=m_daLast-AccCinemas.m_BackupDate;
  }
  AccCinemas.Close();
  session.Close();
  return E_NOTIMPL;
}

// Hack by MoFo
//  [7/25/2005]

HRESULT CBackUpCS::Release()
{
	if(IsBadReadPtr(this, sizeof(CBackUpCS)))
		return E_FAIL;

	// Is thread started?
	if(::WaitForSingleObject(m_hThread, 0) == WAIT_TIMEOUT)
	{
		// try to stop server
		if(::SetEvent(m_heStop))
		{
			// Wait for thread
			WaitForSingleObject(m_hThread, DOLBTIME/2);
		}

		// Still started?
		if(::WaitForSingleObject(m_hThread, 0) == WAIT_TIMEOUT)
		{
			m_pDebugOutput->PrintW(IDebugOutput::mt_error, L"Terminating buckup thread");

			// Kill process :((
			if(!TerminateThread(m_hThread, 1))
			{
				m_pDebugOutput->PrintW(IDebugOutput::mt_error, L"Terminate failed");
			}
		}
	}

	__try
	{
		// delete context
		// if thread still runed it will cause an exception
		// 
		delete this;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return S_FALSE;
	}

	return S_OK;
}

unsigned __stdcall CBackUpCS::ThreadMain(void* pParam)
{
	CBackUpCS* pBackUp = (CBackUpCS*)pParam;
	HRESULT hr;

	hr = CoInitialize(NULL);
	if(FAILED(hr))
	{
		pBackUp->m_pDebugOutput->PrintW(IDebugOutput::mt_error, L"Не могу инициализировать COM!");
		return 1;
	}

	try
	{
		long lError=ERROR_SUCCESS;
		TCHAR szValue[5000];
		DWORD dwLen = 5000;
		/////////////////////////////////////////////////////////////////
		/////////////// Init DataBase source.
		/////////////////////////////////////////////////////////////////
		CRegKey keyCinemaSec;
		///////////////////////////////////////////////////////
		///////////Создание раздела базы данных
		///////////////////////////////////////////////////////

		lError=keyCinemaSec.Create(HKEY_LOCAL_MACHINE,_T("Software\\ElVEES\\CinemaSec"));
		if(ERROR_SUCCESS!=lError)
		{
			pBackUp->m_pDebugOutput->PrintW(IDebugOutput::mt_error,_T("ключ HKLM/Software/ElVEES/CinemaSec недоступен"));
			return lError;
		}
		///////////////////////////////////////////////////////
		///////////Строка инициализации базы
		///////////////////////////////////////////////////////
		lError=keyCinemaSec.QueryValue(_T("DBConnectionString"), NULL, szValue, &dwLen);
		if(ERROR_SUCCESS!=lError)
		{
			pBackUp->m_pDebugOutput->PrintW(IDebugOutput::mt_error,_T("Нет строки базы данных"));
			return lError;
		}
		else
		{
			/////////////////////////////////////////////////////////
			//////////////Открытие постоянной базы
			/////////////////////////////////////////////////////////
			CComBSTR bsTemp(szValue);
			hr = pBackUp->m_ds.OpenFromInitializationString(bsTemp.m_str);
			if(FAILED(hr))
			{
				pBackUp->m_pDebugOutput->PrintW(IDebugOutput::mt_error,_T("Нет доступа к базе данных"));
				return hr;
			}
		}
		///////////////////////////////////////////////////
		////Выбор директории для удаления 
		///////////////////////////////////////////////////
		lError=keyCinemaSec.QueryValue(_T("ArchivePath"), NULL, szValue, &dwLen);
		if(ERROR_SUCCESS!=lError)
		{
			pBackUp->m_pDebugOutput->PrintW(IDebugOutput::mt_error,_T("Нет строки пути к архиву"));
			return lError;
		}
		else
		{
			if(pBackUp->m_lpArchive)
			{
				pBackUp->m_pDebugOutput->PrintW(IDebugOutput::mt_error,_T("Нет строки пути к архиву"));
				return E_FAIL;
			}
			pBackUp->m_lpArchive=new TCHAR[_tcslen(szValue)+2];
			_tcscpy(pBackUp->m_lpArchive, szValue);
		}
		///////////////////////////////////////////////////
		////Выбор RoomID 
		///////////////////////////////////////////////////
		dwLen=4;
		LONG lTest[1];
		lError=keyCinemaSec.QueryValue(_T("RoomID"), NULL, lTest, &dwLen);
		if((ERROR_MORE_DATA==lError)||(ERROR_SUCCESS==lError))
		{
			if(pBackUp->m_plRoomIDs)
			{
				delete []pBackUp->m_plRoomIDs;
			}
			pBackUp->m_lRoomIDNum=dwLen/4;
			pBackUp->m_plRoomIDs=new LONG[pBackUp->m_lRoomIDNum];
			TCHAR  stTemp[5000];
			lError=keyCinemaSec.QueryValue(_T("RoomID"), NULL, pBackUp->m_plRoomIDs, &dwLen);
			if(ERROR_SUCCESS!=lError)
			{
				pBackUp->m_pDebugOutput->PrintW(IDebugOutput::mt_error,_T("Не удалось считать значения RoomID"));
				return lError;
			}
			else
			{
				LONG i=0;
				for (i=0; i<pBackUp->m_lRoomIDNum; i++)
				{
					_stprintf(stTemp, _T("RoomID:%d."), pBackUp->m_plRoomIDs[i]);
					pBackUp->m_pDebugOutput->PrintW(IDebugOutput::mt_debug_info,stTemp);
				}
			}
		}
		else
		{
			pBackUp->m_pDebugOutput->PrintW(IDebugOutput::mt_error,_T("Нет значения RoomID"));
			return lError;
		}
		///////////////////////////////////////////////////
		////Выбор CinemaID 
		///////////////////////////////////////////////////
		lError=keyCinemaSec.QueryValue(_T("CinemaID"), NULL, &pBackUp->m_lCinemaID, &dwLen);
		if(ERROR_SUCCESS!=lError)
		{
			pBackUp->m_pDebugOutput->PrintW(IDebugOutput::mt_error,_T("Нет значения CinemaID"));
			return lError;
		}

		pBackUp->m_bIsWorking=FALSE;
		if(!pBackUp->GetCameraIDs())
		{
			pBackUp->m_pDebugOutput->PrintW(IDebugOutput::mt_error,_T("Не удалось получить значения CameraID"));
			return lError;
		}
		//  InitializeCriticalSection(&pBackUp->m_csIsWorking);

		pBackUp->GetChanges(); 

		while(::WaitForSingleObject(pBackUp->m_heStop, DOLBTIME/2) != WAIT_OBJECT_0)
		{
			//    HRESULT hrTemp=S_OK;
			if (!pBackUp->CheckDiskSpace())
			{
			}
			if (!pBackUp->CheckLastFolder())
			{
			}
			if (!pBackUp->FreeBaseUTD(pBackUp->m_daLast))
			{
			}

			hr=pBackUp->GetChanges();
			if (FAILED(hr))
			{
			}
		}
	}
	catch(...)
	{
		pBackUp->m_pDebugOutput->PrintW(IDebugOutput::mt_error,
			L"Unexpected exception in CBackUpCS::ThreadMain()");
	}

	CoUninitialize();
	return 0;
}