#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#include <boost/any.hpp>
#include <vector>

struct  PropertyObserver
{
	virtual void OnPropertyChange(CString Name, const boost::any& Value) = 0;
};

	struct PropertyCB
	{
		virtual void SetEditProperty(CString Name, CString Value, PropertyObserver* pObserver) = 0;
		virtual void SetNumEditProperty(CString Name, CString Value, PropertyObserver* pObserver) = 0;
		virtual void SetComboProperty(CString Name, std::vector<CString> Values, int nDefValue, PropertyObserver* pObserver) = 0;
		virtual void SetEditComboProperty(CString Name, std::vector<CString> Values, int nDefValue, PropertyObserver* pObserver) = 0;

		// 0 - false, 1 - true, -1 - undefined
		virtual void SetBoolProperty(CString Name, int  nDefValue, PropertyObserver* pObserver) = 0;
		virtual void SetHeader(CString Name) = 0;
	};

struct ReadOnlyProperty
{
	virtual void SetReadOnlyProperty(CString Name, CString Value) = 0 ;
};

#endif  // _CALLBACK_H_