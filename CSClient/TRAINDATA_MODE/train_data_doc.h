#pragma once

#ifndef  MY_AVI_IO_H
#include "../Video/avi_io.h"
#endif

#ifndef  MY_ONE_IMAGE_LAYOUT_H
#include "../../CSUtility/utility/layout.h"
#endif

#include "../CSChair/IChairContentEx.h"
#include "ContentSerializer.h"
#include "../DataPrepare/IFrameIterator.h"

#include "../Video/aviioshow.h"
#include "../Video/aviioGrabber.h"

#include "OffsetChairDialog.h"
#include "../Chair/ChairLink.h"


class CTrainDataFrame;
class CTrainDataView;

class CTrainDataDoc :	public CDocument, 
						public IFrameIterator,
						public IOffsetCallBack
{
	IChairContentEx*	m_pChairInt;
	struct ChairTrain
	{
		MyChair::ChairSet::const_iterator	m_itChair;
		RangeList_t					m_ChairContent;

		ChairTrain() {}
		ChairTrain(const ChairTrain& ct) : 
			m_itChair(ct.m_itChair),
			m_ChairContent(ct.m_ChairContent)
			{}

			ChairTrain(MyChair::ChairSet::const_iterator	itChair) : m_itChair(itChair) {}
	};

	void	ConvertToTrainList();
	Range::CONTENT GetContentInFrame( int nID, int nPos) const;

	CString		m_strVideoFilePath;
	CString		m_strGaugeFilePath;

	void		ChangeContentRange( Range::CONTENT Want );
	bool		CheckCurrentRange(Range::CONTENT Want) ;

	virtual void Move(int x, int y);
public:
#if 0
  AviIO            m_avi;        //!< the currently opened AVI stream
#else
  //AviIOShow			m_avi;		// direct show implementation
	AviIOGrabber		m_avi;
#endif
  CTrainDataFrame * m_pFrameWin;  //!< pointer to the frame window
  MyChair::ChairSet			m_ChairSet;
  ChairTrainList_t	m_ChairTrain;
  ChairLinkList_t		m_ChairLink;
  OneImageLayout  m_layout;       //!< layout of a single image on the screen

  Arr2ub   m_tempBackgr;      //!< temporal background image
  UByteArr m_tempBuffer;      //!< temporal buffer

  MyChair::ChairSet::const_iterator m_itCurrentChair;

  	COffsetChairDialog m_dlgMove;

	int					m_nCurrentCameraNo;
  void RecalcLayout(bool bUpdate);

  int	GetFirstZoneNumber() const;
protected:
  CTrainDataDoc();
  DECLARE_DYNCREATE(CTrainDataDoc)
  // void GetFrameWindow();
  CTrainDataView * GetTrainDataView();

  // IFrameIterator
	virtual int GetTotalFrameNum() ;
	virtual int GetBackGround(int nFrameNum, Arr2ub& bg) ;
	virtual int GetChairs    (int nFrameNum, MyChair::ChairSet& chairs) ;

	
public:

	int		GetCurrentCameraNo() const { return m_nCurrentCameraNo;} ;
  virtual void DeleteContents();

  void		OnLButtonDownHandler( UINT nFlags, CPoint point );
  Range::CONTENT GetCurrentContent( int nID) const;

public:
  virtual ~CTrainDataDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

protected:
  DECLARE_MESSAGE_MAP()
  afx_msg void OnFileOpen();

  afx_msg void OnFileOpenGraph();
  afx_msg void OnFileSaveGraph();

  afx_msg void OnFileOpenTrainData();
  afx_msg void OnFileSaveTrainData();

  afx_msg void OnRangeStart();
  afx_msg void OnRangeCancel();
  afx_msg void OnRangeEnd();
 
  afx_msg void OnRangeStartUpdate(CCmdUI* pCmdUI);
  afx_msg void OnRangeEndUpdate(CCmdUI* pCmdUI);
  afx_msg void OnRangeCancelUpdate(CCmdUI* pCmdUI);  

	afx_msg void OnRangeButtonHuman();
	afx_msg void OnRangeButtonEmpty();
	afx_msg void OnRangeButtonClothes();
	afx_msg void OnRangeButtonUnknown();

	afx_msg void OnRangeButtonHumanUpdate(CCmdUI* pCmdUI);
	afx_msg void OnRangeButtonEmptyUpdate(CCmdUI* pCmdUI);
	afx_msg void OnRangeButtonClothesUpdate(CCmdUI* pCmdUI);
	afx_msg void OnRangeButtonUnknownUpdate(CCmdUI* pCmdUI);
	
	afx_msg void OnMoveChairs();

	afx_msg void	OnLoadAll();
	afx_msg void	OnSaveAll();
};


inline void CopyMemoryWithOffset(void* pDst, const BYTE*& pSrc, size_t Size)
{
  memcpy( pDst, pSrc, Size );
  pSrc += Size;
}