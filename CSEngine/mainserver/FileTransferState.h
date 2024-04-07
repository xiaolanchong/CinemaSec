//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Интерфейс состояния сервера при передачи файла
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 01.08.2005
//                                                                                      //
//======================================================================================//
#ifndef _I_FILE_TRANSFER_STATE_3283603913697854_
#define _I_FILE_TRANSFER_STATE_3283603913697854_
//======================================================================================//
//                               class IFileTransferState                               //
//======================================================================================//

class FileTransferServer;

struct IFileTransferState
{
	int					m_nCameraID;
protected:
	FileTransferServer*	m_pTransfer;
	int GetCameraID() const { return m_nCameraID;}

	IFileTransferState(FileTransferServer* pTransfer, int nCameraID):
		m_pTransfer(pTransfer),
		m_nCameraID(nCameraID)
		{};
public:

	virtual ~IFileTransferState() = default;
/**
	\brief	Выполнение операции состояния автомата
	\return \li NULL - операция не завершена (переход в себя)	
			\li иначе - новое состояние сервера (выделена через new)
*/	
	virtual IFileTransferState*	Process() = 0;

/**
     \brief Отмена операций состояния
 */

	virtual void Cancel() = 0;

/**
     \brief		Создать начальное состояние сервера
	 \return	Начальное состояние сервера (оператор new)
 */
	static IFileTransferState*	GetStartState(FileTransferServer* pTransfer, int nCameraID);
	/**
	\brief		Проверка конечного состояния автомата
	\return		\li	true - конечное состояние, сервер должен завершить
				\li	false - продолжить работу
	*/
	static bool					IsEndState( IFileTransferState* pState );
};

#endif // _I_FILE_TRANSFER_STATE_3283603913697854_