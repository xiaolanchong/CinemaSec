#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "res/resource.h"
#include "train_data_doc.h"
#include "train_data_view.h"
#include "train_data_frm.h"

#include "../res/resource.h"
#include "../Chair/chairserializer.h"
#include "../FileDialogEx.h"
#ifdef USE_DATAPREPARE
#include "../../DataPrepare/StartDataPrepareFunction.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTrainDataDoc, CDocument)

BEGIN_MESSAGE_MAP(CTrainDataDoc, CDocument)
  ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
  ON_COMMAND(ID_FILE_OPEN_GRAPH,		OnFileOpenGraph)
//-----------------------------------------------------------------------------
// Purpose: 
// Input  : ID_FILE_SAVE_GRAPH - 
//					OnFileSaveGraph - 
//-----------------------------------------------------------------------------
  ON_COMMAND(ID_FILE_SAVE_GRAPH,		OnFileSaveGraph)
  ON_COMMAND(ID_FILE_OPEN_TRAIN_DATA,	OnFileOpenTrainData)
  ON_COMMAND(ID_FILE_SAVE_TRAIN_DATA,	OnFileSaveTrainData)
  ON_COMMAND( ID_BUTTON_START_RANGE,	OnRangeStart)
  ON_COMMAND( ID_BUTTON_ADD_RANGE,		OnRangeEnd)
  ON_COMMAND( ID_BUTTON_CANCEL_RANGE,	OnRangeCancel)
  ON_UPDATE_COMMAND_UI( ID_BUTTON_START_RANGE,	OnRangeStartUpdate)
  ON_UPDATE_COMMAND_UI( ID_BUTTON_ADD_RANGE,	OnRangeEndUpdate)
  ON_UPDATE_COMMAND_UI( ID_BUTTON_CANCEL_RANGE,	OnRangeCancelUpdate)

  ON_COMMAND( ID_RANGE_HUMAN,	OnRangeButtonHuman)
  ON_COMMAND( ID_RANGE_EMPTY,	OnRangeButtonEmpty)
  ON_COMMAND( ID_RANGE_CLOTHES,	OnRangeButtonClothes)
  ON_COMMAND( ID_RANGE_UNKNOWN,	OnRangeButtonUnknown)

  ON_COMMAND( ID_MOVE_CHAIRS,	OnMoveChairs)

  ON_UPDATE_COMMAND_UI( ID_RANGE_HUMAN,	OnRangeButtonHumanUpdate)
  ON_UPDATE_COMMAND_UI( ID_RANGE_EMPTY,	OnRangeButtonEmptyUpdate)
  ON_UPDATE_COMMAND_UI( ID_RANGE_CLOTHES,	OnRangeButtonClothesUpdate)
  ON_UPDATE_COMMAND_UI( ID_RANGE_UNKNOWN,	OnRangeButtonUnknownUpdate)
  
END_MESSAGE_MAP()

CTrainDataDoc::CTrainDataDoc() : 
	m_pFrameWin(0),
	m_nCurrentCameraNo(0)
{
	m_itCurrentChair = m_ChairSet.end();
	m_dlgMove.SetCallback(this);
	HRESULT hr = ::CreateInterface( CHAIR_CONTENT_INTERFACE_EX, (void**)& m_pChairInt );
	ASSERT(hr == S_OK);
}


CTrainDataDoc::~CTrainDataDoc()
{
	if( m_pChairInt ) m_pChairInt->Release();
}

static LPCTSTR szWC_XML = _T("*.xml");

//-------------------------------------------------------------------------------------------------
// CTrainDataDoc diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CTrainDataDoc::AssertValid() const
{
  CDocument::AssertValid();
}
void CTrainDataDoc::Dump( CDumpContext & dc ) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG

//-------------------------------------------------------------------------------------------------
/** \brief Function initializes pointer to the base video view of this document. */
//-------------------------------------------------------------------------------------------------
CTrainDataView * CTrainDataDoc::GetTrainDataView()
{
  for (POSITION pos = GetFirstViewPosition(); pos != 0;)
  {
    CWnd * p = GetNextView( pos );
    if (p->IsKindOf( RUNTIME_CLASS( CTrainDataView ) ))
      return (reinterpret_cast<CTrainDataView*>( p ));
  }
  return 0;
}


