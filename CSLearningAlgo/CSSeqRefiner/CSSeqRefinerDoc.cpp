// CSSeqRefinerDoc.cpp : implementation of the CCSSeqRefinerDoc class
//

#include "stdafx.h"
#include "CSSeqRefiner.h"

#include "CSSeqRefinerDoc.h"
#include ".\csseqrefinerdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCSSeqRefinerDoc

IMPLEMENT_DYNCREATE(CCSSeqRefinerDoc, CDocument)

BEGIN_MESSAGE_MAP(CCSSeqRefinerDoc, CDocument)
END_MESSAGE_MAP()


// CCSSeqRefinerDoc construction/destruction

CCSSeqRefinerDoc::CCSSeqRefinerDoc()

{
	// TODO: add one-time construction code here

}

CCSSeqRefinerDoc::~CCSSeqRefinerDoc()
{
}

BOOL CCSSeqRefinerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	mImageCount = 0;
	mImageWidth = -1;
	mImageHeight = -1;


	return TRUE;
}




// CCSSeqRefinerDoc serialization

void CCSSeqRefinerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CCSSeqRefinerDoc diagnostics

#ifdef _DEBUG
void CCSSeqRefinerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCSSeqRefinerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCSSeqRefinerDoc commands
BOOL CCSSeqRefinerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	boost::shared_ptr<avideolib::IVideoReader> videoReader( avideolib::CreateAVideoReader( lpszPathName, NULL, true ) );
	if( NULL == videoReader.get() )
		return FALSE;

	int frameNumber;
	int milliTime;
	Arr2ub umImage;
	alib::TStateFlag flag;


	flag.data = avideolib::IBaseVideo::FRAME_NUMBER;
	mImageCount = 0;
	if( videoReader.get() != 0 )
		if ( videoReader->GetData( &flag ) )
			mImageCount = flag.data;

	flag.data = avideolib::IBaseVideo::WIDTH;
	if( videoReader.get() != 0 )
		if ( videoReader->GetData( &flag ) )
			mImageWidth =  flag.data;

	flag.data = avideolib::IBaseVideo::HEIGHT;
	if( videoReader.get() != 0 )
		if ( videoReader->GetData( &flag ) )
			mImageHeight = flag.data;

	try
	{
		for( int desiredFrame = 0; 
			( desiredFrame < mImageCount ) && ( avideolib::IVideoReader::ReturnCode::NORMAL == videoReader->ReadFrame( &umImage, desiredFrame, &frameNumber, &milliTime ) );
			++desiredFrame	)
		{
			mImages.push_back( std::make_pair( umImage, milliTime ) );
		}

		return TRUE;
	}
	catch( ... )
	{
		return FALSE;
	}
}

BOOL CCSSeqRefinerDoc::OnSaveDocument(LPCTSTR lpszPathName)
{

	boost::shared_ptr<avideolib::IVideoWriter> videoWriter( avideolib::CreateAVideoWriter( lpszPathName, NULL, false, INVERT_AXIS_Y ) );
	if( NULL == videoWriter.get() )
		return FALSE;

	Arr2ub umImage;

	try
	{
		std::vector< std::pair<Arr2ub, unsigned __int32> >::iterator it;
		std::vector< std::pair<Arr2ub, unsigned __int32> >::iterator begin_it = mImages.begin();
		std::vector< std::pair<Arr2ub, unsigned __int32> >::iterator end_it = mImages.end();

		for( it = begin_it; it != end_it; ++it )
		{
			Arr2ub* pImage = &it->first;
			unsigned __int32 milliTime = it->second;
			videoWriter->WriteFrame( pImage, milliTime  );
		}
	}
	catch( ... )
	{
		return FALSE;
	}

	return TRUE;

}

//////////////////////////////////////////////////////////////////////////
int CCSSeqRefinerDoc::GetImageWidth()
{
	return mImageWidth;
}

int CCSSeqRefinerDoc::GetImageHeight()
{
	return mImageHeight;
}

bool CCSSeqRefinerDoc::SetImageWidth( int w )
{
	if ( mImageWidth == -1 || w == mImageWidth ) 
	{
		 mImageWidth = w;
		 return true;
	}
	else
	{
		return false;
	}
}
bool CCSSeqRefinerDoc::SetImageHeight( int h )
{
	if ( mImageHeight == -1 || h == mImageHeight ) 
	{
		mImageHeight = h;
		return true;
	}
	else
	{
		return false;
	}
}

int CCSSeqRefinerDoc::GetImageCount()
{
	return mImageCount;
}

void CCSSeqRefinerDoc::GetImage( int frameNum, unsigned __int32& frameTime, Arr2ub& image )
{
	image = mImages[ frameNum ].first;
	frameTime = mImages[ frameNum ].second;
}

void CCSSeqRefinerDoc::SetImage( int frameNum, unsigned __int32 frameTime, const Arr2ub& image )
{
	mImages[ frameNum ] = std::make_pair( image, frameTime );
}

void CCSSeqRefinerDoc::AddImage( unsigned __int32 frameTime, const Arr2ub& image )
{
	mImages.push_back( std::make_pair( image, frameTime ) );
}
void CCSSeqRefinerDoc::RemoveImage( int frameNum )
{
	mImages.erase( mImages.begin() + frameNum );
}

//////////////////////////////////////////////////////////////////////////
