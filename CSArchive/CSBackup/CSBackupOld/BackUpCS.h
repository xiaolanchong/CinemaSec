// BackUpCS.h: interface for the CBackUpCS class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "backup_interface.h"

#define DOLBTIME	8000
#define MAXLEN		64
#define FILESCOUNT	20

#define WHILE(x) while((x)&&(WaitForSingleObject(pBackUp->m_heStop, 0)!=WAIT_OBJECT_0))
#define WHILEM(x) while((x)&&(WaitForSingleObject(m_heStop, 0)!=WAIT_OBJECT_0))

#define tWHILE(x, t) while((x)&&(WaitForSingleObject(pBackUp->m_heStop, t)!=WAIT_OBJECT_0))
#define tWHILEM(x, t) while((x)&&(WaitForSingleObject(m_heStop, t)!=WAIT_OBJECT_0))

// forward declaration
struct IDebugOutput;

///////////////////////////////////////////////////////////////////////////////
// CEvent
///////////////////////////////////////////////////////////////////////////////

class CBackUpCS : public ICSBackUp
{
private:
  HRESULT m_hrLast;
  // Thread handle
  HANDLE m_hThread;
  // Event for stopping all operations
  HANDLE m_heStop;
  // Thread identification
  DWORD m_dwThreadID;
  // DataSource
  CDataSource m_ds;
  
  LPTSTR m_lpArchive; // почему то удал€етс€ в Release?

  LONG m_lRoomIDNum;
  LONG* m_plRoomIDs;
  LONG m_lCinemaID;

  ULONG m_ulStartPer;
  ULONG m_ulStopPer;
  DATE m_daLast;
  LONG* m_plCameraIDs;
  LONG m_lCameraAmount;
  LONG m_lDeleteCount;

  CRITICAL_SECTION m_csdbLast;
  CRITICAL_SECTION m_csdwStart;
  CRITICAL_SECTION m_csdwStop;

//  CRITICAL_SECTION m_csIsWorking;
  BOOL m_bIsWorking;
  IDebugOutput* m_pDebugOutput;

public:
  explicit CBackUpCS(IDebugOutput* apDebugOutput);
  virtual ~CBackUpCS();

  virtual HRESULT Release();

protected:
  HRESULT Create();
  // Checking free disk space
  bool CheckDiskSpace(void);
  // Check for latest folder is up to time
  bool CheckLastFolder(void);
  // Delete all files up to date
  HRESULT FreeDiskUpToDate(void);
  // Free disk for need space in percentage
  HRESULT FreeDiskForPer(void);

protected:
  // Setting last unsuccessful result
//  void SetLastResult(HRESULT ahrLast);
  // checking for slash at the end
  void CheckForSlash(LPTSTR input);
  // Compare with the time parameter
  bool CheckFileTime(FILETIME aflInput);
  // Convert FILETIME to VariantType
  BOOL FTToOT(FILETIME aftInput, DATE& adaOutput);
  // Convert VariantType to FILETIME
  BOOL OTToFT(DATE adaInput, FILETIME& aftOutput);
  // Remove all files in the dir and the dir one
  bool RemoveNEDir(LPCTSTR alpDir, LPCTSTR alpExt = NULL);
  bool SetFileAttribForDel(LPCTSTR aFile);
  // Free statistic up to date
  bool FreeBaseUTD(DATE daLast);
  // Reopen database session
  void ReopenSession(CSession& aSession);
  // Open session to database
  HRESULT OpenSession(CSession& aSession);
  // getting CameraIDs for the local computer
  bool GetCameraIDs(void);
  // получение измен€ющихс€ параметров
  HRESULT GetChanges(void);

private:
	static unsigned __stdcall ThreadMain(void* pParam);

	// No copies do not implement
	CBackUpCS(const CBackUpCS &rhs);
	CBackUpCS &operator=(const CBackUpCS &rhs);
};