#ifndef  A_PARAMETER_LIB_DIALOG_H
#define  A_PARAMETER_LIB_DIALOG_H

#ifdef WIN32

//=================================================================================================
// Dialog page that contains controls of any section of parameter structure.
//=================================================================================================
class /*ALIB_API*/ CParameterPage : public CPropertyPage
{
private:
  friend class CParameterDialog;
  DECLARE_DYNCREATE(CParameterPage)

  std::auto_ptr<DLGTEMPLATE>   m_pDlgTemplate;  //!< pointer to the dialog template storage
  paramlib::BaseParamSection * m_pSection;      //!< parameter section that is supplied by this page
  CWnd                       * m_controls;      //!< array of dialog's controls
  int                          m_nControl;      //!< number of controls
  bool                         m_bCreated;      //!< non-zero if the dialog has been created

 public:
  CParameterPage();
  virtual ~CParameterPage();
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange( CDataExchange * pDX );

 protected:
  DECLARE_MESSAGE_MAP()
};


//=================================================================================================
// Dialog that contains all pages of all parameter sections.
//=================================================================================================
class /*ALIB_API*/ CParameterDialog : public CPropertySheet
{
private:
  DECLARE_DYNAMIC(CParameterDialog)
  CParameterDialog();                  // no default constructor

public:
  CParameterDialog( paramlib::BaseParamSection & root, CWnd * pParent, int cx, int cy );
  virtual ~CParameterDialog();

protected:
  DECLARE_MESSAGE_MAP()
};

#endif // WIN32

#endif // A_PARAMETER_LIB_DIALOG_H

