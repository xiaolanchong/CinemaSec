//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Input message for CinemaSec exchange component from BO
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 25.05.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "InputMessage.h"
#include "MessageProcessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//======================================================================================//
//                                  class InputMessage                                  //
//======================================================================================//
InputMessage::~InputMessage()
{
}

void	BO_ErrorMessage::Dump(MyDebugOutputImpl& Debug) const
{
	Debug.PrintW( IDebugOutput::mt_debug_info, L"[BO_ErrorMessage] RefID=%d", GetRefID() );
	for( size_t i = 0; i < GetErrors().size(); ++i )
	{
		Debug.PrintW( IDebugOutput::mt_debug_info, L"[BO_ErrorMessage] res=%d, desc=%s",
						GetErrors()[i].first, GetErrors()[i].second.c_str()  );
	}
}

void	CCS_ErrorMessage::Dump(MyDebugOutputImpl& Debug) const
{
	Debug.PrintW( IDebugOutput::mt_debug_info, L"[CCS_ErrorMessage] res=%d, desc=%s",
		GetErrorID(), GetDesc().c_str() );
}

std::wstring	CCS_ErrorMessage::GetText() const
{
	MessageProcessor msg(NULL);
	return msg.GetTextMessage( *this );
}

void			CCS_ReportMessage::Dump(MyDebugOutputImpl& Debug) const
{

}

std::wstring	CCS_ReportMessage::GetText() const
{
	MessageProcessor msg(NULL);
	return msg.GetTextMessage( *this );
}