                                                                                  //
//=====================================================================================//
#ifndef __CAMERA_SERVER_H_INCLUDED_5433557040532002__
#define __CAMERA_SERVER_H_INCLUDED_5433557040532002__

class IBackgroundAccumulator;

//=====================================================================================//
//                                 class CameraServer                                  //
//=====================================================================================//
#include "../../CSUtility/utility/debug_interface.h"
#include "../../CSUtility/utility/base_frame_grabber.h"
#include "../../CSChair/Grabber/IGrabberFactory.h"
#include "../../CSInterfaces/data_transfer_types.h"
#include "../../CSAlgo/algorithm/base_chair.h"
#include "../../CSAlgo/algorithm/data_transfer_types.h"
#include "../../CSAlgo/Interfaces/camera_analyzer_interface.h"
#include "../../CSAlgo/Interfaces/hall_analyzer_interface.h"
#include "../../CSAlgo/CSAlgo2/csalgo2.h"
//#include "../FrameGrabber.h"
#include "ThreadError.h"
#include "../syscfg_int.h"

#include "CameraImage.h"

MACRO_EXCEPTION(CameraServerException, ThreadServerException)
MACRO_EXCEPTION( ImageDataException,  CameraServerException)

//! ������ �� ������
struct CameraDataForProcessing
{
	//! ������������� ������
	int					m_nCameraNo;
	//! ������ �� ICameraAnalyzer
	std::vector<BYTE>	m_Data;
	//! ����������� � ������� JPEG
	std::vector<BYTE>	m_Image;

	CameraDataForProcessing(int id, const std::vector<BYTE>& d, const std::vector<BYTE>& img):
		m_Image(img),
		m_Data(d),
		m_nCameraNo(id)
	{
	}
};

//! \brief ����� ������ ��������� ����������� ������
//! \version 1.1
//! \date 10-26-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//! \li 2005-10-26 �������� ������ �� IHallAnalyzer
class CameraServer : public virtual GenericThreadServer
{
protected:
	//! ����� ������ ������
	class CameraThread
	{
		//! ��������� ���������
		ICameraAnalyzer*						m_pAnalyzer;
	
	protected:
		//! ������� ���������
		ThreadMessage				m_CameraMessage;
		//! ���������� ���������
		bool						PopThreadMessage( CameraServer* pServer );
		//! ������� � �������
		void						PushImageAndData(	CameraServer* pServer,
			const std::vector<BYTE>& ImageBuf, 
			const void* pData, size_t nSize  );
		//! ���������� ������ �� IHallAnalyzer 
		void						SetHallData( CameraServer* pServer, const std::vector<BYTE>& Data );
		//! ������������� ������
		int										m_nCameraNo;
	public:
		//! ������������� ������
		ThreadSync		m_Thread;
		//! ���������� ������������� ������
		//! \return ������������� ������
		int					GetCameraNo() const { return m_nCameraNo;};
		//! ������� ��������� ���������
		//! \return ��������� ���������
		ICameraAnalyzer*	GetAnalyzer() { return m_pAnalyzer; }
		//! ��������� ��������� � ������� ��������� ������
		//! \param Id ������������� ���������
		//! \param Data ������ ���������
		void				PushMessage( int Id, const boost::any&	Data )
		{
			ThreadMessageAutoEnter me( m_CameraMessage, true );
			m_CameraMessage.Push( me, Id, Data );
		}

		//! ��������� �-�
		//! \param pParam �������� ��� ������ 
		//! \return 0 - �����, -1 - ������
		static unsigned int WINAPI ThreadProc(void * pParam);
		CameraThread( ICameraAnalyzer* pAnalyzer, int CameraNo ) : 
			m_pAnalyzer(pAnalyzer),
			m_nCameraNo(CameraNo)
		{
		}
		~CameraThread( )
		{
			if( m_pAnalyzer )
			{
				m_pAnalyzer->Release();
				m_pAnalyzer = 0;
			}	
		}

	};
private:
	//! ��������� ��� �������� ������
	struct CameraThreadParam
	{
		//! this
		CameraServer*	m_pServer;
		//! ������������� ������
		int				m_CameraNo;

		CameraThreadParam( CameraServer* p, int n ) : m_pServer(p), m_CameraNo(n){}
	};

	//! ���������� try-catch ��������� ������
	//! \param pParam ��������� ������
	//! \return 0 - �����, -1 - ������
	static unsigned int WINAPI Handled_CameraAnalyzerThreadProc(void * pParam);

	typedef boost::shared_ptr< CameraThread >	CameraAnalyzer_t;
	typedef std::vector< CameraAnalyzer_t >		CameraAnalyzerArr_t;

	//! ������������� ��������� �� �� � ��������� ���������
	//! \param cc ������������ ��������� �����
	//! \param Data ������ ��� ���������
	//! \return S_OK - success, other - failure
	HRESULT		LoadAlgoParam( const LocalCameraConfig& cc, Int8Arr& Data );

	//! ���������� �������� � ������, �� ������� CSLauncher
	//! \param nCameraNo ������������� ������
	//! \param pAnalyzer ��������� ���������
	void		RenderCamera( int nCameraNo, ICameraAnalyzer* pAnalyzer );
private:
	CameraAnalyzerArr_t							m_CameraAnalyzers;
protected:
	std::vector<LocalCameraConfig>				m_LocalCameras;
private:
	SyncCriticalSection							m_CameraThreadCS;
protected:

	//! ����� ������������ ��� ������, ���������������
	//! \param CameraNo ������������� ������
	//! \return ��������� �� ������������
	const LocalCameraConfig*	FindLocalCameraConfig	(int CameraNo);
	//! ����� ����� ������
	//! \param CameraNo ������������� ������, ���������������
	//! \return ��������� ������
	CameraThread*				FindCameraThread		(int CameraNo);
	//! ����� ��������� ���������, ���������������
	//! \param CameraNo ������������� ������
	//! \return ��������� ��������� 
	ICameraAnalyzer*			GetCameraAnalyzer		(int CameraNo);

	//! ������� ������������
	IGrabberFactory*			m_pGrabberFactory;
	//! ���� ��� �������
	std::map<int, HWND>			m_CameraWindows;

	
	//! ���������� ��� ������ �����
	//! \return S_OK - success, other - failure
	HRESULT	StartCameraThreads( CTime timeBegin, CTime timeEnd );
	//! ���������� ��� ������ �����
	//! \return S_OK - success, other - failure
	HRESULT StopCameraThreads();

private:
	//! ���������� ����� ������, ������ ��������� ��������������
	HRESULT	ResumeCameraThreads();
	//! ���������� �����
	//! \param nIndex ������ � ������� ������������ ������
	//! \param bSuspend ������� �������������?
	//! \return S_OK - success, other - failure
	HRESULT	StartCameraAnalyzer(size_t nIndex, CTime timeBegin, CTime timeEnd , bool bSuspend);

	bool	SetFilmTime( ICameraAnalyzer* pInt, int nCameraNo, CTime timeBegin, CTime timeEnd) ;

	std::pair<CTime, CTime>	m_LastFilmTime;
protected:
	//! �������� ������� (���������� ����� ������)
	//! \return S_OK - success, other - failure
	HRESULT	CheckCameraThreads( );
public:
	CameraServer();
	virtual ~CameraServer();
};

#endif //__CAMERA_SERVER_H_INCLUDED_5433557040532002__
