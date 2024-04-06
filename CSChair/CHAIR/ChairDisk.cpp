//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Chair serializer representation structure
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 31.05.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "ChairDisk.h"
//======================================================================================//
//                                   class ChairDisk                                    //
//======================================================================================//

bool DivideByZone( std::map<int, ChairDiskArr_t>& ChairMap, const ChairDiskArr_t& ChairArr )
{
	ChairMap.clear();
	for( size_t i = 0; i < ChairArr.size(); ++i )
	{
		if( !ChairArr[i].curve.empty() )
		{
			ChairMap[ ChairArr[i].subFrameNo ].push_back( ChairArr[i] );
		}
	}
	return true;
}