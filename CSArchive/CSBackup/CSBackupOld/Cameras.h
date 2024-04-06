// Cameras.h : Declaration of the CCameras

#pragma once

// code generated on 7 апреля 2005 г., 19:35
class CCamerasAccessor
{
public:
  LONG m_CameraID;

  BEGIN_COLUMN_MAP(CCamerasAccessor)
    COLUMN_ENTRY(1, m_CameraID)
  END_COLUMN_MAP()
};

class CCamerasAmAcc
{
public:
  LONG m_CamerasAmount;

  BEGIN_COLUMN_MAP(CCamerasAmAcc)
    COLUMN_ENTRY(1, m_CamerasAmount)
  END_COLUMN_MAP()
};

class CCinemaAcc
{
public:
  double m_BackupStart;
  double m_BackupStop;
  DATE   m_BackupDate;  

  BEGIN_COLUMN_MAP(CCinemaAcc)
    COLUMN_ENTRY(1, m_BackupStart)
    COLUMN_ENTRY(2, m_BackupStop)
    COLUMN_ENTRY(3, m_BackupDate)
  END_COLUMN_MAP()
};
