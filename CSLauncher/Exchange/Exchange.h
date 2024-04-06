//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Exchange component singleton
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 31.05.2005
//                                                                                      //
//======================================================================================//
#ifndef _EXCHANGE_6940560906616922_
#define _EXCHANGE_6940560906616922_
//======================================================================================//
//                                    class Exchange                                    //
//======================================================================================//

struct ICallbackServer;
struct IDebugOutput;

void	CreateExchange();
void	StartExchange( IDebugOutput *pDbgInt); 
void	DestroyExchange();
HRESULT	RequestExchange( BSTR sQuery );

ICallbackServer*	GetCallbackServerInt();

#endif // _EXCHANGE_6940560906616922_