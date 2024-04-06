/****************************************************************************
  CSAlgo2Dll.h
  ---------------------
  begin     : Aug 2004
  modified  : Aug 2005
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

#include "algorithm/AlgoParameters.h"

typedef  std::vector<Arr2ub>                    SampleArr;
typedef  kDimTree<Arr2ub,Arr2ub,float,(64*64)>  ImageTree;

const TCHAR ALGO2_EMPTY_SEQUENCE[] = _T("algo2_empty.avd");
const TCHAR ALGO2_CLOTH_SEQUENCE[] = _T("algo2_cloth.avd");
const TCHAR ALGO2_HUMAN_SEQUENCE[] = _T("algo2_human.avd");

//=================================================================================================
/** \class CCSAlgo2Dll.
    \brief CCSAlgo2Dll. */
//=================================================================================================
class CCSAlgo2Dll : public CWinApp
{
private:
  SampleArr m_emptyImages;      //!< the set of empty chair images
  SampleArr m_clothImages;      //!< the set of clothes images
  SampleArr m_humanImages;      //!< the set of human images

private:
  bool CreateTreeOfSampleImages( LPCWSTR fileName, SampleArr & samples, ImageTree & tree );

public:
  std::wstring        m_dllName;          //!< name of this library
  csalgo2::Parameters m_parameters;       //!< current parameters
  std::wstring        m_paramFileName;    //!< parameter file name
  CCriticalSection    m_dataLocker;       //!< object locks/unlocks state variables
  ImageTree           m_emptyTree;        //!< the set of empty chair images
  ImageTree           m_clothTree;        //!< the set of clothes images
  ImageTree           m_humanTree;        //!< the set of human images

public:
  CCSAlgo2Dll();
  virtual BOOL InitInstance();
  DECLARE_MESSAGE_MAP()
};

extern CCSAlgo2Dll theDll;

