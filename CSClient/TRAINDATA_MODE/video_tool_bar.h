#ifndef MY_VIDEO_TOOL_BAR_H
#define MY_VIDEO_TOOL_BAR_H

class CTrainDataFrame;

#include "RangeCtrl.h"
#include "../GUI/AlphaToolbar/AlphaToolbar.h"

class VideoToolBar : public CAlphaToolBar
{
private:
  enum { FRAME_SCROLL_WIDTH  = 300,
         FRAME_SCROLL_HEIGHT = 20 };

  CTrainDataFrame * m_pParent;          //!< pointer to the parent frame window
  int              m_playButtonIndex;  //!< the index of the play button of the toolbar
  UINT             m_playButtonStyle;  //!< the style of the play button of the toolbar
  int              m_playButtonImage;  //!< the index of an image of the play button of the toolbar

  std::vector<Range::CONTENT>	m_StringArray;

public:
	CRangeCtrl	m_frameScroll;
//	CComboBox	m_wndRangeSelect;

	Range::CONTENT		GetCurrentContent() const
	{
/*		int z = m_wndRangeSelect.GetCurSel();
		// m_StringArray is much less MAX_INT
		ASSERT( z != LB_ERR && z < int( m_StringArray.size() ) );
		return m_StringArray[z];*/
		return m_frameScroll.GetCurrentRangeContent();
	}


  VideoToolBar();
  virtual ~VideoToolBar();

  BOOL Init( CTrainDataFrame * pWindow, BOOL bToolTips = FALSE );
  void SetPlayButtonImage( bool bPlay );

  void	StartEdit();
  void	EndEdit();
  void	CancelEdit();

protected:
  afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar * pScrollBar );
  DECLARE_MESSAGE_MAP()
};

#endif // MY_VIDEO_TOOL_BAR_H


