#pragma once

#ifndef  VISLIB_LIVE_SNAKE_H
#include "utility/live_snake.h"
#endif

#ifndef  KERNEL_H
#endif

#ifndef  MY_ONE_IMAGE_LAYOUT_H
#include "../../CSUtility/utility/layout.h"
#endif

#include "../DataPrepare/my_chair.h"

#include "MultiImageLayout.h"
#include "CacheDC.h"
#include "LineCache.h"
#include "../Chair/UtilityChair.h"
#include "../Chair/ChairLink.h"
#include "../TRAINDATA_MODE/OffsetChairDialog.h"
#include "../../CSChair/chair_int.h"

#include <boost/shared_ptr.hpp>

const bool     PRODUCE_CLOSED_CURVE = true;
const double   CURVE_SMOOTHING_SIGMA = 3.0;
const COLORREF CURVE_COLOR = RGB(0,255,0);
const COLORREF ANIMATED_CURVE_COLOR = RGB(0,128,255);
const COLORREF SELECTED_ANIMATED_CURVE_COLOR = RGB(255,0,0);

class CGaugeHallView;
class CGaugeHallFrame;
class CGaugeHallDoc;

const int MAX_FRAME = 6;

class CMoveImageCallback : public IOffsetCallBack
{
	CGaugeHallDoc* m_pDoc;
public:
	virtual void Move(int x, int y);
	CMoveImageCallback( CGaugeHallDoc* pDoc ): m_pDoc(pDoc){}
};

class CMoveChairCallback : public IOffsetCallBack
{
	CGaugeHallDoc* m_pDoc;
public:
	virtual void Move(int x, int y);
	CMoveChairCallback( CGaugeHallDoc* pDoc ): m_pDoc(pDoc){}
};

// NOTE for db version
// all data in arrays
// assume config is not changed while working
// if not use db, only one room is accessible and all cameras in map owns him

class CGaugeHallDoc : public CDocument
{
	friend class CMoveImageCallback;
	friend class CMoveChairCallback;
	bool	m_bStandAlone;
	// unique for each room
	std::map<int, MyChair::ChairSet>	m_ChairSet;
	std::map<int, ChairLinkList_t>	m_ChairLink;

	struct ImageData
	{
		Arr2f               m_hallImage;     //!< an image of empty hall
	  Arr2f               m_gradients;     //!< equated gradients of an image
	  Arr2ub              m_demoImage;     //!< demo version of the hall image
	  Vec2fImg            m_energyGrads;   //!< gradient vectors of potential energy
	  uint                m_subMode;       //!< the current editing sub-mode
	  int                 m_adjustRadius;  //!< the radius of curve adjusting operator
	  Vec2f               m_headPt;        //!< head point of the current snake
	  Vec2f               m_tailPt;        //!< tail point of the current snake
	  Vec2fArr            m_curve;         //!< the current contour drawn around an object of interest
	  Vec2fArr            m_auxCurve;      //!< auxiliary curve
	  PixColArr           m_pixCurve;      //!< screen representation of the current curve
	  PixColArr           m_curPatch;      //!< screen representation of last patch of a curve being edited
	  int                 m_selPointNo;    //!< index of currently selected curve's point
	  LiveSnake           m_liveSnake;     //!< object for snake calculation
	  bool                m_bContouring;   //!< TRUE if contouring of an object is under way
	  bool                m_bLine;         //!< TRUE if line is created instead of snake
	  std::list<Vec2fArr> m_undoCurveLst;  //!< undo storage of curves

	  OneImageLayout      m_layout;        //!< layout of a single image on the screen

	  alib::GaussFilter1D m_GaussFilter;   //!< Gauss's filter used for local curve smoothing

	  MyChair::ChairSet&				m_ChairSet;
	  
	  std::vector< MyChair::ChairSet::iterator >		m_Chairs;
	  int									m_nCurrentChair;
	  bool									m_bEditMode;

	  CGaugeHallDoc*						m_pDoc;
		 struct ChairCache
		{
			 ChairCache() : icurve(), animation(0) {}
			Vec2iArr icurve;      //!< the same curve with integer (rounded) coordinates
			int      animation;
		};	

		 class IdGenerator
		 {
			LONG m_lCounter;
		 public:
			 IdGenerator() : m_lCounter(-1L){}
			 DWORD	operator() (){ return InterlockedIncrement(&m_lCounter); }
			 void	SetCounter(LONG NewValue) { InterlockedExchange(&m_lCounter, NewValue); }
			 LONG	GetCounter() const { return m_lCounter;}
		 };

