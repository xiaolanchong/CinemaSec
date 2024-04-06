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
// Date   : 12.08.2005
//                                                                                      //
//======================================================================================//
#ifndef _I_INPUT_MESSAGE_7144749753950385_
#define _I_INPUT_MESSAGE_7144749753950385_
//======================================================================================//
//                                 struct IInputMessage                                 //
//======================================================================================//

struct IInputBuffer
{
	typedef std::queue< std::wstring >	InputBuffer_t;

	virtual		InputBuffer_t&	Lock()		= 0;
	virtual		void			Unlock()	= 0;
	virtual		void			RegisterWnd( HWND hWnd, UINT msg) = 0;
	virtual		bool			PutIncomingMessage( const std::wstring& sText) = 0;
};

IInputBuffer&	GetInputBuffer();

#endif // _I_INPUT_MESSAGE_7144749753950385_