void CTrainDataDoc::DeleteContents()
{
	m_layout.clear();
  m_avi.Close();
  m_itCurrentChair = m_ChairSet.end();
  m_tempBackgr.clear();
  m_tempBuffer.clear();
  CDocument::DeleteContents();
}


void CTrainDataDoc::OnFileOpen()
{
	CString sAviFilter;
	sAviFilter.LoadString( IDS_FILTER_AVI_AVD );
  CFileDialog dlg( TRUE, _T("*.avi"), 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
                   OFN_PATHMUSTEXIST, sAviFilter, ::AfxGetMainWnd() );
  if (dlg.DoModal() != IDOK)
    return;

  DeleteContents();
  
  if (m_avi.OpenRead( (LPCTSTR)(dlg.GetPathName()) ))
  {
    SetPathName( (LPCTSTR)(dlg.GetPathName()) );
    SetTitle( (LPCTSTR)(dlg.GetFileName()) );
    SetModifiedFlag( FALSE );
	m_strVideoFilePath = dlg.GetPathName();
	m_pFrameWin->SetCurrentFrame( 0 );
	//NOTENOTE: for test
#ifdef CHECK_BG
	Arr2ub ImageArray; bool res;

	GetBackGround( 0, ImageArray );
	res = SaveImage( _T("dmp_Frame_0.bmp"), ImageArray, true, false); ASSERT(res);
	GetBackGround( 200, ImageArray );
	res = SaveImage( _T("dmp_Frame_200.bmp"), ImageArray, true, false); ASSERT(res);
#endif
  }
  else 
  {
	  DeleteContents();
	  AfxMessageBox( IDS_ERROR_FILE_LOADING, MB_OK|MB_ICONERROR );
  }
  UpdateAllViews(0);
}


void CTrainDataDoc::OnFileOpenGraph()
{
	CString sChairFilter;
	sChairFilter.LoadString( IDS_FILTER_CHAIR );
	CFileDialogEx dlg(	TRUE, szWC_XML, 0, 0, sChairFilter, 0 );
	if( dlg.DoModal() == IDOK)
	{
		try
		{
			ChairLoader cl;
			IChairSerializer::PositionMap_t PosMap;
			cl.Load( dlg.GetPathName(), m_ChairSet, m_ChairLink, PosMap );
	//		cl.Load( dlg.GetPathName(), MyChair::ChairSet(), ChairLinks );
			m_nCurrentCameraNo = GetFirstZoneNumber();
			ConvertToTrainList();
			
			m_itCurrentChair = m_ChairSet.end();
			RecalcLayout(true);
			m_strGaugeFilePath = dlg.GetPathName();
			
		}
		catch(ChairSerializerException)
		{
			//FIXME: to resources
			AfxMessageBox( IDS_ERROR_FILE_LOADING, MB_OK|MB_ICONERROR);
		}
	}
}

void CTrainDataDoc::OnFileSaveGraph()
{
	CString sChairFilter;
	sChairFilter.LoadString( IDS_FILTER_CHAIR );
	CFileDialogEx dlg(	FALSE, szWC_XML, 0, 
						OFN_READONLY|OFN_OVERWRITEPROMPT, 
						sChairFilter, 0 );
	if( dlg.DoModal() == IDOK)
	{
		try
		{
			ChairSaver cs;
			IChairSerializer::PositionMap_t PosMap;
			cs.Save( dlg.GetPathName(), m_ChairSet, m_ChairLink, PosMap );
		}
		catch(ChairSerializerException)
		{
			//FIXME: 
			AfxMessageBox(IDS_ERROR_FILE_SAVING, MB_OK|MB_ICONERROR);
		}
	}
}

void DumpChairSet( const MyChair::ChairSet& cs)
{
	MyChair::ChairSet::const_iterator crvIt;
	for (crvIt = cs.begin(); crvIt != cs.end(); ++crvIt)
	{
		TRACE1( "Id=%d", crvIt->id );
		switch( crvIt->contents )
		{
		case /*ChairContents::*/CHAIR_CLOTHES : TRACE(_T(" C ")); break;
		case /*ChairContents::*/CHAIR_HUMAN :	TRACE(_T(" H ")); break;
		case /*ChairContents::*/CHAIR_UNKNOWN : TRACE(_T(" U ")); break;
		case /*ChairContents::*/CHAIR_EMPTY :	TRACE(_T(" E ")); break;
		case /*ChairContents::*/CHAIR_UNDEFINED :	TRACE(_T(" Un ")); break;
		default : ASSERT(FALSE);
		}
		TRACE(_T("\n"));
	}
}