		 IdGenerator&	m_IdGenerator;
		 int			m_nZoneNumber;

	  std::vector<ChairCache>				m_ChairCache;
	  
	  enum
	  {
		  INVALID_CHAIR = -1
	  };

	  ImageData(	CGaugeHallDoc*		pDoc,
					//Arr2f* hi, 
					MyChair::ChairSet& cs,
					int nCameraNo,
					IdGenerator& ig);

	  bool PartialClear();
	  void CalcGradOfEnergy( const Vec2fImg & dirs, const Arr2f & grads );
	  void LocallySmoothCurve();
	  void CopyCurveToPixelCurve( bool bClosed, bool bResample = false );
	  int  KeepPointDensity( Vec2fArr & curve );
	  void CalcSnake( Vec2f p1, Vec2f p2 );
	  void DrawPixColArr( const PixColArr & src, CDC * pDC, bool bSameColor = false, COLORREF color = 0 );
	  void InitPixColArr( const Vec2fArr & src, CDC * pDC, PixColArr & dst );
	  void EndContouring(CView* pView);
	  void OnLButtonDownHandler(CView* pView,  UINT nFlags, CPoint point );
	  void OnLButtonUpHandler( CView* pView, UINT nFlags, CPoint point );
	  void OnCancelMode(CView* pView);
	  void OnMouseMoveHandler( CView* pView, UINT nFlags, CPoint point );
	  void OnKeyDownHandler( CView* pView, UINT nChar, UINT nRepCnt, UINT nFlags );
	  void OnKeyUpHandler( CView* pView, UINT nChar, UINT nRepCnt, UINT nFlags );
	  void OnEditUndoHandler(CView* pView);
	  void RecalcLayout( CView* pView, bool bUpdate );
	  bool DeleteContents();

	  void OnEditUndo(CView *pView);
	  void OnUpdateEditUndo( CCmdUI * pCmdUI ) ;

	  void LoadFloatImage();
		
		void	DeleteCurrentChair();
		bool HasSelectObject() const { return m_nCurrentChair != INVALID_CHAIR;};
		bool HasSelectedChair() const { return HasSelectObject();};
		bool IsSelectedObjectEmpty() const 
		{ 
			return HasSelectObject() && GetChair(m_nCurrentChair).curve.empty();
		}

		MyChair::Chair&			GetChair(int nIndex)		{ return *m_Chairs[nIndex]; }
		const 	MyChair::Chair	GetChair(int nIndex) const	{ return *m_Chairs[nIndex]; }

		ImageData(const ImageData& id);
		ImageData& operator = ( const ImageData& id);

		MyChair::Chair*		 GetCurrentChair()		{ MyChair::Chair& z = GetChair(m_nCurrentChair); return &z; }
		const MyChair::Chair* GetCurrentChair() const{ const MyChair::Chair& z = GetChair(m_nCurrentChair); return &z; }
		
		ChairCache*		 GetCurrentChairCache()		{ return &m_ChairCache[m_nCurrentChair]; }
		const ChairCache* GetCurrentChairCache() const{ return &m_ChairCache[m_nCurrentChair]; }

		MyChair::ChairSet::iterator InsertChair()
		{
			MyChair::ChairSet::iterator itr = m_ChairSet.insert( m_ChairSet.end(), MyChair::Chair() );
			itr->subFrameNo = /*m_pDoc->IndexToId*/( m_nZoneNumber );
			itr->id		= m_IdGenerator();
			ZeroMemory( itr->pBackChair, sizeof(itr->pBackChair) );
			ZeroMemory( itr->pFrontChair, sizeof(itr->pBackChair) );
			itr->pLeftChair		= NULL;
			itr->pRightChair	= NULL;
			return itr;
		}

		void InsertCache( MyChair::ChairSet::iterator itr )
		{
			m_Chairs.push_back( itr );
			m_ChairCache.push_back( ChairCache() );
			ChairCache& cc = m_ChairCache.back();
			//NOTENOTE: BUGBUG cc.icurve.reserve( itr->curve.size() );
			cc.icurve.resize( itr->curve.size() );
			PixCol pc;
			for (int i = 0, n = (int)(itr->curve.size()); i < n; i++)
			{
				m_layout.p2s( itr->curve[i], static_cast<CPoint&>(pc) );
				cc.icurve[i].x = pc.x;
				cc.icurve[i].y = pc.y;
			}
			//NOTENOTE: only for image cache
		//	ASSERT( vislib::IsValidCurve( cc.icurve.begin(), cc.icurve.end() ) );
		}

