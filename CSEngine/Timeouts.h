//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	�������� �������
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

//! ����� ��������� ICameraAnalyzer, ��
const unsigned int	ThreadTimeout_CameraAnalyzer	= 10 * 1000; // 10s
//! ����� ��������� IHallAnalyzer, ��
const unsigned int	ThreadTimeout_HallAnalyzer		= 10 * 1000; // 10s
//! ����� ��������� ������ ��������� ����, ��
const unsigned int	ThreadTimeout_MainServer		= ThreadTimeout_CameraAnalyzer + 5 * 1000; // 15s

//! ����� ��������� ������ �������� ������ �� ������� �������, ��
const unsigned int	ThreadTimeout_FileTransferServer	= 3 * 1000; // 3s
//! ����� ��������� ������ ���������� �� ������� �������, ��
const unsigned int	ThreadTimeout_SystemDiagnostic		= 1000; //1s
//! ����� ��������� ������ ��������� �������� �� BoxOffice
const unsigned int	ThreadTimeout_Request				= 1500; //
//! ����� ��������� ������ �������� ������ �� ������� �������, ��
const unsigned int	ThreadTimeout_FileTransferClient	= ThreadTimeout_FileTransferServer; //

#endif // _TIMEOUTS_3131283605369432_