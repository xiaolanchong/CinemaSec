
#pragma once


// CDropSource command target

class CDropSource : public COleDropSource
{
	DECLARE_DYNAMIC(CDropSource)

public:
	CDropSource();
	virtual ~CDropSource();
public:


protected:
	DECLARE_MESSAGE_MAP()
};

