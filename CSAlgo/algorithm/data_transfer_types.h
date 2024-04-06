/****************************************************************************
  DataTransferTypes.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csalgocommon
{

// Index of camera of interest (first) and demo image (second).
typedef
csinterface::TDataWraper< std::pair<__int32,QImage>, csinterface::DATAID_DEMO_IMAGE >
TDemoImage;

// Index of the current frame (first) and the number of dropped frames (second).
typedef
csinterface::TDataWraper< std::pair<__int32,__int32>, csinterface::DATAID_FRAME_COUNTERS >
TFrameCounters;

// The binary image of camera's statistics data.
typedef
csinterface::TDataWraper< Int8Arr, csinterface::DATAID_CAMERA_STATISTICS >
TCameraStatistics;

// Container of base chair structures.
typedef
csinterface::TDataWraper< BaseChairArr, csinterface::DATAID_BASE_CHAIR_SET >
TBaseChairSet;

// Base chair and its content type.
typedef
csinterface::TDataWraper< std::pair<BaseChair,__int32>, csinterface::DATAID_BASE_CHAIR_AND_CONTENT >
TBaseChairAndContent;

} // namespace csalgocommon

