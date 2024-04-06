//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	��������� �������� ������ �� ������� � �������
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 27.07.2005
//                                                                                      //
//======================================================================================//
#ifndef _I_FILE_TRANSFER_9698803196515548_
#define _I_FILE_TRANSFER_9698803196515548_

#include <boost/tuple/tuple.hpp>

//! ������� ������������� ����������
#define	FILE_TRANSFER_INTERFACE		L"{27046CA4-1059-4C64-AB8F-75D81D8E515E}"
//======================================================================================//
//                                 struct IFileTransfer                                 //
//======================================================================================//

/// <summary>
/// ��������� �������� ������. ��������� �������� ����� CreateEngineInterface � ������������� ����� ����� Release
/// </summary>

struct IFileTransfer
{
	/// interface of the camera transferring state
	struct IState
	{
		virtual ~IState() {};
		//! prototype template
		virtual IState* Clone() const = 0;
	};

	//! have not yet started
	struct NotStarted : IState	
	{
		virtual IState* Clone() const { return new NotStarted;};
	};

	//! have already finished w/ a result
	struct Finished : IState
	{
		//! result code of the donwload
		int m_nResultCode;
		Finished( int rc): m_nResultCode(rc){}

		virtual IState* Clone() const { return new Finished(m_nResultCode);};
	};

	//! downloading is proceeding
	struct Working : IState
	{
		//! 0.0 - 0% completed, 1.0 - 100% completed
		double	m_fCompleted;
		//! size of the downloaded file
		DWORD64	m_nFileSize;
		Working( double f, DWORD64 nfs ) : m_fCompleted(f), m_nFileSize(nfs){}

		virtual IState* Clone() const { return new Working(m_fCompleted, m_nFileSize);};
	};

	//! A critical error have occuried
	struct CriticalError : IState	{};

	//! A server doesn't respond
	struct CriticalErrorRequest : CriticalError
	{
		//! server name for which the error occuiried
		std::wstring m_sServerName;

		CriticalErrorRequest( const std::wstring& n ): m_sServerName(n){}

		virtual IState* Clone() const { return new CriticalErrorRequest(m_sServerName);};
	};

	struct CriticalErrorFile : CriticalError
	{
		//! server name for which the error occuiried
		std::wstring m_sFileName;

		CriticalErrorFile( const std::wstring& n ): m_sFileName(n){}

		virtual IState* Clone() const { return new CriticalErrorFile(m_sFileName);};
	};

	struct CriticalErrorSocket : CriticalError
	{
		virtual IState* Clone() const { return new CriticalErrorSocket();};
	};
/*! 
	\brief ���������� ���������� ����������
*/
	virtual void	Release()	= 0;
/*!	
	\li	ID ������
	\li	��� �������
	\li	��� ����������
*/
	typedef	boost::tuple<int, std::wstring, std::wstring>	CamRequest_t;
/*!	
	\brief	��������� ������ �� �������� � ������� � ����� ��������
	\parma	pDbgInt				���������� ���������
	\param	CameraIDAndPaths	������ ������������� �����
	\param	nBeginTime			��������� �����, ��. _time64()
	\param	nEndTime			�������� �����, ��. _time64()
	\return \li	S_OK - ������ ���������
			\li	E_FAIL - ������
*/

	virtual HRESULT	Start( IDebugOutput* pDbgInt, std::vector<CamRequest_t>&	CamRequest, INT64 nBeginTime, INT64 nEndTime )		= 0;

	//! result code of the downloading
	enum	ResultCode
	{
		//! no error
		Ok,			
		//! request error, e.g. wrong ip or server is offline
		ErrorRequest,
		//! timeout, e.g. server have exited
		ErrorTimeOut,
		//! transfer error, e.g. network problem
		ErrorTransfer,
		//! server is curruntly busy, try later
		ErrorServerBusy,
		//! unspecified error
		ErrorUnhandled,
		//! source
		ErrorSourceError
	};
/**
     \brief		����� ��������� ������ ��� ������
	 \param		nCameraID ������������� ������
	 \return	��������� �� �������� ��������� ��������
 */
	virtual	std::auto_ptr<IState>	GetState( int nCameraID )										= 0;  
//!	\brief obsolete
	virtual	std::auto_ptr<IState>	GetTotalState(  )												= 0;  
/*!
	\brief	������ �������
*/
	virtual	HRESULT	Cancel()																		= 0;

/**
   \brief	�������� ������ �������
   \return	\li true	- �������� �����������
			\li false	- �������� ���
 */
	virtual bool IsBusy()	= 0;


};

#endif // _I_FILE_TRANSFER_9698803196515548_