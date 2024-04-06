// PropertyListBox.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyListBox.h"
//#include "Helper.h"
#include "CJFlatComboBox.h"
#ifdef USE_COLORPICKER
#include "../../ut/OXColorPickerCtrl.h"
#endif
/*#include "ut/oxedit.h"*/
#include "NumEdit.h"

using namespace Gdiplus;

#include <limits>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyListBox

CPropertyListBox::CPropertyListBox():m_bDragMode(false)
{
}

CPropertyListBox::~CPropertyListBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyListBox, CListBox)
	//{{AFX_MSG_MAP(CPropertyListBox)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	ON_WM_CANCELMODE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP

	ON_CONTROL_RANGE(EN_KILLFOCUS, 1, 99, OnEditKillFocus)
	ON_CONTROL_RANGE(EN_CHANGE, 1, 99, OnEditChange)
	ON_CONTROL_RANGE(CBN_KILLFOCUS, 1, 99, OnEditKillFocus)
	ON_CONTROL_RANGE(CBN_SELENDOK , 1, 99, OnComboBoxChange)
	ON_CONTROL_RANGE(CBN_EDITUPDATE, 1, 99, OnComboBoxEditChange)
	ON_WM_ENTERMENULOOP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyListBox message handlers

void CPropertyListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	lpMeasureItemStruct->itemHeight = 22;
}

void CPropertyListBox::DrawEdge(Graphics& gr, const CRect& rcItem)
{
		Color cl;
		cl.SetFromCOLORREF( GetSysColor(COLOR_3DSHADOW));
		Pen pen( cl, 1);
		
		cl.SetFromCOLORREF( GetSysColor(COLOR_3DFACE) );
		SolidBrush br(cl);

		gr.FillRectangle( &br, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height() );
		gr.DrawLine( &pen, rcItem.left, rcItem.top, rcItem.right, rcItem.top );

		gr.DrawRectangle( &pen, rcItem.left, rcItem.top, rcItem.Width()-1, rcItem.Height() );

		cl.SetFromCOLORREF( GetSysColor(COLOR_3DSHADOW) );
		int pt = m_nDelemiter;;
		gr.DrawLine( &Pen(cl, 1),  pt-1, rcItem.bottom, pt-1, rcItem.top );

}

void CPropertyListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);
	CRect rcItem(lpDrawItemStruct->rcItem);

	int nIndex = lpDrawItemStruct->itemID;
	Graphics gr(lpDrawItemStruct->hDC);

	if( nIndex == LB_ERR) return;

	DrawEdge(gr, rcItem);

	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
      (lpDrawItemStruct->itemState & ODS_SELECTED))
	{
/*		Color cl;
		cl.SetFromCOLORREF( GetSysColor(COLOR_ACTIVEBORDER) );
		SolidBrush br(cl);

		gr.FillRectangle( &br, rcItem.left, rcItem.top-1, rcItem.Width(), rcItem.Height()-1 );
//		DrawEdge(gr, rcItem);*/


	}
   
   else
	{
/*		Color cl;
		cl.SetFromCOLORREF( GetSysColor(COLOR_3DSHADOW));
		Pen pen( cl, 1);
		
		cl.SetFromCOLORREF( GetSysColor(COLOR_ACTIVEBORDER) );
		SolidBrush br(cl);

		gr.FillRectangle( &br, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height() );
		gr.DrawLine( &pen, rcItem.left, rcItem.top, rcItem.right, rcItem.top );

		gr.DrawRectangle( &pen, rcItem.left, rcItem.top, rcItem.Width()-1, rcItem.Height() );

		cl.SetFromCOLORREF( GetSysColor(COLOR_3DSHADOW) );
		int pt = m_nDelemiter;;
		gr.DrawLine( &Pen(cl, 1),  pt-2, rcItem.bottom, pt-2, rcItem.top );*/

	}

   if ((lpDrawItemStruct->itemAction | ODA_FOCUS) &&
      (lpDrawItemStruct->itemState & ODS_FOCUS))
   {

   }
	
   

	CString PropName = m_Windows[nIndex].m_Name;
	CString Value;
	m_Windows[nIndex].m_Control->GetWindowText(Value);
	CRect rcName(rcItem);
	CRect rcValue(rcItem);
	rcName.right = rcItem.left + m_nDelemiter; 
	rcValue.left = rcName.right;

	FontFamily  fontFamily(L"Arial");
	Font        font(&fontFamily, 12, FontStyleRegular, UnitPixel);
	   
	StringFormat stringFormat;
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter   );
	stringFormat.SetFormatFlags( StringFormatFlagsNoWrap );

	USES_CONVERSION;
	LPCWSTR szDrawString = T2CW( PropName );
	if(!rcName.IsRectEmpty())
	gr.DrawString( szDrawString, -1, &font, 
					RectF(rcName.left, rcName.top, rcName.Width(), rcName.Height()), 
					&stringFormat, 
					&SolidBrush( Color( 0, 0, 0)));

	CWnd* pWnd = m_Windows[nIndex].m_Control.get();
