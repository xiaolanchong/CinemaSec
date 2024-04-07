//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	��������� ��������� ������� ��� �������� �����
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
	\brief	���������� �������� ��������� ��������
	\return \li NULL - �������� �� ��������� (������� � ����)	
			\li ����� - ����� ��������� ������� (�������� ����� new)
*/	
	virtual IFileTransferState*	Process() = 0;

/**
     \brief ������ �������� ���������
 */

	virtual void Cancel() = 0;

/**
     \brief		������� ��������� ��������� �������
	 \return	��������� ��������� ������� (�������� new)
 */
	static IFileTransferState*	GetStartState(FileTransferServer* pTransfer, int nCameraID);
	/**
	\brief		�������� ��������� ��������� ��������
	\return		\li	true - �������� ���������, ������ ������ ���������
				\li	false - ���������� ������
	*/
	static bool					IsEndState( IFileTransferState* pState );
};

#endif // _I_FILE_TRANSFER_STATE_3283603913697854_