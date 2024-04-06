#include "../../CSChair/XML/XmlLite.h"
#include "PlayListParser/PlayListItem.h"

struct PlayListParameters {
	int simulated_fps;
	bool waitForDllStop;
	PlaylistItem item;
	PlayListParameters()
		: simulated_fps(1)
		, waitForDllStop(true)
	{
	}
	bool LoadParameters(LPCWSTR fname)
	{
		VARIANT_BOOL b;
		XmlLite::XMLDocument doc;
		doc.m_pDoc->load( CComVariant(fname), &b );
		if (b != VARIANT_TRUE) return false;
		XmlLite::XMLElement rootElem( doc.GetRoot() );

		try
		{
			rootElem.GetAttr(_T("simulated_fps"), simulated_fps);

			std::wstring s;
			rootElem.GetAttr(_T("wait_for_dll_stop"), s);
			if (s == L"yes") 
				waitForDllStop = true;
			else
				waitForDllStop = false;

		}
		catch (...)
		{
			return false;
		}
        return true;
	}

	// copy constructor
	PlayListParameters& operator = (const PlayListParameters& rh)
	{	
		this->simulated_fps = rh.simulated_fps;
		this->waitForDllStop = rh.waitForDllStop;
		this->item = rh.item;
		return (*this);
	}
};

