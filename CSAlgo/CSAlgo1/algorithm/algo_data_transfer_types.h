/****************************************************************************
  algo_data_transfer_types.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//#include "algo_types.h"

namespace csalgo
{

// Index of camera of interest (first) and demo image (second).
typedef  csinterface::TDataWraper< std::pair<__int32,QImage>, DATAID_DEMO_IMAGE >  TDemoImage;

// Index of the current frame (first) and the number of dropped frames (second).
typedef  csinterface::TDataWraper< std::pair<__int32,__int32>, DATAID_FRAME_COUNTERS >  TFrameCounters;

// The binary image of camera's statistics data.
typedef  csinterface::TDataWraper< Int8Arr, DATAID_CAMERA_STATISTICS >  TCameraStatistics;

// Array of chair's learning data.
typedef  csinterface::TDataWraper< std::vector<ChairLearn>, DATAID_CHAIR_LEARN_ARRAY >  TChairLearnArr;

// Container of base chair structures.
typedef  csinterface::TDataWraper< BaseChairArr, DATAID_BASE_CHAIR_SET >  TBaseChairSet;

// Base chair and its content type.
typedef  csinterface::TDataWraper< std::pair<BaseChair,__int32>, DATAID_BASE_CHAIR_AND_CONTENT >  TBaseChairAndContent;

} // namespace csalgo