void CTrainDataDoc::OnFileOpenTrainData()
{
	CString sTrainFilter;
	sTrainFilter.LoadString( IDS_FILTER_TRAIN );
	CFileDialogEx dlg(	TRUE, szWC_XML, 0, 0, sTrainFilter, 0 );
	if( dlg.DoModal() == IDOK)
	{
		try
		{
			ContentLoader cl;
			CString strVideo, strGauge;
			
			ConvertToTrainList();
			m_itCurrentChair = m_ChairSet.end(); 
			cl.Load( dlg.GetPathName(), m_ChairSet, m_ChairTrain, strVideo, strGauge );
			UpdateAllViews(0);
#if 0
//			MyChair::ChairSet csSet;
//			GetChairs( 1, csSet );
//			DumpChairSet(csSet);

			std::vector< BaseChair> BaseChairs;
			size_t nSize = m_ChairSet.size();
			BaseChairs.resize( nSize );
			MyChair::ChairSet::const_iterator it = m_ChairSet.begin();
			for( size_t i = 0; it != m_ChairSet.end(); ++it, ++i)
			{
				BaseChairs[i].cameraNo	= it->subFrameNo;
				BaseChairs[i].curve		= it->curve;
				BaseChairs[i].index		= it->id;
			}

			IChairContent* pInt;
			DWORD dwwww = CreateInterface( CHAIR_CONTENT_INTERFACE, (void**)&pInt );
			
			pInt->Load( dlg.GetPathName(), BaseChairs );
			std::map<int, ChairContents > zMap;
			pInt->GetContent( 30, zMap );
			pInt->Release();
			int k = zMap.size();
#endif
		}
		catch(ContentSerializerException)
		{
			//FIXME: to resources
			// handle this bug
			//__asm int 3;
			ConvertToTrainList();
			AfxMessageBox( IDS_ERROR_FILE_LOADING, MB_OK|MB_ICONERROR);
		}
	}
}



void CTrainDataDoc::OnFileSaveTrainData()
{

	CString sTrainFilter;
	sTrainFilter.LoadString( IDS_FILTER_TRAIN );
  CFileDialog dlg( FALSE, szWC_XML , 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
                   OFN_PATHMUSTEXIST, sTrainFilter, ::AfxGetMainWnd() );
  if (dlg.DoModal() != IDOK)
    return;

  ContentSaver cs;
  try
  {
	cs.Save( dlg.GetPathName(), m_ChairSet, m_ChairTrain, m_strVideoFilePath, m_strGaugeFilePath);
  }
  catch( ContentSerializerException )
  {
	  AfxMessageBox( IDS_ERROR_FILE_SAVING, MB_OK|MB_ICONERROR);
  }
  UpdateAllViews(0);
}

void CTrainDataDoc::RecalcLayout(bool bUpdate)
{
	UNREFERENCED_PARAMETER(bUpdate);
	UpdateAllViews(0);
	
}


void CTrainDataDoc::OnRangeStart()
{
	m_pFrameWin->m_wndPrepareDataBar.StartEdit();
	UpdateAllViews(0);
}

void CTrainDataDoc::OnRangeCancel()
{
	m_pFrameWin->m_wndPrepareDataBar.CancelEdit();
	UpdateAllViews(0);
}

void CTrainDataDoc::OnRangeEnd()
{
	m_pFrameWin->m_wndPrepareDataBar.EndEdit();
	UpdateAllViews(0);
}
 
void CTrainDataDoc::OnRangeStartUpdate(CCmdUI* pCmdUI)
{
	bool bEditMode = m_pFrameWin->m_wndPrepareDataBar.m_frameScroll.IsEditMode();
	pCmdUI->Enable( !bEditMode && m_avi.IsOk() && m_itCurrentChair != m_ChairSet.end()
					? TRUE : FALSE );
}

