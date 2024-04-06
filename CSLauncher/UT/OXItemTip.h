// ==========================================================================
//						   Class Specification : 
//								COXItemTip
// ==========================================================================

// Copyright © Dundas Software Ltd. 1997 - 1998, All Rights Reserved
                          
// //////////////////////////////////////////////////////////////////////////

#ifndef _ITEMTIP_H
#define _ITEMTIP_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "OXHookWnd.h"
#include "OXItemTipWnd.h"


#ifndef _WIN32_IE
#if _MFC_VER>=0x0420
#define _WIN32_IE 0x0300
#else
#define _WIN32_IE 0x0200
#endif
#endif

// message to request info
#define WM_DOYOUNEEDITEMTIP			WM_USER+150

// offset from left and right side of ItemTipWindow
#define ID_ITEMTIP_XOFFSET			6

//result codes returned by NeedItemTip function
#define ID_NIT_SUCCESS				0x00000001
#define ID_NIT_WRONGFORMAT			0x00000002
#define ID_NIT_OUTOFCONTROLBORDER	0x00000003
#define ID_NIT_OUTOFITEMBORDER		0x00000004
#define ID_NIT_NOTHIDDEN			0x00000005
#define ID_NIT_CUSTOMREJECT			0x00000006

// max length of itemtip text
#define MAX_ITEMTIP					300


typedef struct _tagNEEDITEMTIPINFO
{
	POINT point;
	int row;
	int col;
	RECT rectItem;
	TCHAR itemText[MAX_ITEMTIP];
	int sizeText;
	int offset;
	LPARAM pFont;
	COLORREF clrText;
	COLORREF clrBackground;
	int alignment;
	int result;
} NEEDITEMTIPINFO, *LPNEEDITEMTIPINFO;

// special callback function that can be registered in Attach function
typedef void (CALLBACK* NEEDITEMTIP_HELPER) (LPARAM lpSource, LPARAM lpInfo);


