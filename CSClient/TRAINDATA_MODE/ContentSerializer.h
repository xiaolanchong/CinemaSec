// ContentSerializer.h: interface for the ContentSerializer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTENTSERIALIZER_H__FF6DBC54_4DAF_435C_BEA9_735EB6F5962E__INCLUDED_)
#define AFX_CONTENTSERIALIZER_H__FF6DBC54_4DAF_435C_BEA9_735EB6F5962E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../CSAlgo/ALGORITHM/base_chair.h"
#include "../../CSChair/IChairContentEx.h"
#include "../DataPrepare/my_chair.h"
#include "../COMMON/Exception.h"

MACRO_EXCEPTION( ContentSerializerException,		CommonException )
MACRO_EXCEPTION( ContentSerializerInvalidVersion, ContentSerializerException )
MACRO_EXCEPTION( ContentSerializerInvalidFile,	ContentSerializerException )
MACRO_EXCEPTION( ContentInvalidValue,			ContentSerializerException )

typedef std::map< int , RangeList_t >			ChairTrainList_t;
typedef std::vector<BaseChair>					BaseChairArr_t;

class ContentSerializer  
{
protected:
	static void Convert(	const MyChair::ChairSet& chSet, BaseChairArr_t& BaseChairs  );
	static void Convert(	const BaseChairArr_t& chSet, std::vector<RangeList_t>&	RangeListArr, const ChairTrainList_t&   );
	static void Convert(	const BaseChairArr_t& chSet, ChairTrainList_t& ,	const std::vector<RangeList_t>&	RangeListArr );
};

class ContentLoader : protected ContentSerializer
{
public:
	void Load(	LPCTSTR szFileName,
				const MyChair::ChairSet& chset,
				ChairTrainList_t& ctl, 
				CString& strVideoFile ,
				CString& strGaugeFile) 
				const THROW_HEADER (ContentSerializerException);
};

class ContentSaver: protected ContentSerializer
{
public:
	void Save(	LPCTSTR szFileName, 
				const MyChair::ChairSet& chset,
				const ChairTrainList_t& ctl, 
				CString strVideoFile,
				CString strGaugeFile
				) THROW_HEADER (ContentSerializerException);
};

#endif // !defined(AFX_CONTENTSERIALIZER_H__FF6DBC54_4DAF_435C_BEA9_735EB6F5962E__INCLUDED_)
