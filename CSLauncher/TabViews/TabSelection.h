//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	helper class for easy tab selection
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 20.03.2005
//                                                                                      //
//======================================================================================//
#ifndef _TAB_SELECTION_3802624824689572_
#define _TAB_SELECTION_3802624824689572_
//======================================================================================//
//                                  class TabSelection                                  //
//======================================================================================//
#include <boost/shared_ptr.hpp>

class SelectionTag
{
#if 0
	LPARAM				m_lCookie;
	static	LPARAM	GenerateCookie()
	{
		static LPARAM s_lCurCookie = 0;
		return InterlockedIncrement( &s_lCurCookie );
	}
#endif
protected:
	SelectionTag() {}
	virtual ~SelectionTag(){};
public:
#if 0
	LPARAM				GetCookie() const { return m_lCookie; };
#endif
	virtual bool		operator == ( const SelectionTag& rhs ) = 0;
	virtual CString		GetTabName() 							= 0;
};

class TabSelection
{
	boost::shared_ptr<SelectionTag>	m_Tag;
public:
	void					SetTag(boost::shared_ptr<SelectionTag> p ) { m_Tag = p;}
	const SelectionTag*		GetTag() const { return m_Tag.get() ;}
	TabSelection();
	virtual ~TabSelection();
};

#endif // _TAB_SELECTION_3802624824689572_