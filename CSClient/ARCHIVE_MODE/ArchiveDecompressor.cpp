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
#include "stdafx.h"
#include "ArchiveDecompressor.h"
//======================================================================================//
//                              class ArchiveDecompressor                               //
//======================================================================================//

bool	ArchiveDecompressor::InitCompressor( const BITMAPINFOHEADER& bihIn ) 
{
	try
	{
		m_Decompressor  = boost::shared_ptr<ImageDecompressor>
			( new ImageDecompressor( bihIn, bihIn.biWidth, bihIn.biHeight, 24 ));
		m_cacheHdr.bmiHeader = m_Decompressor->GetDstFormat();
		return true;
	}
	catch(ImageDecompressorException)
	{
		return false;
	}
}

void ArchiveDecompressor::Process(	const BITMAPINFO* pSrcHdr,	const BYTE* pSrcByte, 
				const BITMAPINFO*& pDstHdr, const BYTE*& pDstByte )
{
	if( !pSrcHdr || !pSrcByte) throw ArchiveDecompressorException( "Empty data" );
	if( pSrcHdr->bmiHeader.biCompression != BI_RGB )
	{
		if( !m_Decompressor )
		{
			bool bInit = InitCompressor( pSrcHdr->bmiHeader );
			if( !bInit) throw ArchiveDecompressorException( "Failed to init" );
		}
		if( !m_Decompressor )
		{
			throw ArchiveDecompressorException( "Not initialized" );
		}
		else 
		{
			bool res = m_Decompressor->Decompress( pSrcByte, m_cacheByte );
			if(res)
			{
				pDstByte	= &m_cacheByte[0];
				pDstHdr		= &m_cacheHdr;
			}
			else 
				throw ArchiveDecompressorException( "Decompression failed" );
		}
	}
	else
	{
		pDstHdr		= pSrcHdr;
		pDstByte	= pSrcByte;
	}
}