		bool PreLoad()
		{
			PartialClear();
			m_Chairs.clear();
			m_ChairCache.clear();
			m_nCurrentChair = INVALID_CHAIR;
			return true;
		}

		void DrawView(CView* pView, CDC& dc);
		void DrawObject(CDC& dc);
		void InitMode( CView* pView );

		bool OnDeleteChair( MyChair::ChairSet::iterator			itr );
		bool OnDeleteLink( ChairLinkList_t::iterator	itr );

	};

	typedef std::map<int, boost::shared_ptr<ImageData> >	ImageMap_t;


	class AllImageData
	{
		ImageMap_t&	m_ImageData;
		MyChair::ChairSet&				m_ChairSet;
		ChairLinkList_t&		m_ChairLink;			

		int						m_nAnimation;
		
		MyChair::ChairSet::iterator			m_CurrentChair;
		ChairLinkList_t::iterator	m_CurrentLink;

		void UpdateLinks();

		std::pair<DWORD, DWORD>		m_ImageDimension;
		CGaugeHallDoc*				m_pDoc;
		int							m_nRoomID;
	public:
		int	m_subMode;

		struct CurrentLine
		{
			MyChair::ChairSet::iterator	m_itrStart;
			CPoint				m_ptStart, m_ptEnd;

			CCacheDC<LineCache>	m_LineDC;

			CurrentLine( MyChair::ChairSet::iterator it, CPoint pt ) : 
				m_itrStart( it ), m_ptStart( pt ), m_ptEnd(pt) {}

			void Move(CPoint pt) { m_ptEnd = pt; }

			void Draw( CWnd* pWnd )	
			{	
				m_LineDC.Draw( pWnd, LineCache(m_ptStart, m_ptEnd) ) 	;
			}
			void Invalidate(CWnd* pWnd)				
			{ 
				m_LineDC.InvalidateCache(pWnd);
			}
		};

		std::auto_ptr<CurrentLine> m_pCurrentLine;

		void SetImageDim(DWORD x, DWORD y) { m_ImageDimension.first = x; m_ImageDimension.second = y; }

		AllImageData(	CGaugeHallDoc*		pDoc,
						std::map<int, boost::shared_ptr<ImageData> >& id, 
						int nRoomID,
						MyChair::ChairSet& cs, 
						ChairLinkList_t& cl
						);

		SparceRectImageLayout m_layout;

		void DrawView(CView* pView, CDC& dc);
		void DrawObject(CView* pView,CDC& dc);

		void InitMode( CView* pView );
		
		void RecalcLayout( CView* pView, bool bUpdate );
		void OnLButtonDownHandler(CView* pView,  UINT nFlags, CPoint point );
		void OnLButtonUpHandler( CView* pView, UINT nFlags, CPoint point );
		void OnMouseMoveHandler( CView* pView, UINT nFlags, CPoint point );
		void OnKeyDownHandler( CView* pView, UINT nChar, UINT nRepCnt, UINT nFlags );
		void OnKeyUpHandler( CView* pView, UINT nChar, UINT nRepCnt, UINT nFlags );
		void OnRButtonDownHandler(CView* pView,  UINT nFlags, CPoint point );

		void StartEdit(CView* pView);
		void EndEdit(CView* pView);

		void DeleteCurrentChair();
		void DeleteCurrentLink();

		bool HasSelectedChair() const { return m_CurrentChair != m_ChairSet.end();}
		bool HasSelectedLink() const { return m_CurrentLink != m_ChairLink.end();}

		bool OnDeleteChair( MyChair::ChairSet::iterator			itr );
		bool OnDeleteLink( ChairLinkList_t::iterator	itr );

		bool	PostLoadImage( );
		int		GetRoomID() const { return m_nRoomID; }
	};

	typedef std::map<int, boost::shared_ptr<AllImageData> > AllImageMap_t;
	// for each room
	std::map<int, boost::shared_ptr<AllImageData> >			m_AllImageData;