/////////////////////////////////////////////////////////////////////////////
// Generic class that provides tips for items from List Control that are not 
// entirely visible. Derived from COXHookWnd - generic class that is 
// used to subclass any CWnd and provides capability to handle any message 
// routed to the hooked window before it. Handles WM_MOUSEMOVE messages to handle 
// drawing tips. When mouse is moved over any of items that only partially seen
// then item tip will be displayed (the same functionality as CTreeCtrl provides)
//
// To use it:
//
//	1)	call Attach to hook any CListCtrl based control
//
//	2)	for ownerdraw CListCtrl you have to provide additional information
//		about current item. Such information can be requested in two ways.
//		There is WM_DOYOUNEEDITEMTIP message which COXItemTip will send to
//		corresponding CListCtrl each time it needs to request specific 
//		information by means of NEEDITEMTIPINFO structure. Another way to 
//		provide item information is to use callback function that can be
//		set while calling COXItemInfo::Attach fuction and should be defined as:
//
//			void CallbackNeedItemTip(LPARAM lpSource, LPARAM lpInfo)
//
//		where lpSource is a pointer to COXItemTip object that is requesting 
//		item information and lpInfo is pointer to NEEDITEMTIPINFO structure.
//
//		As I mentioned before you have to provide item information for
//		COXItemTip if CListCtrl is ownerdrawfixed. But you can use the same 
//		technique to amend any item info that was collected by COXItemTip for
//		standard CListCtrl too. E.g. you can change the color of text or 
//		background or even font used to draw item tip window.
//
//		It is up to a programmer to choose the best way of providing item info.
//		As a possible solution I would recommend the next:
//		
//			a)	you want to use item tip functionality in your CListCtrl derived
//				class. Then it's good idea to provide handler for 
//				WM_DOYOUNEEDITEMTIP message.
//			b)	you want to use item tip functionality for any CListCtrl derived
//				class and you don't want to make any changes in the source code
//				and don't want to write any specific derivation. Then you can easily
//				register callback function.
//
//		You cannot use both way simultaneously because if you register callback 
//		function then WM_DOYOUNEEDITEMTIP message won't be sent.
//
//		Here is the description of NEEDITEMTIPINFO structure:
//
//			typedef struct _tagNEEDITEMTIPINFO
//			{
//				POINT point;
//				int row;
//				int col;
//				RECT rectItem;
//				TCHAR itemText[MAX_ITEMTIP];
//				int sizeText;
//				int offset;
//				LPARAM pFont;
//				COLORREF clrText;
//				COLORREF clrBackground;
//				int alignment;
//				int result;
//			} NEEDITEMTIPINFO, *LPNEEDITEMTIPINFO;
//
//		where:
//			point		-	current coordinates of mouse cursor, using this parameter 
//							you can	identify the item cell
//			row			-	item row
//			col			-	item column
//			rectItem	-	rectangle where item tip should be drawn, in particular 
//							the left-top point is the most important here
//			itemText	-	text to draw in item tip
//			sizeText	-	size of text
//			offset		-	offset from the right and left edge of item tip window 
//							to draw text in
//			pFont		-	pointer to a CFont object to be used to draw text in
//							item tip window
//			clrText		-	color to be used to draw text in item tip window
//			clrBackground	color to be used to fill background in item tip window
//			alignment	-	alignment used to draw text in item tip window, can be:
//							DT_LEFT
//							DT_CENTER
//							DT_RIGHT
//			result		-	result of collecting item information, can be:
//							ID_NIT_SUCCESS				-	info was successfully 
//															collected
//							ID_NIT_WRONGFORMAT			-	wrong format of CListCtrl 
//															(e.g. it is not in Report 
//															view)
//							ID_NIT_OUTOFCONTROLBORDER	-	point is out of valid 
//															area of CListCtrl items
//															
//							ID_NIT_OUTOFITEMBORDER		-	point is within CListCtrl
//															but doesn't lay in any 
//															item
//							ID_NIT_NOTHIDDEN			-	text of (row,col) is 
//															entirely seen 
//							ID_NIT_CUSTOMREJECT			-	user requests not to 
//															display an item tip
//
//	
//		Before WM_DOYOUNEEDITEMTIP message or callback function is fired above 
//		described structure is filled by COXItemTip::NeedItemTip function. But
//		if a hooked CListCtrl is ownerdrawfixed then next elements could be defined 
//		incorrectly: row, col, rectItem, itemText, offset.  
//
//
//	3)	In the case when you don't have and don't want to provide item info using
//		above described ways but still don't like the settings that COXItemTip
//		uses by default you can use next functions to change them:
//
//			// sets font to draw text
//			void SetTextFont(CFont* pFont);
//			// gets font to draw text
//			CFont* GetTextFont();
//
//			// sets color of text
//			void SetTextColor(COLORREF clrText);
//			// gets color of text
//			COLORREF GetTextColor();
//
//			// sets color of text
//			void SetBkColor(COLORREF clrBackground);
//			// gets color of text
//			COLORREF GetBkColor();
//
//			// sets offset from the right and left edge of item tip window
//			// to draw text in
//			void SetOffset(int nOffset)
//			// gets offset from the right and left edge of item tip window
//			// to draw text in
//			int GetOffset();
//

class OX_CLASS_DECL COXItemTip : public COXHookWnd 
{
protected:
	DECLARE_DYNAMIC(COXItemTip);

	// pointer to hooked CListCtrl
	CListCtrl* m_pListCtrl;
	// item tip window used to display item tips
	COXItemTipWnd m_ItemTipWnd;
	
	// callback function that can be registered to provide or amen item info 
	// before it is displayed
	NEEDITEMTIP_HELPER m_pfnCallback;

	// font to draw text
	CFont* m_pFont;
	// color to draw text
	COLORREF m_clrText;
	// color of background
	COLORREF m_clrBackground;
	// offset to draw text
	int m_nOffset;