#ifdef USE_COLORPICKER
	COXColorPickerCtrl * pButton = dynamic_cast<COXColorPickerCtrl *>(pWnd);
	if(pButton)
	{
		Color cl;
		cl.SetFromCOLORREF( pButton->GetSelectedColor());
		SolidBrush br(cl);
		CRect rcColor(rcValue);
		rcColor.DeflateRect(5, 3, 5, 3);
		gr.FillRectangle( &br, rcColor.left, rcColor.top, rcColor.Height(), rcColor.Height() );
		Value.Format( _T("[%d %d %d]"), cl.GetR(), cl.GetG(), cl.GetB() );
		rcValue .left += rcValue.Height();
		rcValue.NormalizeRect();
	}
#endif
//	else
	szDrawString = T2CW( Value );
	gr.DrawString( szDrawString, -1, &font, 
					RectF(rcValue.left, rcValue.top, rcValue.Width(), rcValue.Height()), 
					&stringFormat, 
					&SolidBrush( Color( 0, 0, 0)));


}

CRect	CPropertyListBox::GetItemWndRect(int nIndex) const
{
	CRect rc;
	GetItemRect(nIndex, &rc);
	rc.left += m_nDelemiter; 

	CWnd* pWnd = m_Windows[nIndex].m_Control.get();
#ifdef USE_COLORPICKER
	COXColorPickerCtrl * pPicker = dynamic_cast<COXColorPickerCtrl *>(pWnd);

	if(pPicker)
	{
		ClientToScreen(&rc);
	}
	else
#endif
	{
		rc.DeflateRect(0, 1, 1, 0);
		if(dynamic_cast< CComboBox* >(pWnd))
			rc += CSize(0,-2);
	}
	return rc;
}

void CPropertyListBox::OnSelchange() 
{
	// TODO: Add your control notification handler code here
	if(m_bDragMode) return;
	int nIndex = GetCurSel();
	if(nIndex == LB_ERR) return;
	CWnd* pWnd = m_Windows[nIndex].m_Control.get();

	CRect rc = GetItemWndRect(nIndex);

	
#ifdef USE_COLORPICKER
	COXColorPickerCtrl * pPicker = dynamic_cast<COXColorPickerCtrl *>(pWnd);

	if(pPicker)
	{
		ClientToScreen(&rc);
		if( pPicker->Pick(ID_POPUPBAR_ALIGNBOTTOM, &rc))
		{
			CString PropName = m_Windows[nIndex].m_Name;

			COLORREF cl = pPicker->GetSelectedColor();
			if( m_Windows[nIndex].m_Observer ) m_Windows[nIndex].m_Observer ->OnPropertyChange( PropName,cl  );
		}
		return;
	}
#endif
	if(dynamic_cast<CComboBox *>(pWnd))
	{
		rc.DeflateRect(0,0, 0, 5);
	}

	m_Windows[nIndex].m_Control->MoveWindow(&rc, TRUE);
	m_Windows[nIndex].m_Control->ShowWindow(SW_SHOW);
	m_Windows[nIndex].m_Control->SetFocus();
	m_Windows[nIndex].m_Control->Invalidate();
//	Invalidate();
}

