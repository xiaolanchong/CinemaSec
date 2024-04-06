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

class CTexture;

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

	virtual void SetSource( const BITMAPINFO* pBitmapInfo, const BYTE* pSrcData );
	virtual CTexture* GetTexture();
	virtual std::pair<int, int> GetId();
	virtual void SetId( int idx, int idy );
	virtual void SetIdX( int idx );
	virtual void SetIdY( int idY );
	virtual int GetIdX();
	virtual int GetIdY();



private:
	int m_nIdX;
	int m_nIdY;

	boost::shared_ptr<CTexture> m_pTexture;
	CElementPtrList m_ElementList;

};


#endif //__CAMERA_HPP__
