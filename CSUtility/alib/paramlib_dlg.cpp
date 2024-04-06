#include "stdafx.h"

#ifdef WIN32

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Note how a CPropertySheet usually works. When you call DoModal() the first CPropertyPage
// object is initialized (OnInitDialog()), but, by default, all other pages still uninitialized.
// The object CPropertyPage begins life-cycle if user at least once opened this pages clicking
// the mouse on appropriate tab. The page still living until dialog closure.

// All constants are given in dialog units.
const int DIALOG_BORDER = 7;
const int CONTROL_HEIGHT = 12;
const int CONTROL_WIDTH = 24;
const int COMBO_BOX_DROPDOWN_HEIGHT = (1<<10);
const int CHECK_BOX_WIDTH = 10;
const int GAP_BEFORE_COMMENT = 3;
const int COMMENT_DEFLATION = 2;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of CParameterPage
///////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CParameterPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CParameterPage, CPropertyPage)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
CParameterPage::CParameterPage() : CPropertyPage()
{
  m_pDlgTemplate.reset();
  m_pSection = 0;
  m_controls = 0;
  m_nControl = 0;
  m_bCreated = false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function is called by the framework to exchange and validate dialog data
           between this dialog page and associated parameter section.

  \param  pDX  a pointer to a CDataExchange object (see MFC documentation). */
///////////////////////////////////////////////////////////////////////////////////////////////////
void CParameterPage::DoDataExchange( CDataExchange * pDX )
{
  if (!m_bCreated)
    return;

  CPropertyPage::DoDataExchange( pDX );

  if (m_pSection == 0)
    return;

  paramlib::BaseParamField * pParameter = m_pSection->first();
  for (; pParameter != 0; pParameter = pParameter->next())
  {
    switch (pParameter->type())
    {
      case paramlib::BaseParamField::INTEGER:
      {
        paramlib::IntParam * p = (paramlib::IntParam*)pParameter;
        int v = (*p)();
        ::DDX_Text( pDX, p->id(), v );
        if (pDX->m_bSaveAndValidate)
          (*p) = v;
        break;
      }

      case paramlib::BaseParamField::FLOAT:
      {
        paramlib::FltParam * p = (paramlib::FltParam*)pParameter;
        float v = (*p)();
        ::DDX_Text( pDX, p->id(), v );
        if (pDX->m_bSaveAndValidate)
          (*p) = v;
        break;
      }

      case paramlib::BaseParamField::BOOLEAN:
      {
        paramlib::BlnParam * p = (paramlib::BlnParam*)pParameter;
        int v = ((*p)()) ? 1 : 0;
        ::DDX_Check( pDX, p->id(), v );
        if (pDX->m_bSaveAndValidate)
          (*p) = (v != 0);
        break;
      }

      case paramlib::BaseParamField::CHAR_SET:
      {
        paramlib::ChrParam * p = (paramlib::ChrParam*)pParameter;
        CString v( (*p)() );
        ::DDX_Text( pDX, p->id(), v );
        if (pDX->m_bSaveAndValidate)
        {
          if (!v.IsEmpty())
            (*p) = v[0];
        }
        else
        {
          CComboBox                        * pCombo = (CComboBox*)GetDlgItem( p->id() );
          const std::basic_string<wchar_t> & set = p->get_char_set();
          wchar_t                            txt[2];

          ASSERT( pCombo != 0 );
          ASSERT( (pCombo->GetCount() == 0) || (pCombo->GetCount() == (int)(set.size())) );

          bool bFilled = (pCombo->GetCount() == (int)(set.size()));

          for (unsigned int i = 0; i < set.size(); i++)
          {
            txt[0] = set[i];
            txt[1] = '\0';

            int index = bFilled ? i : pCombo->AddString( txt ); // Is already filled?

            ASSERT( index != CB_ERR );
            if ((*p)() == txt[0])
              pCombo->SetCurSel( index );
          }
        }
        break;
      }

      default: ASSERT(0);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function initializes this pages, creates controls and fills controls by the
           values of parameters of associated parameter section. */
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CParameterPage::OnInitDialog() 
{
  if (!(CPropertyPage::OnInitDialog()))
    return FALSE;
  if (m_pSection == 0)
    return FALSE;

  paramlib::BaseParamField * pf = 0;
  CRect                      dlgRect( DIALOG_BORDER, DIALOG_BORDER, DIALOG_BORDER + CONTROL_WIDTH,
                                      DIALOG_BORDER + CONTROL_HEIGHT );

  const int EDIT_STYLE      = ES_AUTOHSCROLL | WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER;
  const int TEXT_STYLE      = SS_CENTERIMAGE | WS_CHILD | WS_VISIBLE;
  const int CHECK_BOX_STYLE = BS_AUTOCHECKBOX | WS_CHILD | WS_TABSTOP | WS_VISIBLE;
  const int COMBO_BOX_STYLE = CBS_DROPDOWNLIST | WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER;

  // Determine number of parameters of this page and number of controls. Create array of controls.
  m_nControl = 0;
  for (pf = m_pSection->first(); pf != 0; pf = pf->next())
    ++m_nControl;
  m_nControl *= 2;                     // control itself + static control for comment
  m_controls = (m_nControl > 0) ? (new CWnd[ m_nControl ]) : 0;

  CClientDC dc( this );
  CWnd  *   pWin = m_controls;
  CWnd  *   pEnd = m_controls + m_nControl;
  CFont *   pFont = GetFont();
  CFont *   pOldFont = dc.SelectObject( pFont );
  int       width = 0;

  // Determine maximal width of comment string in screen coordinates.
  for (pf = m_pSection->first(); pf != 0; pf = pf->next())
  {
    SIZE size = dc.GetOutputTextExtent( pf->comment(), (int)(_tcslen( pf->comment() )) );
    width = std::max( width, (int)(size.cx) );
  }
  width += 2;
  dc.SelectObject( pOldFont );

  // For each parameter of associated section create control element for parameter itself
  // and static control for comment, if necessary. Identifier of comment control is equal
  // to identifier of parameter control + 1.
  if (pWin != 0)
  {
    for (pf = m_pSection->first(); pf != 0;)
    {
      CRect rect = dlgRect;
      MapDialogRect( &rect );

      switch (pf->type())
      {
        case paramlib::BaseParamField::INTEGER:
        case paramlib::BaseParamField::FLOAT:
        {
          rect = dlgRect;
          MapDialogRect( &rect );

          pEnd;
          ASSERT( pWin < pEnd );
          if (!(pWin->CreateEx( WS_EX_CLIENTEDGE, _T("EDIT"), _T(""), EDIT_STYLE,
                                rect, this, pf->id() )))
            return FALSE;
          pWin->SetFont( pFont );
          ++pWin;

          rect = dlgRect;
          rect.left = rect.right + GAP_BEFORE_COMMENT;
          rect.top += COMMENT_DEFLATION;
          rect.bottom -= COMMENT_DEFLATION;
          MapDialogRect( &rect );
          rect.right = rect.left + width;

          ASSERT( pWin < pEnd );
          if (!(pWin->Create( _T("STATIC"), pf->comment(),
                              TEXT_STYLE, rect, this, pf->id()+1 )))
            return FALSE;
          pWin->SetFont( pFont );
          ++pWin;
          break;
        }

        case paramlib::BaseParamField::BOOLEAN:
        {
          rect = dlgRect;
          rect.left += GAP_BEFORE_COMMENT;
          rect.right = rect.left + CHECK_BOX_WIDTH;
          rect.top += COMMENT_DEFLATION;
          rect.bottom -= COMMENT_DEFLATION;
          MapDialogRect( &rect );
          rect.right += width;

          ASSERT( pWin < pEnd );
          if (!(pWin->Create( _T("BUTTON"), pf->comment(),
                              CHECK_BOX_STYLE, rect, this, pf->id() )))
            return FALSE;
          pWin->SetFont( pFont );
          ++pWin;
          break;
        }

        case paramlib::BaseParamField::CHAR_SET:
        {
          rect = dlgRect;
          rect.bottom += COMBO_BOX_DROPDOWN_HEIGHT;
          MapDialogRect( &rect );

          ASSERT( pWin < pEnd );
          if (!(pWin->CreateEx( WS_EX_CLIENTEDGE, _T("COMBOBOX"), _T(""),
                                COMBO_BOX_STYLE, rect, this, pf->id() )))
            return FALSE;
          pWin->SetFont( pFont );
          ++pWin;
        
          rect = dlgRect;
          rect.left = rect.right + GAP_BEFORE_COMMENT;
          rect.top += COMMENT_DEFLATION;
          rect.bottom -= COMMENT_DEFLATION;
          MapDialogRect( &rect );
          rect.right = rect.left + width;

          ASSERT( pWin < pEnd );
          if (!(pWin->Create( _T("STATIC"), pf->comment(), TEXT_STYLE,
                              rect, this, pf->id()+1 )))
            return FALSE;
          pWin->SetFont( pFont );
          ++pWin;
          break;
        }
      }

      pf = pf->next();
      if (pf != 0)
        dlgRect.OffsetRect( 0, CONTROL_HEIGHT );
    }
    ASSERT( pWin <= pEnd );
  }

  // Code below doesn't work. One must manually set dialog size via template resource.
  //dlgRect.SetRect( 0, 0, 2*DIALOG_BORDER+CONTROL_WIDTH, DIALOG_BORDER+dlgRect.bottom );
  //MapDialogRect( &dlgRect );
  //dlgRect.right += width;
  //MoveWindow( &dlgRect, FALSE );

  // Initialize control element by parameter values (DoDataExchange(..) will be called).
  m_bCreated = true;
  return UpdateData( FALSE );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Destructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
CParameterPage::~CParameterPage()
{
  delete [] m_controls;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of CParameterDialog
///////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CParameterDialog, CPropertySheet)

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor.

  \param  root     the top parameter section that contains subsections of parameters.
  \param  pParent  pointer to the parent window or 0.
  \param  cx       the width of a property page (in dialog units).
  \param  cy       the height of a property page (in dialog units). */
///////////////////////////////////////////////////////////////////////////////////////////////////
CParameterDialog::CParameterDialog( paramlib::BaseParamSection & root, CWnd * pParent, int cx, int cy )
: CPropertySheet( _T("Parameters"), pParent, 0 )
{
  const WCHAR CAPTION[] = L"Parameter Page";
  const WCHAR TYPEFACE[] = L"MS Sans Serif";
  const int   SIZE = sizeof(DLGTEMPLATE)+sizeof(CAPTION)+sizeof(TYPEFACE)+256*sizeof(WORD);

  try
  {
    // Create a property sheet where each property page corresponds to a parameter section.
    paramlib::BaseParamField * pSection = root.first();
    for (; pSection != 0; pSection = pSection->next())
    {
      // Top parameter structure must consist of sections only.
      ALIB_ASSERT( pSection->type() == paramlib::BaseParamField::SECTION );

      // Create property page.
      CParameterPage * pPage = new CParameterPage();
      ALIB_ASSERT( pPage != 0 );

      // Create the storage of dialog template (in fact, property page is a modeless dialog).
      pPage->m_pDlgTemplate.reset( reinterpret_cast<DLGTEMPLATE*>( new char [SIZE] ) );
      DLGTEMPLATE * pTempl = pPage->m_pDlgTemplate.get();
      ALIB_ASSERT( pTempl != 0 );
      memset( pTempl, 0, SIZE );

      // Fill dialog template structure.
      {
        pTempl->style = DS_CONTROL | WS_CHILD | WS_CAPTION | DS_CENTER |
                        DS_3DLOOK | WS_VISIBLE | DS_SETFONT;
        pTempl->dwExtendedStyle = 0; 
        pTempl->cdit = 0;
        pTempl->x = 0;
        pTempl->y = 0;
        pTempl->cx = (short)cx;
        pTempl->cy = (short)cy;

        WORD * pw = reinterpret_cast<WORD*>( pTempl+1 );
        *pw++ = 0;
        *pw++ = 0;
        memcpy( pw, CAPTION, sizeof(CAPTION) );
        pw += sizeof(CAPTION);
        *pw++ = 16;
        memcpy( pw, TYPEFACE, sizeof(TYPEFACE) ); // WM_SETFONT message must be processed???
      }

      // Fill some fields of property page setup structure.
      pPage->m_pSection = (paramlib::BaseParamSection*)pSection;
      pPage->m_strCaption = pSection->name();
      pPage->m_psp.pszTitle = pPage->m_strCaption;
      pPage->m_psp.dwFlags |= (PSP_USETITLE | PSP_DLGINDIRECT);
      pPage->m_psp.pResource = pTempl;

      // Add page to the property sheet without explicit creation (it will be created automatically).
      AddPage( pPage );
    }
  }
  catch (...)
  {
    const wchar_t ErrMsg[] = _T("Failed to create parameter dialog\n");
    TRACE( ErrMsg );
    alib::ErrorMessage( ErrMsg );
  }

  m_psh.dwFlags |= PSH_NOAPPLYNOW;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Destructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
CParameterDialog::~CParameterDialog()
{
  for (int i = 0; i < GetPageCount(); i++)
  {
    CPropertyPage * pPage = GetPage( i );
    delete pPage;
  }
}


BEGIN_MESSAGE_MAP(CParameterDialog, CPropertySheet)
END_MESSAGE_MAP()

#endif // WIN32

