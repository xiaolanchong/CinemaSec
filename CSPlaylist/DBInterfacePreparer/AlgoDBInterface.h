// AlgoDBInterface.h: interface for the CAlgoDBInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALGODBINTERFACE_H__22BF78E5_7AED_4255_B74A_18408A05475D__INCLUDED_)
#define AFX_ALGODBINTERFACE_H__22BF78E5_7AED_4255_B74A_18408A05475D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(STRUCT_AlgoDBFileHeader)
#define STRUCT_AlgoDBFileHeader
struct AlgoDBFileHeader // ��������� �������� �����
{
	char label[4];		// ������ ����� "ALGO"
	long id;			// ������������� ������� ���� ������
	long HeaderLen;		// ����� ����� ��������� �����
	long NumOfRecords;	// ����� �������
	long UserFlags;		// �������������� ���������� ��� ������������
	char Description[1];// ����������� � �������� � ����������� �����; ����� �������� ����� ���� ������������
};
#endif ///STRUCT_AlgoDBFileHeader

struct AlgoDBFileRecord // ������
{
	long LenOfRecord;	// ������ ������
	BYTE DataBlock[1];	// ���� ������ ������������ ������
};

struct AlgoDBParametersGroups      //������ ������� ����������
{
	long NumOfGroups;		//���������� �����
	WCHAR **NameOfGroup;		//�������� �����
	long *NumOfParameters;	//���������� ��������� � ������ ������
	WCHAR ***NameOfParameter;//�������� ����������
};

struct AlgoDBAnswerGroups      //������ �������
{
	long NumOfGroups;		//���������� �����
	WCHAR **NameOfGroup;		//�������� �����
	long *NumOfAnswers;		//���������� ������� � ������ ������
	WCHAR ***NameOfAnswers;	//�������� �������
};
struct AlgoDBInfoColumns	//�������� ������� � ����������� ��� ����������� ������� (��������, �������)
{
	long NumOfColumns;		//���������� ��������
	WCHAR **NameOfColumns;	//�������� ��������	
	long *TypeOfColumns;	//��� ����������: 1-Picture, 2 - Text
};

struct AlgoLearningSystems	//��������� �������
{
	long NumOfSystems;		//���������� ������
	WCHAR **NameOfSystems;	//�������� ������		
};

struct AlgoFullInfoAboutRecord //��������� ���������� � ������� ��� ������
{
	double **Parameters;	//��� ���������
	long *Answers;			//��� ������ (-1, ���� ����� �� ���������)
};

class CAlgoDBLearning {
public:
	virtual char* GetErrorString(long e)=0; //���������� �������� ������
	virtual long Init(long NumOfEntries, long NumOfAnswers)=0; //������������� ���������� ������ � �������
	virtual long GetInitParamsOfSystem(long *pNumOfEntries, long *pNumOfAnswers)=0;

	virtual long SetLearningSet(long NumOfLearningSamples, double* lset)=0; //��������� ���������� ������
	virtual long SetTestSet(long NumOfTestSamples, double* tset)=0; //��������� �����

	virtual long RunOptionsDialog()=0;//����� ������� ��������
	virtual long LoadOptions(void *source)=0; //��������� ��c������ 
	virtual long SaveOptions(void **dest)=0; //�������� ��c������ � *dest. ���������� ����� ���������� ������
		//���������� ���������� ���������� ������.

	virtual long RunLearningProcess()=0;	//����� ������� ��������
	virtual long RunTest(long *Answers)=0; //� *Answers ������������ ������ ��������� �������
		//���� ������� ������ �� ����������, ������, ������� ������, ��� ����� �� ���������.
		//������ ��� ������ ������ ���� �������� � ������� NumOfTestSamples*NumOfAnswers*sizeof(long);
	virtual long RunStatisticDialog()=0; //����� ������� ����������
		
	virtual long LoadSystem(void *source)=0; //��������� ������� 
	virtual long SaveSystem(void **dest)=0;//�������� ���� ������� � *dest. ���������� ����� ���������� ������
		//���������� ���������� ���������� ������.

	virtual long FreeDataBlock(void *data)=0; //���������� ��, ��� ������� SaveOptions ��� SaveSystem.
};

class CAlgoDBInterface
{
public:
	virtual char * GetErrorString(long e)=0;			//���������� �������� ������
	virtual bool CheckIDCompatibility(long id)=0;				// �������� �� ������������� ���������� � ����� ���� ������
	virtual long InitAlgoGroups(AlgoDBParametersGroups* pg, AlgoDBAnswerGroups *ag,
		AlgoDBInfoColumns *ac)=0; // ��������� ������ �������� ����� � ����������
	virtual long DeleteAlgoGroups(AlgoDBParametersGroups* pg, AlgoDBAnswerGroups* ag,
		AlgoDBInfoColumns *ac)=0; // ������������ ������ �� ������	
	
	virtual long GetParametersAndAnswers(const AlgoDBFileRecord* record, AlgoFullInfoAboutRecord* info)=0; //��������� ���� ����������
	virtual long DeleteParametersAndAnswers(AlgoFullInfoAboutRecord* info)=0;

	virtual long DrawInfoItem(const AlgoDBFileRecord* record, HDC hdc, long InfoIndex, RECT* rect=NULL) = 0;// ���������� ������� InfoIndex 
						// ���������� 0, ���� �����, ����� - ��� ������. ���� rect=NULL, �� �������� ��������� � 0,0 � �������� 1:1
						// ���� hdc==NULL, �� � rect ������������ ������� ��������.
	virtual long GetInfoIntemText(const AlgoDBFileRecord* record, long InfoIndex, WCHAR **OutTxt)=0; //������ ���������� ����������

	virtual long GetLearningSystems(AlgoLearningSystems* ls)=0; // ���������� ���������� �� ��������� ��������
	virtual long DeleteLearningSystemsStruct(AlgoLearningSystems* ls)=0; // ������� ���������� �� ��������� ��������
	virtual long CreateLearningSystem(long SystemIndex, CAlgoDBLearning **pSystem)=0; //������� ��������� �������
	virtual long DestroyLearningSystem(CAlgoDBLearning* pSystem)=0; //����� ������������ �������
};
int CreateDBInterface(CAlgoDBInterface** pInterface);
int CreateDBInterfaceID(CAlgoDBInterface** pInterface,long ID);
int DestroyDBInterface(CAlgoDBInterface* pInterface);



#endif // !defined(AFX_ALGODBINTERFACE_H__22BF78E5_7AED_4255_B74A_18408A05475D__INCLUDED_)