	ImageData::IdGenerator	m_IdGenerator;
	COffsetChairDialog		m_dlgMoveImage;
	COffsetChairDialog		m_dlgMoveChair;
	CMoveImageCallback		m_cbMoveImage;
	CMoveChairCallback		m_cbMoveChair;

	CGaugeHallFrame*		GetHallFrame();
	void				InitAll();

	std::vector< ImageData*	>					GetImageDataInRoom( int nRoomID );
	std::vector< std::pair<int, ImageData*>	>	GetImageDataInRoomPair( int nRoomID );
	IChairSerializer::PositionMap_t				GetCameraPosition( int nRoomID );

	bool	IsLayoutValid( int nRoomID );
	bool	IsAllImagesLoaded( int nRoomID );
	void	PostLoadProcessGraph( int nRoomID, MyChair::ChairSet& ChSet, ChairLinkList_t& ChLink );

	void	MoveImage(int dx, int dy);
	void	MoveChair(int dx, int dy);
	void	ShiftImage( Arr2f& img, int dx, int dy );
	bool	CheckImage(CString sName);
	void	LoadImage( AllImageData& All, int x, int y );
	int		PositionToID( int x, int y, int nRoomID );
	std::pair<int,int>	IDToPosition( int nCameraID );
	CString	PostitionToName( int x, int y, size_t nAll );
	size_t		GetCamerasInRoom(int nRoomID);
	
public:
	// for each camera
	std::map< int, boost::shared_ptr<ImageData> >	m_ImageData;

	std::vector<int>								m_StandAloneCameras;
	bool			IsViewAll();
	int				GetCurrentCamera();
	int				GetCurrentRoom();

	ImageData&		GetImageData() { return *m_ImageData[GetCurrentCamera()];}
//	const ImageData&		GetImageData() const { return m_ImageData[m_nCurrentIndex];}


 // FrameAccumulator	m_FrameAcc;

  int&				GetAdjustRadius()	{ return GetImageData().m_adjustRadius; }

public:
		
	bool			IsObjectListEmpty() { return !IsViewAll() && !m_ImageData.empty() && GetImageData().m_Chairs.empty(); }
	bool			HasSelectObject() { return !IsViewAll() && !m_ImageData.empty() && GetImageData().HasSelectObject(); }
	bool			IsSelectedObjectEmpty() 
	{ 
		return !IsViewAll() && !m_ImageData.empty() && GetImageData().IsSelectedObjectEmpty() ;
	}

	void			DrawObject(CDC& dc);
	void			DrawView( CDC& dc);

	//FIXME: Allimage view mode is invalid
	uint				GetSubMode() 	{ return IsViewAll()? /*std::numeric_limits<uint>::max*/-1 : GetImageData().m_subMode;		}
  void				SetSubMode(uint sm)	{ if( !IsViewAll()) GetImageData().m_subMode = sm;		}

  bool				GetCountouting()  { return !IsViewAll() && GetImageData().m_bContouring; }

  PixColArr&		GetCurPatch()		{ return GetImageData().m_curPatch; }
  bool				IsGradientsEmpty()  { return !IsViewAll() && GetImageData().m_gradients.empty();}

  void				LoadFloatImage(DWORD dwImageNumber);

  void				DeleteObject();
  bool				HasMenu()  { return !IsViewAll();}
  void				GetCameras( int nRoomID, std::vector<int>& CameraNo );
 // 

		void DeleteChair( MyChair::ChairSet::iterator			itr );
		void DeleteLink( ChairLinkList_t::iterator	itr );			
  
		void OnRoomChange( int nRoomID );
		void OnCameraChange( int nCameraID );
		bool IsStandAlone() const { return m_bStandAlone ;}
  
 //const  Arr2ub*			m_pBackground;
protected:
  DECLARE_DYNCREATE(CGaugeHallDoc)
  CGaugeHallDoc();
  virtual ~CGaugeHallDoc();

  //{{AFX_VIRTUAL(CGaugeHallDoc)
  virtual void DeleteContents();
  virtual BOOL SaveModified();
  //}}AFX_VIRTUAL

protected:
  