void CTrainDataDoc::OnRangeEndUpdate(CCmdUI* pCmdUI)
{
	bool bEditMode = m_pFrameWin->m_wndPrepareDataBar.m_frameScroll.IsEditMode();
	pCmdUI->Enable( bEditMode && m_avi.IsOk() && m_itCurrentChair != m_ChairSet.end()
					? TRUE : FALSE );
}

void CTrainDataDoc::OnRangeCancelUpdate(CCmdUI* pCmdUI)
{
	bool bEditMode = m_pFrameWin->m_wndPrepareDataBar.m_frameScroll.IsEditMode();
	pCmdUI->Enable( bEditMode && m_avi.IsOk() && m_itCurrentChair != m_ChairSet.end()
					? TRUE : FALSE );
}

void CTrainDataDoc::OnLButtonDownHandler( UINT nFlags, CPoint point )
{
	UNREFERENCED_PARAMETER(nFlags);
    for (MyChair::ChairSet::iterator it = m_ChairSet.begin(); it != m_ChairSet.end(); ++it)
    {
		const Vec2fArr& Curve = it->curve;
		CPoint NewPoint;
		m_layout.s2p( /*it->subFrameNo,*/ point, NewPoint );
	//	ptPos.Offset( sizeOffset );

      if (vislib::CurveAndPointRelation( Curve.begin(), Curve.end(), NewPoint.x, NewPoint.y ) > 0)
      {
		m_itCurrentChair = it;
		int nID = it->id;
		ChairTrainList_t::iterator itTrain = m_ChairTrain.find( nID );
		ASSERT( itTrain != m_ChairTrain.end() );
		UpdateAllViews(0);
		m_pFrameWin->m_wndPrepareDataBar.m_frameScroll.SetContent( &itTrain->second );
		m_pFrameWin->m_wndPrepareDataBar.m_frameScroll.Invalidate();
		//m_pFrameWin->m_wndVideoToolBar.Invalidate();
	//	m_pFrameWin->m_wndVideoToolBar.m_frameScroll.Invalidate();
		
		break;
      }
    }	
}

void	CTrainDataDoc::ConvertToTrainList()
{
	m_ChairTrain.clear();
    for (MyChair::ChairSet::const_iterator it = m_ChairSet.begin(); it != m_ChairSet.end(); ++it)
    {
		m_ChairTrain.insert( std::make_pair(  it->id, RangeList_t() ) );
	}
}

Range::CONTENT CTrainDataDoc::GetCurrentContent( int nID) const
{
/*	ChairTrainList_t::const_iterator itTrain = m_ChairTrain.find( nID );
	ASSERT( itTrain != m_ChairTrain.end() );*/	
	int nPos = m_pFrameWin->m_wndPrepareDataBar.m_frameScroll.GetPos();
/*	Range::CONTENT cnt = GetState( itTrain->second, nPos);*/
	return GetContentInFrame( nID, nPos );
}

Range::CONTENT CTrainDataDoc::GetContentInFrame( int nID, int nPos) const
{
	ChairTrainList_t::const_iterator itTrain = m_ChairTrain.find( nID );
	ASSERT( itTrain != m_ChairTrain.end() );	
	Range::CONTENT cnt = m_pChairInt->GetState( itTrain->second, nPos);
	return cnt;
}

int CTrainDataDoc::GetTotalFrameNum() 
{
	return m_avi.GetFrameNumber();
}

