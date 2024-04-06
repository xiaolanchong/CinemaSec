#pragma once

#if _MSC_VER <= 1200
#pragma warning( disable : 4097 )
#endif 

#include "../CommonView/wv_message.h"
#include "../CommonView/ArrangeDocument.h"
#include "../video/CameraManager.h"
#include "ArchiveDecompressor.h"

//#define USE_TEST_ARCHIVE

class CArchiveView;
class CArchiveStatView;
class CVideoControlView;



class CArchiveDoc : public CArrangeDocument
{
	CVideoControlView*	GetVideoControl();
	void				CreateTestArchiveImage( CTime time );
	void				UpdateImage( bool bSetTime);
	void				CreateLocationHeader( int nRoomID );

	std::vector<int> m_CameraArr;
	typedef std::map< int, boost::shared_ptr<IArchiveGrabber> >	ArchiveGrabberMap_t;
	ArchiveGrabberMap_t m_ArchiveGrabberMap;

	ArchiveDecompressor		m_arc;
	CTime					m_timeLastFrame;
	CTime					m_timeBegin;
	bool					m_bDragged;
protected:
  DECLARE_DYNCREATE(CArchiveDoc)
  CArchiveDoc();
  virtual ~CArchiveDoc();

	virtual void OnActivateMode( bool bActivate);
	void	SeekVideoInternal( CTime timeSeek, bool bVideo, bool bUpdateImage );
public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

	void			SeekArchive( int nFilmID);
	void			UpdateArchive( CTime timeBegin, CTime timeEnd );	

	virtual void	LoadImageRoomState	( int nStatID );
	virtual void	LoadRoom( int nRoomID);
	virtual void	LoadArchive( const std::vector<int>& CameraArr );

	void	SetCurrentTime( CTime time );

	void	SeekVideo( CTime timeSeek, bool bUpdateImage = true );
	void	StepVideo( CTime timeStep );

	void	StepVideo();
	void	StopVideo();
	void	PauseVideo();
	void	PlayAfterPause();

	void	StartDrag();
	void	StopDrag();

  //{{AFX_MSG(CArchiveDoc)

  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


