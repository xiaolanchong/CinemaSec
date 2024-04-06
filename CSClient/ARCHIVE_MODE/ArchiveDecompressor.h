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
// Date   : 30.06.2005
//                                                                                      //
//======================================================================================//
#ifndef _ARCHIVE_DECOMPRESSOR_8440041511954709_
#define _ARCHIVE_DECOMPRESSOR_8440041511954709_

#include "../../CSChair/common/Exception.h"
#include "../../CSChair/Grabber/ImageDecompressor.h"
//======================================================================================//
//                              class ArchiveDecompressor                               //
//======================================================================================//

MACRO_EXCEPTION( ArchiveDecompressorException, CommonException );

class ArchiveDecompressor
{
	mutable std::vector<BYTE>						m_cacheByte;
	mutable boost::shared_ptr<ImageDecompressor>	m_Decompressor;
	mutable BITMAPINFO								m_cacheHdr;

	bool			InitCompressor( const BITMAPINFOHEADER& bihIn );
public:
	void Process(	const BITMAPINFO* pSrcHdr, const BYTE* pSrcByte, 
					const BITMAPINFO*& pDstHdr, const BYTE*& pDstByte );
};

#endif // _ARCHIVE_DECOMPRESSOR_8440041511954709_