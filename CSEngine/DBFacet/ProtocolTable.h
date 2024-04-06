//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Description is not available
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 12.09.2005
//                                                                                      //
//======================================================================================//
#ifndef _PROTOCOL_TABLE_3803575690956641_
#define _PROTOCOL_TABLE_3803575690956641_
//======================================================================================//
//                                 class ProtocolTable                                  //
//======================================================================================//

#include "DBRows.h"
#include "DBHelper.h"
#include "OleDBConnection.h"

class ProtocolTable :	protected virtual COleDBConnection,
						public DBProtocolTable
{
public:
	ProtocolTable();
	virtual ~ProtocolTable();

	virtual void	AddMessage( int nCinemaID, MsgType mt, CTime timeMsg, const std::wstring& sText );
	virtual void	GetMessage( int nCinemaID, CTime timeFrom, CTime timeTo, std::vector<Message_t> & Msgs );
};

#endif // _PROTOCOL_TABLE_3803575690956641_