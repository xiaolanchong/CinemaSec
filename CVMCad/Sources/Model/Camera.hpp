/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Camera.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-15
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

class IBaseProperty;
class CElement;
class CScene;
class CCameraSource;
class CNode;
class CCamera;

typedef std::list<CCamera*> CCameraPtrList;
typedef std::vector<CCamera*> CCameraPtrArray;
typedef CCameraPtrList::iterator CCameraPtrListIt;
typedef CCameraPtrArray::iterator CCameraPtrArrayIt;

class CCamera : public CNode
{
public:
	CCamera( );
	CCamera( const CCamera& camera );
	virtual ~CCamera();

public:
	CElement* AddElement();
	CElement* AddElement( const CElement& element );
	void RemoveElement( CElement* pElement );
	CNodePtr GetChild( int n );

	void SetStatus( bool bActive );
	bool GetStatus();


	virtual void SetSource( BITMAPINFO* pBitmapInfo, BYTE* pSrcData );
	virtual void SetSource( const wstring& sFileName );
	virtual void GetSource( BITMAPINFO** ppBitmapInfo, BYTE** ppSrcData );
	virtual void GetSource( wstring& sFileName );
	virtual std::pair<int, int> GetId();
	virtual void SetId( int idx, int idy );
	virtual void SetIdX( int idx );
	virtual void SetIdY( int idY );



private:
	int m_nIdX;
	int m_nIdY;
	wstring m_sFileName;
	CBitmap* m_pBitmap;
	bool m_bActive;
	BITMAPINFO* m_pBitmapInfo;
	BYTE* m_pSrcData;

	CElementPtrList m_ElementList;

};


#endif //__CAMERA_HPP__
