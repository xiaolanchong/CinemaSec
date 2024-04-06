//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Таймауты системы
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 22.09.2005
//                                                                                      //
//======================================================================================//
#ifndef _TIMEOUTS_3131283605369432_
#define _TIMEOUTS_3131283605369432_
//======================================================================================//
//                                   struct Timeouts                                    //
//======================================================================================//

//! время остановки ICameraAnalyzer, мс
const unsigned int	ThreadTimeout_CameraAnalyzer	= 10 * 1000; // 10s
//! время остановки IHallAnalyzer, мс
const unsigned int	ThreadTimeout_HallAnalyzer		= 10 * 1000; // 10s
//! время остановки потока обработки зала, мс
const unsigned int	ThreadTimeout_MainServer		= ThreadTimeout_CameraAnalyzer + 5 * 1000; // 15s

//! время остановки потока передачи файлов на стороне сервера, мс
const unsigned int	ThreadTimeout_FileTransferServer	= 3 * 1000; // 3s
//! время остановки потока диагносики на стороне клиента, мс
const unsigned int	ThreadTimeout_SystemDiagnostic		= 1000; //1s
//! время остановки потока обработки запросов от BoxOffice
const unsigned int	ThreadTimeout_Request				= 1500; //
//! время остановки потока передачи файлов на стороне клиента, мс
const unsigned int	ThreadTimeout_FileTransferClient	= ThreadTimeout_FileTransferServer; //

#endif // _TIMEOUTS_3131283605369432_