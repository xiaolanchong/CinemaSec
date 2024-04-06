#pragma once

#if _MSC_VER <= 1200
#pragma warning( disable : 4097 )
#endif 

#include "../../CSEngine/DBFacet/TimeHelper.h"

class CTimeTableView;

class CTimeTableDoc : public CDocument
{
protected:
  DECLARE_DYNCREATE(CTimeTableDoc)
  CTimeTableDoc();
  virtual ~CTimeTableDoc();

	CTimeTableView* GetTimeTableView();
public:

	void				Save(int nRoomID, const CTime& timeBegin, const CTime&  timeEnd, const TimeTable_t& tt, const std::vector<std::wstring>& Names);
	void				Load(int nRoomID, const CTime& timeBegin, const CTime&  timeEnd, TimeTable_t& tt, std::vector<std::wstring>& Names);
	void				Validate( const TimeTable_t& tt);

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

public:
  //{{AFX_MSG(CTimeTableDoc)

  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


