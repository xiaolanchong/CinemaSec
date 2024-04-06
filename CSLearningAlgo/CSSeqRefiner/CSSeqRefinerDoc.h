// CSSeqRefinerDoc.h : interface of the CCSSeqRefinerDoc class
//


#pragma once

class CCSSeqRefinerDoc : public CDocument
{
protected: // create from serialization only
	CCSSeqRefinerDoc();
	DECLARE_DYNCREATE(CCSSeqRefinerDoc)

// Attributes
public:

// Operations
public:
	int GetImageWidth();
	int GetImageHeight();
	
	bool SetImageWidth( int w );
	bool SetImageHeight( int h );

	
	int GetImageCount();

	void GetImage( int frameNum, unsigned __int32& frameTime, Arr2ub& image );
	void SetImage( int frameNum, unsigned __int32 frameTime, const Arr2ub& image );
	void AddImage( unsigned __int32 frameTime, const Arr2ub& image );
	void RemoveImage( int frameNum );



// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CCSSeqRefinerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	std::vector< std::pair<Arr2ub, unsigned __int32> > mImages;

	int mImageWidth;
	int mImageHeight;
	int mImageCount;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


