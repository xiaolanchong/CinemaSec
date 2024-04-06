//=====================================================================================//
//                                                                                     //
//                                       CSChair                                       //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   26.01.2005                                                                //
//                                                                                     //
//=====================================================================================//
#include "stdafx.h"
#include "ThreadError.h"
#include <sys/types.h>
#include <sys/timeb.h>

//last header
#include "../../CSChair/public/memleak.h"

LPCWSTR GenericThreadServer::GetThreadErrorDescription( unsigned int Res )
{
	switch( Res )
	{
	case	err_noerror	:		return L"Operation succeeded";
	case 	err_general	:		return L"General error occured";
	case 	err_exception:		return L"Exception occuried";

	case 	err_grabber_invalid:			return L"General error occured";
	case 	err_grabber_invalid_url:		return L"Invalid camera URL";
	case 	err_grabber_start:				return L"IBaseGrabber::Start failed";
	case 	err_grabber_lock:				return L"IBaseGrabber::Lock failed";
	case 	err_grabber_unlock:				return L"IBaseGrabber::Unlock failed";
	case 	err_camanalyzer_start:			return L"ICameraAnalyzer::Start failed";
	case 	err_camanalyzer_process:		return L"ICameraAnalyzer::Process failed";
	case 	err_camanalyzer_setdata:		return L"ICameraAnalyzer::SetData failed";
	case 	err_camanalyzer_getdata:		return L"ICameraAnalyzer::GetData failed";
	case 	err_camanalyzer_initialize:		return L"ICameraAnalyzer::Initialize failed";
	case 	err_camanalyzer_stop	:		return L"ICameraAnalyzer::Stop failed";
	case 	err_camanalyzer_invalid	:		return L"Invalid camera analyzer interface";

	case 	err_hallanalyzer_initialize:	return L"IHallAnalyzer::Initialize failed";
	default: return L"Unknown error";
	}
}

__int64 Plat_AbsoluteTime()
{
	struct __timeb64 timeptr; 
	_ftime64(&timeptr);
	return timeptr.time * 1000 + timeptr.millitm;
}