  void CalcGradOfEnergy( const Vec2fImg & dirs, const Arr2f & grads );
  void LocallySmoothCurve();
  void CopyCurveToPixelCurve( bool bClosed, bool bResample = false );
  int  KeepPointDensity( Vec2fArr & curve );
  void CalcSnake( Vec2f p1, Vec2f p2 );
  void DrawPixColArr( const PixColArr & src, CDC * pDC, bool bSameColor = false, COLORREF color = 0 );
  void InitPixColArr( const Vec2fArr & src, CDC * pDC, PixColArr & dst );
  void EndContouring();
public:
	void PartialClear();
	CGaugeHallView * GetView();
  void OnLButtonDownHandler( UINT nFlags, CPoint point );
  void OnLButtonUpHandler( UINT nFlags, CPoint point );
  void OnCancelMode();
  void OnMouseMoveHandler( UINT nFlags, CPoint point );
  void OnKeyDownHandler( UINT nChar, UINT nRepCnt, UINT nFlags );
  void OnKeyUpHandler( UINT nChar, UINT nRepCnt, UINT nFlags );
  void OnEditUndoHandler();
  void RecalcLayout( bool bUpdate );

  void OnRButtonDownHandler( CView* pView, UINT nFlags, CPoint point, CMenu* pMenu );

  bool	InitBackground(const SparceRectImageLayout::IndexArray_t& arr);

	void	PostLoadImage( ImageData& id );
	void	PostLoadGraph( );
	void	InitMode();
	void	DisableDBButton( CCmdUI * pCmdUI, bool bMustViewAll );

//	void	PreLoadGraph( );
//	void	PreSaveGraph( );
#if 0
	void	RenumberZoneIndex();
	void	RenumberChairs(const std::vector<int>& Old, const std::vector<int>& New);
	int		IndexToId(int nIndex) const;
	int		IdToIndex(int nId) const;
#endif

	void OnInit();
public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

protected:
  //{{AFX_MSG(CGaugeHallDoc)
  afx_msg void OnFileOpen();
  afx_msg void OnFileSave();
  afx_msg void OnFileSaveAs();
  afx_msg void OnEditUndo();
  afx_msg void OnUpdateEditUndo( CCmdUI * pCmdUI );
#if 0
  afx_msg void OnImage( UINT nID );
  afx_msg void OnImageUpdate( CCmdUI* pCmdUI);
  afx_msg void OnImageAll(  );
  afx_msg void OnImageAllUpdate( CCmdUI* pCmdUI);
#endif

  afx_msg void OnFileSaveGraph();
  afx_msg void OnFileLoadGraph();

  afx_msg void OnFileLoadImage();
  afx_msg void OnFileSaveImage();

  afx_msg void OnDeleteItem();
  afx_msg void OnDeleteItemUpdate(CCmdUI* pCmdUI);

  afx_msg void OnDBImageLoad();
  afx_msg void OnDBImageLoadUpdate(CCmdUI* pCmdUI);

  afx_msg void OnDBImageSave();
  afx_msg void OnDBImageSaveUpdate(CCmdUI* pCmdUI);

  afx_msg void OnDBGraphLoad();
  afx_msg void OnDBGraphLoadUpdate(CCmdUI* pCmdUI);

  afx_msg void OnDBGraphSave();
  afx_msg void OnDBGraphSaveUpdate(CCmdUI* pCmdUI);

  afx_msg void OnImageGrab();
  afx_msg void OnNewGraph();	

	afx_msg void OnParamShow();
	afx_msg void OnParamShowUpdate(CCmdUI* pCmdUI);

  afx_msg void OnClone();

  afx_msg void OnGrabCamera();
   afx_msg void OnGrabCameraUpdate( CCmdUI* pCmdUI );

  afx_msg void OnGrabFile();

	afx_msg void OnLayout();
	afx_msg void OnLayoutUpdate (CCmdUI* pCmdUI);

	afx_msg void OnFileSaveGraphUpdate (CCmdUI* pCmdUI);
	afx_msg void OnFileLoadGraphUpdate (CCmdUI* pCmdUI);
	afx_msg void OnFileLoadImageUpdate (CCmdUI* pCmdUI);
	afx_msg void OnFileSaveImageUpdate (CCmdUI* pCmdUI);

	afx_msg void OnDBSaveModel();
	afx_msg void OnDBSaveModelUpdate(CCmdUI* pCmdUI);

	afx_msg void OnMoveImage(  );
	afx_msg void OnMoveChair(  );
	afx_msg void OnMoveImageUpdate( CCmdUI* pCmdUI );
	afx_msg void OnMoveChairUpdate( CCmdUI* pCmdUI );
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