struct Find1st : std::unary_function< CPropertyListBox::Item ,bool>
{
	CString m_Find;
	Find1st(CString Find): m_Find(Find){}
	bool operator()(const CPropertyListBox::Item& t) const
	{
		return t.m_Name == m_Find;
	}
};

CString CPropertyListBox::operator[] ( CString Name) const
{
	std::vector< Item >::const_iterator itr 
		= std::find_if( m_Windows.begin(), m_Windows.end(), Find1st(Name) );
	CString str;
	itr->m_Control->GetWindowText(str);
	return str;
}

void CPropertyListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	int nIndex = GetCurSel();
//	AfxMessageBox("zzz");
	if(nIndex != LB_ERR) 
	{
		m_Windows[nIndex].m_Control->ShowWindow(SW_HIDE);
//		Invalidate();
	
	}
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CPropertyListBox::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int nIndex = GetCurSel();
	if(nIndex != LB_ERR) 
	{
		m_Windows[nIndex].m_Control->ShowWindow(SW_HIDE);
		Invalidate();
	}
	CListBox::OnHScroll(nSBCode, nPos, pScrollBar);
}

int CPropertyListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	CRect rc;
	GetClientRect(&rc);
	m_nDelemiter = rc.Width()/2;


	m_Font.CreateFont(
	   15,                        // nHeight
	   0,                         // nWidth
	   0,                         // nEscapement
	   0,                         // nOrientation
	   FW_DONTCARE,               // nWeight
	   FALSE,                     // bItalic
	   FALSE,                     // bUnderline
	   0,                         // cStrikeOut
	   DEFAULT_CHARSET,           // nCharSet
	   OUT_DEFAULT_PRECIS,        // nOutPrecision
	   CLIP_DEFAULT_PRECIS,       // nClipPrecision
	   DEFAULT_QUALITY,           // nQuality
	   DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily
	   _T("Arial"));                 // lpszFacename




	return 0;
}

void CPropertyListBox::PreSubclassWindow()
{
	CRect rc;
	GetClientRect(&rc);
	m_nDelemiter = rc.Width()/2;


	m_Font.CreateFont(
	   15,                        // nHeight
	   0,                         // nWidth
	   0,                         // nEscapement
	   0,                         // nOrientation
	   FW_DONTCARE,               // nWeight
	   FALSE,                     // bItalic
	   FALSE,                     // bUnderline
	   0,                         // cStrikeOut
	   DEFAULT_CHARSET,           // nCharSet
	   OUT_DEFAULT_PRECIS,        // nOutPrecision
	   CLIP_DEFAULT_PRECIS,       // nClipPrecision
	   DEFAULT_QUALITY,           // nQuality
	   DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily
	   _T("Arial"));                 // lpszFacename
}

void CPropertyListBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if( m_bDragMode )
	{
		ClipCursor(0);
		m_bDragMode = false;
	}
	
	CListBox::OnLButtonUp(nFlags, point);
}

void CPropertyListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( IsOnDelemiter( point ) )
	{
		SetCursor( m_Cursor.Size );
		m_bDragMode = true;
		CRect rc;
		GetClientRect(&rc);
		ClientToScreen(&rc);
		ClipCursor( &rc );
	}

		
	CListBox::OnLButtonDown(nFlags, point);
}

void CPropertyListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if( m_bDragMode )
	{
		m_nDelemiter = point.x;
		Invalidate(FALSE);
		SetCursor( m_Cursor.Size );
	}
	else if( IsOnDelemiter( point ) )
	{
		SetCursor( m_Cursor.Size );
	}
	else
	{	
		SetCursor( m_Cursor.Arrow );
	}
	
	CListBox::OnMouseMove(nFlags, point);
}