int CTrainDataDoc::GetBackGround(int nFrameNum, Arr2ub& bg) 
{
#ifdef WIN32
struct RgbQuad : public RGBQUAD
{
#else
struct RgbQuad
{
  unsigned char rgbBlue; 
  unsigned char rgbGreen; 
  unsigned char rgbRed; 
  unsigned char rgbReserved;
#endif // WIN32

  RgbQuad()
  {
    assert( sizeof(unsigned char) == 1 );
  }
};


	bool bInvertY = true;
	if( !m_avi.IsOk() || nFrameNum >= m_avi.GetFrameNumber() ) return -1;

 // BitmapInfoHeader            bi;
	BITMAPINFOHEADER bi;
  std::vector<unsigned char>  scan;
  bool                        ok = true;

	bool res = m_avi.GetFrame( nFrameNum );
	if(!res) return false;

  const BITMAPINFOHEADER*	pDib	= m_avi.GetDIB();
  const BYTE*				pData	= m_avi.GetData();
  try
  { 
    CopyMemoryWithOffset( &bi,	(const BYTE*&)pDib,sizeof(bi) ) ;

    if (!( (bi.biCompression == BI_RGB) && ((bi.biBitCount >= 24) || (bi.biBitCount == 8))))
      ALIB_THROW( _T("Expects 32-,24-,8-bits uncompressed bitmap") );
    
    if (bi.biBitCount >= 24)                     // true-color bitmap
    {
      int nByte = (bi.biBitCount == 32) ? 4 : 3; // bytes per pixel

      scan.resize( ((nByte*bi.biWidth + 3) & ~3), 0 );
      bg.resize( bi.biWidth, bi.biHeight );

      // Read file scan by scan.
      for(int y = 0; y < bi.biHeight; y++)
      {
        unsigned char * pDst = bg.row_begin( bInvertY ? (bi.biHeight-1-y) : y );
        unsigned char * pSrc = &(*(scan.begin()));

        CopyMemoryWithOffset( &(*(scan.begin())), pData, (int)(scan.size()*sizeof(unsigned char))) ;

        for(int k = 0; k < bi.biWidth; k++)
        {
          unsigned int b = pSrc[0];
          unsigned int g = pSrc[1];
          unsigned int r = pSrc[2];
          (*pDst) = (unsigned char)((r*306 + g*601 + b*117 + 512) >> 10);
          pDst++;
          pSrc += nByte;
        }
      }
    }
    else if (bi.biBitCount == 8)                 // 8-bits bitmap
    {
      RgbQuad palette[256];

      scan.resize( ((bi.biWidth + 3) & ~3), 0 );
      bg.resize( bi.biWidth, bi.biHeight );
      memset( palette, 0, sizeof(palette) );
            
      CopyMemoryWithOffset( palette, (const BYTE*&)pDib, sizeof(palette) )  ;

      // Read file scan by scan.
      for (int y = 0; y < bi.biHeight; y++)
      {
        unsigned char * pDst = bg.row_begin( bInvertY ? (bi.biHeight-1-y) : y );
        unsigned char * pSrc = &(*(scan.begin()));

        CopyMemoryWithOffset( &(*(scan.begin())), pData, (int)(scan.size()*sizeof(unsigned char)) ) ;

        for (int k = 0; k < bi.biWidth; k++)
        {
          RgbQuad &    color = palette[ (unsigned int)(*pSrc) ];
          unsigned int b = color.rgbBlue;
          unsigned int g = color.rgbGreen;
          unsigned int r = color.rgbRed;

          (*pDst) = (unsigned char)((r*306 + g*601 + b*117 + 512) >> 10);
          pDst++;
          pSrc++;
        }
      }
    }

    if (csutility::CorrectByteBrightness( &bg, 70, &m_tempBackgr, &m_tempBuffer ))
      bg.swap( m_tempBackgr );
  }
  catch (std::exception & /*e*/)
  {
    bg.clear();
 //   alib::ErrorMessage( _T("Failed to load image"), _T("reason:"), (LPCTSTR)CString( e.what() ), fname );
    ok = false;
  }
  catch (...)
  {
    bg.clear();
 //   alib::ErrorMessage( _T("Failed to load image"), ALIB_UNSUPPORTED_EXCEPTION, fname );
    ok = false;
  }


  return ok ? 0 : -1;
}

int CTrainDataDoc::GetChairs    (int nFrameNum, MyChair::ChairSet& chairs) 
{
	chairs.clear();
	if( !m_avi.IsOk() || nFrameNum >= m_avi.GetFrameNumber() ) return -1;
	MyChair::ChairSet::const_iterator crvIt;
	 for (crvIt = m_ChairSet.begin(); crvIt != m_ChairSet.end(); ++crvIt)
	 {
		 Range::CONTENT cnt = GetContentInFrame( crvIt->id , nFrameNum);
	//	 if( Range::undefined != cnt )
		 {
			 ChairContents cntChair;
			 switch( cnt )
			 {
				case Range::clothes :		cntChair = CHAIR_CLOTHES; break;
				case Range::human :			cntChair = CHAIR_HUMAN;break;
				case Range::unknown :		cntChair = CHAIR_UNKNOWN;break;
				case Range::empty :			cntChair = CHAIR_EMPTY;break;
				case Range::undefined:		cntChair = CHAIR_UNDEFINED; break;
					// dont know
				default:
					continue;
			 }
			 chairs.push_back( MyChair::Chair() );
			MyChair::Chair& ch = chairs.back();
			ch.center		= crvIt->center;              
            ch.subFrameNo	= crvIt->subFrameNo;   
            ch.id			= crvIt->id;           
			ch.contents		= cntChair;         
			ch.curve		= crvIt->curve;     
/*			ch.points;          //!< external and internal points of a chair
  MyChair::Chair *        ch.pLeftChair;      //!< pointer to the left neighbour chair or 0
  MyChair::Chair *        pRightChair;     //!< pointer to the right neighbour chair or 0
  MyChair::Chair *        pBackChair[3];   //!< pointers to the back neighbour chairs or 0s
  MyChair::Chair *        pFrontChair[3];  //!< pointers to the front neighbour chairs or 0s
 */        
		 }
	 }

	return 0;
}

void CTrainDataDoc::OnRangeButtonHuman()
{
	ChangeContentRange( Range::human );
}

void CTrainDataDoc::OnRangeButtonEmpty()
{
	ChangeContentRange( Range::empty );
}

void CTrainDataDoc::OnRangeButtonClothes()
{
	ChangeContentRange( Range::clothes );
}

void CTrainDataDoc::OnRangeButtonUnknown()
{
	ChangeContentRange( Range::unknown );
}

void CTrainDataDoc::OnRangeButtonHumanUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( CheckCurrentRange( Range::human ) );	
}

