#pragma once

#include "algorithm/algo_param.h"

class CCSAlgo1Dll : public CWinApp
{
public:
  std::wstring        m_dllName;       //!< full-path name of this module
  csalgo1::Parameters m_parameters;    //!< current parameters
  std::wstring        m_paramFileName; //!< parameter file name
  CCriticalSection    m_dataLocker;    //!< object locks/unlocks state variables

public:
  CCSAlgo1Dll();
  virtual BOOL InitInstance();
  DECLARE_MESSAGE_MAP()
};

extern CCSAlgo1Dll theDll;