bool	CPropertyListBox::IsOnDelemiter(CPoint point) const
{
	CRect rcClient;
	GetClientRect(&rcClient);
	int pt = m_nDelemiter;
	CRect rc( pt - 2, 0, pt + 2, rcClient.Height() );
	return TRUE == rc.PtInRect( point );
}

BOOL CPropertyListBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	int nIndex = GetCurSel();
	if(nIndex != LB_ERR) 
	{
		m_Windows[nIndex].m_Control->ShowWindow(SW_HIDE);
		CRect rc;
		GetItemRect(nIndex, &rc);
		InvalidateRect(rc);
	}


	return CListBox::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CPropertyListBox::OnEraseBkgnd( CDC* pDC )
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect( &rc, GetSysColor( COLOR_3DFACE ) );
	return TRUE;
}

void CPropertyListBox::OnCancelMode( )
{
	ClipCursor(0);
	m_bDragMode = false;
	SetCursor(m_Cursor.Arrow);
}

void	CPropertyListBox::Reset()
{
	m_Windows.clear();
	if(GetSafeHwnd())
	{
		ResetContent();
		GetParent()->SetWindowText(CString());
		GetParent()->SendMessage(WM_NCPAINT);
	}
}

HBRUSH CPropertyListBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	//return NULL;

	return CreateSolidBrush( GetSysColor( COLOR_BTNFACE) );
}

void CPropertyListBox::OnSize(UINT nType, int cx, int cy) 
{
	CListBox::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	int nIndex = GetCurSel();
	if(LB_ERR == nIndex) return;
	DWORD st = m_Windows[nIndex].m_Control->GetStyle();
	if(st & WS_VISIBLE) 
	{
		CRect rc = GetItemWndRect(nIndex);
		m_Windows[nIndex].m_Control->MoveWindow(&rc);
	}
}

void	CPropertyListBox::SetEditItem		
						(	CString Name, CString Value , 
							PropertyObserver* Obs )
{
	std::vector< Item >::iterator itr 
		= std::find_if( m_Windows.begin(), m_Windows.end(), Find1st(Name) );
	if( itr ==  m_Windows.end())
	{
		AddString(CString());
		CEdit* pEdit = new CEdit();
		pEdit->Create( WS_CHILD|ES_AUTOHSCROLL, CRect(0, 0, 0, 0), this, m_Windows.size()+1 );
		pEdit->SetWindowText(Value);
		pEdit->SetFont(&m_Font);
		m_Windows.push_back( Item(Name, Obs, pEdit) );
	}
	else
	{
		CString str;
		itr->m_Control->SetWindowText(Value);
	}
	Invalidate(FALSE);	
}

void	CPropertyListBox::SetNumEditItem		
						(	CString Name, CString Value , 
							PropertyObserver* Obs )
{
	std::vector< Item >::iterator itr 
		= std::find_if( m_Windows.begin(), m_Windows.end(), Find1st(Name) );
	if( itr ==  m_Windows.end())
	{
		AddString(CString());
		CNumEdit* pEdit = new CNumEdit();
	//	std::numeric_limits<DWORD>::max();
		
		pEdit->Create( WS_CHILD|ES_AUTOHSCROLL|ES_NUMBER, CRect(0, 0, 0, 0), this, m_Windows.size()+1 );
		float zz  = std::numeric_limits<LONG>::max();
		pEdit->SetRange(std::numeric_limits<LONG>::max(), std::numeric_limits<LONG>::min());
		pEdit->SetWindowText(Value);
		
		pEdit->SetFont(&m_Font);
		m_Windows.push_back( Item(Name, Obs, pEdit) );
	}
	else
	{
		CString str;
		itr->m_Control->SetWindowText(Value);
	}
	Invalidate(FALSE);	
}

