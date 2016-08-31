#ifndef _PACK_H
#define _PACK_H

#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>

typedef struct tagFieldInfo
{
 ///I������F���㣬C�ַ���S�ַ�����R�������������
 char Type;
 ///��󳤶�
 int32_t Width;
 ///FʱС����λ��
 uint8_t Scale;
} FIELD_INFO;

typedef struct tagPackerField
{
///������
const char *Name;
///����Ϣ
FIELD_INFO *FieldInfo;

} PACKER_FIELD;

typedef struct tagDatasetHead
{
 ///����
 int32_t ColCount;
 ///����
 int32_t RowCount;
 ///��������ȣ���������ͷ����
 int32_t DatasetLength;
 ///����ֵ��һ���0��ʾ����
 int32_t ReturnCode;
} DATASET_HEAD;

class CPacker
{
public:
	CPacker()
	{
		m_pBuff = NULL;
		m_pCurr = NULL;
		m_pDataSetHead = NULL;
		m_CurrField = 0;
		m_CurrVal = 0;
	};
	int32_t BeginPack();
	int32_t AddField(const char *szFieldName, char cFieldType, int32_t nWidth = 255, uint8_t byScale = 0);
	int32_t AddValue(const char* szValue);
	int32_t EndPack();
	void* GetContent()
	{
		return m_pBuff;
	}
	int GetLength()
	{
		return m_BuffLen;
	}
private:
	PACKER_FIELD *m_pFields;
	DATASET_HEAD *m_pDataSetHead;
	void* m_pBuff;
	void* m_pCurr;
	int m_BuffLen;
	uint32_t m_CurrField;
	uint32_t m_CurrVal;
};

//��������
class CField
{
public:
	CField();
	~CField();
	char* GetName()
	{
		return m_szName;
	}
	int GetWidth()
	{
		return m_lpFieldInfo->Width;
	}
	char GetType()
	{
		return m_lpFieldInfo->Type;
	}
	char GetNo()
	{
		return m_No;
	}
	int SetValue(char* Name,FIELD_INFO* ptr,int No)
	{
		m_szName = Name;
		m_lpFieldInfo = ptr;
		m_No = No;
		return 0;
	}
private:
	char* m_szName;
	int m_No;
	FIELD_INFO* m_lpFieldInfo;
};
//�����е��ܺ�
class CFields
{
	//friend class CDataSet;
public:
	CFields()
	{
		m_nFieldCount = 0;
		m_nCapacity = 0;
	}
	~CFields();
	int parse(void* lpBuf,int nFieldCount,int nLength);
	CField* GetField(int colcount);
	CField* GetField(const char* fieldname);
private:
	typedef struct FName
	{
		char* FieldName;
		int No;
	}FName;
	CField* m_Field;
	int m_nFieldCount;
	int m_nCapacity;
	FName* m_lpFNameBuf;
};
//�����
class CDataItem
{
public:
	char* GetValue()
	{
		return m_value;
	}
	void SetValue(void* lpbuf,int len)
	{
		m_value = (char*)lpbuf;
		m_len = len;
	}
private:
	char* m_value;
	int m_len;
};

class CDataSet
{
	friend class CUnpacker;
public:
	int Parse(void* lpbuf,int rowcount,int colcount);
	char* GetField(const char* filename);
private:
	CFields m_Fields;
	CDataItem* m_DataItem;
};

class CUnpacker
{
public:
	CUnpacker(){}
	~CUnpacker(){}
	int Open(void* lpBuf);
	int Parse(void* lpBuf);
	char* GetField(const char* szFieldName);
private:
	CDataSet m_lpDataset;
	CDataSet* m_lpCurrDataSet;
};

#endif