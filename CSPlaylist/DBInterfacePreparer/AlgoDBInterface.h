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
struct AlgoDBFileHeader // Заголовок входного файла
{
	char label[4];		// всегда равен "ALGO"
	long id;			// идентификатор формата базы данных
	long HeaderLen;		// длина всего заголовка файла
	long NumOfRecords;	// число записей
	long UserFlags;		// дополнительная информация для разработчика
	char Description[1];// Комментарии и описания к содержимому файла; длина описания может быть произвольной
};
#endif ///STRUCT_AlgoDBFileHeader

struct AlgoDBFileRecord // Запись
{
	long LenOfRecord;	// Длинна записи
	BYTE DataBlock[1];	// Блок данных произвольной длинны
};

struct AlgoDBParametersGroups      //Группы входных параметров
{
	long NumOfGroups;		//Количество групп
	WCHAR **NameOfGroup;		//Названия групп
	long *NumOfParameters;	//Количество парамеров в каждой группе
	WCHAR ***NameOfParameter;//Названия параметров
};

struct AlgoDBAnswerGroups      //Группы ответов
{
	long NumOfGroups;		//Количество групп
	WCHAR **NameOfGroup;		//Названия групп
	long *NumOfAnswers;		//Количество ответов в каждой группе
	WCHAR ***NameOfAnswers;	//Названия ответов
};
struct AlgoDBInfoColumns	//Название колонок с информацией для определения объекта (картинки, силуэты)
{
	long NumOfColumns;		//Количество столбцов
	WCHAR **NameOfColumns;	//Названия столбцов	
	long *TypeOfColumns;	//Тип информации: 1-Picture, 2 - Text
};

struct AlgoLearningSystems	//Обучающие системы
{
	long NumOfSystems;		//Количество систем
	WCHAR **NameOfSystems;	//Названия систем		
};

struct AlgoFullInfoAboutRecord //Структура параметров и ответов для записи
{
	double **Parameters;	//Все параметры
	long *Answers;			//Все ответы (-1, если ответ не определен)
};

class CAlgoDBLearning {
public:
	virtual char* GetErrorString(long e)=0; //Возвращает описание ошибки
	virtual long Init(long NumOfEntries, long NumOfAnswers)=0; //Инициализация количества входов и выходов
	virtual long GetInitParamsOfSystem(long *pNumOfEntries, long *pNumOfAnswers)=0;

	virtual long SetLearningSet(long NumOfLearningSamples, double* lset)=0; //Установка обучающего набора
	virtual long SetTestSet(long NumOfTestSamples, double* tset)=0; //Установка теста

	virtual long RunOptionsDialog()=0;//Вызов диалога настроек
	virtual long LoadOptions(void *source)=0; //Загрузить наcтройки 
	virtual long SaveOptions(void **dest)=0; //Записать наcтройки в *dest. Необходимо потом освободить память
		//Возвращает количество записанных байтов.

	virtual long RunLearningProcess()=0;	//Вызов диалога обучения
	virtual long RunTest(long *Answers)=0; //В *Answers записываются ответы обучающей системы
		//Если индекса ответа не существует, значит, система решила, что ответ не определен.
		//Память под ответы должна быть выделена в размере NumOfTestSamples*NumOfAnswers*sizeof(long);
	virtual long RunStatisticDialog()=0; //Вызов диалога статистики
		
	virtual long LoadSystem(void *source)=0; //Загрузить систему 
	virtual long SaveSystem(void **dest)=0;//Записать саму систему в *dest. Необходимо потом освободить память
		//Возвращает количество записанных байтов.

	virtual long FreeDataBlock(void *data)=0; //Уничтожить то, что создали SaveOptions или SaveSystem.
};

class CAlgoDBInterface
{
public:
	virtual char * GetErrorString(long e)=0;			//Возвращает описание ошибки
	virtual bool CheckIDCompatibility(long id)=0;				// Проверка на совместимость интерфейса и файла базы данных
	virtual long InitAlgoGroups(AlgoDBParametersGroups* pg, AlgoDBAnswerGroups *ag,
		AlgoDBInfoColumns *ac)=0; // Получение списка назавний групп и параметров
	virtual long DeleteAlgoGroups(AlgoDBParametersGroups* pg, AlgoDBAnswerGroups* ag,
		AlgoDBInfoColumns *ac)=0; // Освобождение памяти от списка	
	
	virtual long GetParametersAndAnswers(const AlgoDBFileRecord* record, AlgoFullInfoAboutRecord* info)=0; //получение всех параметров
	virtual long DeleteParametersAndAnswers(AlgoFullInfoAboutRecord* info)=0;

	virtual long DrawInfoItem(const AlgoDBFileRecord* record, HDC hdc, long InfoIndex, RECT* rect=NULL) = 0;// отображает колонку InfoIndex 
						// Возвращает 0, если успех, иначе - код ошибки. Если rect=NULL, то картинка выводится в 0,0 в масштабе 1:1
						// Если hdc==NULL, то в rect записываются размеры картинки.
	virtual long GetInfoIntemText(const AlgoDBFileRecord* record, long InfoIndex, WCHAR **OutTxt)=0; //Память необходимо освободить

	virtual long GetLearningSystems(AlgoLearningSystems* ls)=0; // возвращает ифнорамцию об обучающих системах
	virtual long DeleteLearningSystemsStruct(AlgoLearningSystems* ls)=0; // удаляет ифнорамцию об обучающих системах
	virtual long CreateLearningSystem(long SystemIndex, CAlgoDBLearning **pSystem)=0; //создать обучающую систему
	virtual long DestroyLearningSystem(CAlgoDBLearning* pSystem)=0; //убить обучающающую систему
};
int CreateDBInterface(CAlgoDBInterface** pInterface);
int CreateDBInterfaceID(CAlgoDBInterface** pInterface,long ID);
int DestroyDBInterface(CAlgoDBInterface* pInterface);



#endif // !defined(AFX_ALGODBINTERFACE_H__22BF78E5_7AED_4255_B74A_18408A05475D__INCLUDED_)