void	CPropertyListBox::SetComboBoxItem	
						(	CString Name, const std::vector<CString>& Values, int nDefValue,
							bool bDropDown,
							PropertyObserver* Obs)
{
	std::vector< Item >::iterator itr 
		= std::find_if( m_Windows.begin(), m_Windows.end(), Find1st(Name) );
	if( itr ==  m_Windows.end())
	{
		AddString(CString());
		CRect rc;
		GetItemRect(m_Windows.size(),rc);
		rc.left = m_nDelemiter;

		CComboBox* pEdit = new CCJFlatComboBox();
		pEdit->Create( (bDropDown ? CBS_DROPDOWN : CBS_DROPDOWNLIST) | CBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VSCROLL, 
			CRect(0, 0, 0, 100), this, m_Windows.size()+1 );
		for(size_t i=0; i < Values.size(); ++i)
		{
			pEdit->AddString( Values[i] );
		}
		pEdit->SetCurSel(nDefValue);
//		pEdit->SetWindowText(Values[nDefValue]);
		pEdit->SetFont(&m_Font);
		m_Windows.push_back( Item(Name, Obs, pEdit) );
	}
	else
	{
		CString str;
		itr->m_Control->SetWindowText(Values[nDefValue]);
	}
	Invalidate(FALSE);	
}

void	CPropertyListBox::SetColorPicker( CString Name, COLORREF clDefault, PropertyObserver* Obs  )
{
#ifdef USE_COLORPICKER
	std::vector< Item >::iterator itr 
		= std::find_if( m_Windows.begin(), m_Windows.end(), Find1st(Name) );

		COXColorPickerCtrl* pPicker = new COXColorPickerCtrl();
		pPicker->Create(this );
		pPicker->SetSelectedColor( clDefault );
		pPicker->SetDefaultColor( clDefault );

	if( itr ==  m_Windows.end())
	{
		AddString(CString());
		m_Windows.push_back( Item(Name, Obs, pPicker) );		
	}
	else
	{
		CString str;
		itr->m_Control = boost::shared_ptr<CWnd>(pPicker);
	}
	Invalidate(FALSE);	
#endif
}

////////// Handlers ////////////////

void CPropertyListBox::OnEditKillFocus(UINT nID)
{
	m_Windows[nID-1].m_Control->ShowWindow(SW_HIDE);
}

void CPropertyListBox::OnEditChange(UINT nID)
{
	int nIndex = nID-1;
	if(nIndex >= m_Windows.size()) return;
	CString PropName = m_Windows[nIndex].m_Name;
	CString Value;
	m_Windows[nIndex].m_Control->GetWindowText(Value);
	if( m_Windows[nIndex].m_Observer ) m_Windows[nIndex].m_Observer ->OnPropertyChange( PropName, Value  );
}
/*
void CPropertyListBox::OnComboBoxKillFocus(int nID)
{
	CPropertyListBox::OnEditKillFocus( nID);
}
*/
void CPropertyListBox::OnComboBoxChange(UINT nID)
{
	//CPropertyListBox::OnEditChange( nID);

	int nIndex = nID-1;
	if(nIndex >= m_Windows.size()) return;
	CString PropName = m_Windows[nIndex].m_Name;
	CString Value;

	CComboBox* pCombo = dynamic_cast<CComboBox*>( m_Windows[nIndex].m_Control.get() );
	CString s;
	int nSel = pCombo->GetCurSel();
	pCombo->GetLBText(nSel,Value);
	m_Windows[nIndex].m_Control->SetWindowText(Value);
	Invalidate();
	if( m_Windows[nIndex].m_Observer ) m_Windows[nIndex].m_Observer ->OnPropertyChange( PropName,Value  );
	m_Windows[nIndex].m_Control->SetWindowText(Value);
	
}

void CPropertyListBox::OnComboBoxEditChange(UINT nID)
{
	CPropertyListBox::OnEditChange( nID);
}

void CPropertyListBox::OnEnterMenuLoop( BOOL bIsTrackPopupMenu )
{
	ClipCursor(0);
	m_bDragMode = false;
	SetCursor(m_Cursor.Arrow);	
}

/////////// End of Handlers ////////////