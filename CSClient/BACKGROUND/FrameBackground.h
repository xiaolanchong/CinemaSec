// FrameBackground.h: interface for the CFrameBackground class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAMEBACKGROUND_H__94741B64_BA06_4EB6_A0A6_724759AF68EE__INCLUDED_)
#define AFX_FRAMEBACKGROUND_H__94741B64_BA06_4EB6_A0A6_724759AF68EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Background.h"
#include <boost/shared_ptr.hpp>
#if 0
#include "../../CSUtility/utility/base_frame_grabber.h"
#endif
#include "../../CSChair/Grabber/IGrabberFactory.h"
#include "../../CSChair/Grabber/ImageDecompressor.h"

#if 0
class CFrameBackground : public CBackground  
{
protected:
	boost::shared_ptr<BaseFrameGrabber> 	m_pGrabber;
	typedef boost::shared_ptr<HIC>			HicPtr_t;
	mutable HicPtr_t						m_pCompressor;
	mutable CSize							m_sizeCachedLastSize;
	mutable std::vector<BYTE>				m_DecompressedBuffer;

	//mutable HIC		m_hCompressor;
	void	InitCompressor( const BITMAPINFOHEADER& bihIn ) const;
	void	DrawError( Graphics& gr, const CRectF& rc ) const;
	static void	DeleteCompressor(HIC* phComressor);
public:
	CFrameBackground( boost::shared_ptr<BaseFrameGrabber> pGrabber ) ;
	virtual ~CFrameBackground();
	

	virtual void	Draw(Graphics& gr, const CRectF& rc) const ;
	virtual CSize	GetSize()							 const ;
};

#endif

class CFrameBackground : public CBackground  
{
protected:
	boost::shared_ptr<IBaseGrabber> 				m_pGrabber;
	mutable boost::shared_ptr<ImageDecompressor>	m_Decomressor;	

	mutable CSize							m_sizeCachedLastSize;
	mutable std::vector<BYTE>				m_DecompressedBuffer;

	bool			InitCompressor( const BITMAPINFOHEADER& bihIn ) const;
	void			DrawError( Graphics& gr, const CRectF& rc, LPCWSTR szDesc ) const;
public:
	CFrameBackground( boost::shared_ptr<IBaseGrabber> pGrabber ) ;

	boost::shared_ptr<IBaseGrabber>			GetGrabber()		{ return m_pGrabber;	}
	boost::shared_ptr<ImageDecompressor>	GetDecompressor()	{ return m_Decomressor;	}

	virtual void	Draw(Graphics& gr, const CRectF& rc) const ;
	virtual CSize	GetSize()							 const ;
};


#endif // !defined(AFX_FRAMEBACKGROUND_H__94741B64_BA06_4EB6_A0A6_724759AF68EE__INCLUDED_)
