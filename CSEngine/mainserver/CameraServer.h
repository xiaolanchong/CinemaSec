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

//! данные от камеры
struct CameraDataForProcessing
{
	//! идентификатор камеры
	int					m_nCameraNo;
	//! данные от ICameraAnalyzer
	std::vector<BYTE>	m_Data;
	//! изображение в формате JPEG
	std::vector<BYTE>	m_Image;

	CameraDataForProcessing(int id, const std::vector<BYTE>& d, const std::vector<BYTE>& img):
		m_Image(img),
		m_Data(d),
		m_nCameraNo(id)
	{
	}
};

//! \brief класс потока обработки изображений камеры
//! \version 1.1
//! \date 10-26-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//! \li 2005-10-26 передача данных от IHallAnalyzer
class CameraServer : public virtual GenericThreadServer
{
protected:
	//! класс потока камеры
	class CameraThread
	{
		//! интерфейс алгоритма
		ICameraAnalyzer*						m_pAnalyzer;
	
	protected:
		//! очередь сообщений
		ThreadMessage				m_CameraMessage;
		//! вытолкнуть сообщения
		bool						PopThreadMessage( CameraServer* pServer );
		//! занести в очередь
		void						PushImageAndData(	CameraServer* pServer,
			const std::vector<BYTE>& ImageBuf, 
			const void* pData, size_t nSize  );
		//! обработать данные от IHallAnalyzer 
		void						SetHallData( CameraServer* pServer, const std::vector<BYTE>& Data );
		//! идентификатор камеры
		int										m_nCameraNo;
	public:
		//! синхронизация потока
		ThreadSync		m_Thread;
		//! возвращает идентификатор камеры
		//! \return идентификатор камеры
		int					GetCameraNo() const { return m_nCameraNo;};
		//! вернуть интерфейс алгоритма
		//! \return интерфейс алгоритма
		ICameraAnalyzer*	GetAnalyzer() { return m_pAnalyzer; }
		//! отправить сообщение в очередь основного потока
		//! \param Id идентификатор сообщения
		//! \param Data данные сообщения
		void				PushMessage( int Id, const boost::any&	Data )
		{
			ThreadMessageAutoEnter me( m_CameraMessage, true );
			m_CameraMessage.Push( me, Id, Data );
		}

		//! потоковая ф-я
		//! \param pParam параметр для потока 
		//! \return 0 - успех, -1 - ошибка
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
	//! структура для передачи потоку
	struct CameraThreadParam
	{
		//! this
		CameraServer*	m_pServer;
		//! идентификатор камеры
		int				m_CameraNo;

		CameraThreadParam( CameraServer* p, int n ) : m_pServer(p), m_CameraNo(n){}
	};

	//! защищенная try-catch процедура потока
	//! \param pParam параметра потока
	//! \return 0 - успех, -1 - ошибка
	static unsigned int WINAPI Handled_CameraAnalyzerThreadProc(void * pParam);

	typedef boost::shared_ptr< CameraThread >	CameraAnalyzer_t;
	typedef std::vector< CameraAnalyzer_t >		CameraAnalyzerArr_t;

	//! преобразовать параметры из БД в параметры алгоритма
	//! \param cc конфигурация локальных камер
	//! \param Data данные для алгоритма
	//! \return S_OK - success, other - failure
	HRESULT		LoadAlgoParam( const LocalCameraConfig& cc, Int8Arr& Data );

	//! отобразить картинку с камеры, на вкладке CSLauncher
	//! \param nCameraNo идентификатор камеры
	//! \param pAnalyzer интерфейс алгоритма
	void		RenderCamera( int nCameraNo, ICameraAnalyzer* pAnalyzer );
private:
	CameraAnalyzerArr_t							m_CameraAnalyzers;
protected:
	std::vector<LocalCameraConfig>				m_LocalCameras;
private:
	SyncCriticalSection							m_CameraThreadCS;
protected:

	//! найти конфигурацию для камеры, потокобезопасен
	//! \param CameraNo идентификатор камеры
	//! \return указатель на конфигурацию
	const LocalCameraConfig*	FindLocalCameraConfig	(int CameraNo);
	//! найти поток камеры
	//! \param CameraNo идентификатор камеры, потокобезопасен
	//! \return структура потока
	CameraThread*				FindCameraThread		(int CameraNo);
	//! найти интерфейс алгоритма, потокобезопасен
	//! \param CameraNo идентификатор камеры
	//! \return интерфейс алгоритма 
	ICameraAnalyzer*			GetCameraAnalyzer		(int CameraNo);

	//! фабрика видеопотоков
	IGrabberFactory*			m_pGrabberFactory;
	//! окна для отладки
	std::map<int, HWND>			m_CameraWindows;

	
	//! стартовать все потоки камер
	//! \return S_OK - success, other - failure
	HRESULT	StartCameraThreads( CTime timeBegin, CTime timeEnd );
	//! остановить все потоки камер
	//! \return S_OK - success, other - failure
	HRESULT StopCameraThreads();

private:
	//! стартовать после начала, потоки создаются остановленными
	HRESULT	ResumeCameraThreads();
	//! стартовать поток
	//! \param nIndex индекс в массиве конфигураций камеры
	//! \param bSuspend создать остановленным?
	//! \return S_OK - success, other - failure
	HRESULT	StartCameraAnalyzer(size_t nIndex, CTime timeBegin, CTime timeEnd , bool bSuspend);

	bool	SetFilmTime( ICameraAnalyzer* pInt, int nCameraNo, CTime timeBegin, CTime timeEnd) ;

	std::pair<CTime, CTime>	m_LastFilmTime;
protected:
	//! проверка потоков (перезапуск после выхода)
	//! \return S_OK - success, other - failure
	HRESULT	CheckCameraThreads( );
public:
	CameraServer();
	virtual ~CameraServer();
};

#endif //__CAMERA_SERVER_H_INCLUDED_5433557040532002__
