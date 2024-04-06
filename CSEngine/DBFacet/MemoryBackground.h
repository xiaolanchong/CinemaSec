//=====================================================================================//
//                                                                                     //
//                                       CSChair                                       //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   01.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __MEMORY_BACKGROUND_H_
#define __MEMORY_BACKGROUND_H_

namespace MemoryBackground
{
	
bool MemoryLoadFloatImage( const BYTE* pData, size_t DataLen, Arr2f & image, bool bInvertY );
bool MemorySaveFloatImage( std::vector<BYTE>& Arr, const Arr2f & image, bool bInvertY );

}

#endif //__MEMORY_BACKGROUND_H_