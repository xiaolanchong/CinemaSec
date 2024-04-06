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
#ifndef _CHAIR_DISK_8441052881926968_
#define _CHAIR_DISK_8441052881926968_

#include "../../CSAlgo/Algorithm/base_chair.h"

//======================================================================================//
//                                   class ChairDisk                                    //
//======================================================================================//

//! представление кресла для записи в документ
struct ChairDisk_t
{
	Vec2i          center;          //!< the central point of chair
	int            subFrameNo;      //!< the index of control area
	int            id;              //!< unique identifier of this chair
	Vec2fArr       curve;			//!< the curve that outlines the main zone inside a chair	
	int	nRight,						//!< the index of the right neighbour chair
		nLeft;						//!< the index of the left neighbour chair
	int nBottom[3];					//!< the indices of the bottom 3 neighbour chairs
	int nTop[3];					//!< the indices of the top 3 neighbour chairs
};

//! для простоты обращения
typedef std::vector<ChairDisk_t> ChairDiskArr_t;

//! разделение общего массива кресел по камерам
//! \param	ChairMap	карта ID камеры-разметка
//!	\param	ChairArr	разметка по всему залу
bool DivideByZone( std::map<int, ChairDiskArr_t>& ChairMap, const ChairDiskArr_t& ChairArr ) ;

#endif // _CHAIR_DISK_8441052881926968_