	// The heart of the class. Handle all needed messages. While deriving
	// your own class make sure to call this function. I wouldn't recommend you 
	// to change anything in it
	virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);	
	// --- In  :	msg	-	message ID
	//				wp	-	WPARAM
	//				lp	-	LPARAM
	// --- Out : 
	// --- Returns:	result of message handling. Different for different messages.
	// --- Effect :	Sets the number of color shades used in gradient filling. 

	virtual void NeedItemTip(LPNEEDITEMTIPINFO pnitInfo);
	// --- In  :	pnitInfo	-	pointer to NEEDITEMTIPINFO structure that 
	//								should be filled as a result of this function
	// --- Out : 
	// --- Returns:
	// --- Effect : fills pnitInfo with item information

public:
	COXItemTip();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	virtual ~COXItemTip();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destroys the object

	BOOL Attach(CWnd* pWnd, NEEDITEMTIP_HELPER pfnCallback=NULL);
	// --- In  :	pWnd		-	pointer to created CListCtrl-derived control
	//								to be hooked
	//				pfnCallback	-	callback function to be called to request item 
	//								info (must  be implemented if CListCtrl is 
	//								ownerdrawfixed). If pfnCallback is NULL then
	//								WM_DOYOUNEEDITEMTIP message will be sent (this 
	//								is an alternative way of providing item info).
	// --- Out : 
	// --- Returns:	TRUE if pWnd was successfully hooked, otherwise FALSE
	// --- Effect : Hooks window
	void Detach();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Unhooks attached window if any was hooked

	void Reset();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : set font, text color, background color and offset to their
	//				default values.

	inline void SetTextFont(CFont* pFont) { m_pFont=pFont; }
	// --- In  :	pFont	-	pointer to CFont object to be used to draw text
	//							in item tip window
	// --- Out : 
	// --- Returns:
	// --- Effect : sets the font to draw text. By default we use the font 
	//				associated with hooked CListCtrl.
	inline CFont* GetTextFont() { return m_pFont; }
	// --- In  :	
	// --- Out : 
	// --- Returns:	pointer to CFont object to be used to draw text	in item tip 
	//				window
	// --- Effect : gets the font to draw text

	inline void SetTextColor(COLORREF clrText) { m_clrText=clrText; }
	// --- In  :	clrText	-	color to be used to draw text in item tip window
	// --- Out : 
	// --- Returns:
	// --- Effect : sets color of text. By default we use the color returned by 
	//				CListCtrl::GetTextColor function applied to the hooked CListCtrl.
	inline COLORREF GetTextColor() { return m_clrText; }
	// --- In  :	
	// --- Out : 
	// --- Returns:	color to be used to draw text in item tip window
	// --- Effect : gets the color to draw text

	inline void SetBkColor(COLORREF clrBackground) { m_clrBackground=clrBackground; }
	// --- In  :	clrBackground	-	color to be used to fill background 
	//									in item tip window
	// --- Out : 
	// --- Returns:
	// --- Effect : sets color of background. By default we use the background color 
	//				used in ToolTip controls.
	inline COLORREF GetBkColor() { return m_clrBackground; }
	// --- In  :	
	// --- Out : 
	// --- Returns:	color to be used to fill background in item tip window
	// --- Effect : gets the color of background

	inline void SetOffset(int nOffset) { m_nOffset=nOffset; }
	// --- In  :	nOffset	-	offset from the right and left edge of item tip 
	//							window to draw text in
	// --- Out : 
	// --- Returns:
	// --- Effect : sets the size of an offset. By default we use the 
	//				ID_ITEMTIP_XOFFSET constant
	inline int GetOffset() { return m_nOffset; }
	// --- In  :	
	// --- Out : 
	// --- Returns:	offset from the right and left edge of item tip window to draw 
	//				text in
	// --- Effect : gets the size of an offset.

private:
	int GetRealColumn(int nCol);
	// --- In  :	nCol	-	number of column
	// --- Out : 
	// --- Returns:	real number of column taking into account order of column
	//				that could be changed starting from comctl32.dll version 4.70
	// --- Effect : 

};


#endif // _ITEMTIP_H