void CTrainDataDoc::OnRangeButtonEmptyUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( CheckCurrentRange( Range::empty ) );	
}

void CTrainDataDoc::OnRangeButtonClothesUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( CheckCurrentRange( Range::clothes ) );	
}

void CTrainDataDoc::OnRangeButtonUnknownUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( CheckCurrentRange( Range::unknown ) );
}

void CTrainDataDoc::ChangeContentRange( Range::CONTENT Want )
{
	Range::CONTENT Was = m_pFrameWin->m_wndPrepareDataBar.m_frameScroll.GetCurrentRangeContent( );
	if( Was == Want )
	{
		m_pFrameWin->m_wndPrepareDataBar.m_frameScroll.SetCurrentRangeContent( Range::undefined );
	}
	else
	{
		m_pFrameWin->m_wndPrepareDataBar.m_frameScroll.SetCurrentRangeContent( Want );	
	}
//	m_pFrameWin->Invalidate();
//	m_pFrameWin->m_wndVideoToolBar.Invalidate();
	//FIXME: //FIXME: //FIXME: //FIXME: //FIXME: //FIXME: //FIXME: 
	m_pFrameWin->m_wndPrepareDataBar.m_frameScroll.Repaint();

}

bool	CTrainDataDoc::CheckCurrentRange( Range::CONTENT Want )
{
	return m_pFrameWin->m_wndPrepareDataBar.m_frameScroll.GetCurrentRangeContent( ) == Want;
}

void CTrainDataDoc::OnMoveChairs()
{
	m_dlgMove.ShowWindow(SW_SHOW);
}

void CTrainDataDoc::Move(int x, int y)
{
	MyChair::ChairSet::iterator crvIt;
	for (crvIt = m_ChairSet.begin(); crvIt != m_ChairSet.end(); ++crvIt)
	{
		for(DWORD i=0; i < crvIt->curve.size(); ++i)
		{
			crvIt->curve[i] += Vec2f( float( x) , float(y) );
		}
	}
	UpdateAllViews(0);
}

int	CTrainDataDoc::GetFirstZoneNumber() const
{
	std::set<int> ZoneNumber;
	MyChair::ChairSet::const_iterator crvIt = m_ChairSet.begin();
	for (; crvIt != m_ChairSet.end(); ++crvIt)
	{
		ZoneNumber.insert( crvIt->subFrameNo );
	}
	if( ZoneNumber.empty() ) return 0;
	else return *ZoneNumber.begin();
}