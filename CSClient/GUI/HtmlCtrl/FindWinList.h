
//////////////////
// Top-level window list, STL vector of HWNDs. To use:
//
//		CWinList wl;		// window list
//		wl.Fill();			// fill it
//		for (CWinList::iterator it=wl.begin(); it!=wl.end(); it++) {
//			HWND hwnd = *it;
//			// do something with it
//		}
//
// Derived classes can override OnWindow to filter windows--eg, only visible.
// If you specify a parent window, CWinList will generate a deep child window
// list:
//
//		CWinList wl(hwndParent);
//		wl.Fill();
//
// In this case, CWinList uses EnumChildWindows instead of EnumWindows.
//
class CWinList : public std::vector<HWND> {
protected:
	HWND m_hwndParent;
	static BOOL CALLBACK EnumProc(HWND hwnd, LPARAM lp);
	// override to filter different kinds of windows
	virtual BOOL OnWindow(HWND /* hwnd */) {
		return TRUE;
	}
public:
	CWinList(HWND hwndParent=NULL, UINT nReserve=25);
	virtual ~CWinList() { }
	UINT Fill();
};

//////////////////
// CFindWinList finds all windows with a given class name at top level or as a
// descendant of a given window. To use it, instantiate like so:
//
//		CFindWinList fwl(hwndParent, classname);
//		fwl.Fill();
//
// CFindWinList is now an array of HWNDs with the desired classname. Use
// hwndParent=NULL to search top-level windows only; otherwise CFindWinList
// does a deep search. Iterate using STL as for CWinList.
//
class CFindWinList : public CWinList {
protected:
	virtual BOOL OnWindow(HWND hwnd);	 // override
public:
	LPCTSTR m_lpClassName; // class name to look for
	CFindWinList(HWND hwndParent, LPCTSTR classname, int nReserve=25)
		: CWinList(hwndParent, nReserve), m_lpClassName(classname) { }
};
