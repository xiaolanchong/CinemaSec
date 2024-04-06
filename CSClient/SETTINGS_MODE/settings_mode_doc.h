#pragma once

#if _MSC_VER <= 1200
#pragma warning( disable : 4097 )
#endif 

class CSettingsView;

class CSettingsDoc : public CDocument
{
protected:
  DECLARE_DYNCREATE(CSettingsDoc)
  CSettingsDoc();
  virtual ~CSettingsDoc();

	CSettingsView* GetSettingsView();

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

public:
  //{{AFX_MSG(CSettingsDoc)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


