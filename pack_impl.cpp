#include "pack.h"
int elf_hash(const char* name,int mod)
{
	const char* p = name;
	long result = 0;
	while(*p)
	{
		result = result*31 + *p++;
	}
	return result % mod ;
}
int32_t CPacker::BeginPack()
{
	m_pFields = (PACKER_FIELD*)malloc(sizeof(PACKER_FIELD)*10);
	m_pBuff = (void*)malloc(1024);
	memset(m_pBuff,0,1024);
	m_pDataSetHead = (DATASET_HEAD*)m_pBuff;
	m_pCurr = (char*)m_pBuff + sizeof(DATASET_HEAD) + 1;

	return 1;
}

int32_t CPacker::AddField(const char *szFieldName, char cFieldType, int32_t nWidth, uint8_t byScale)
{
	//拷贝列名
	int len = strlen(szFieldName);
	m_pFields[m_CurrField].Name = (const char*)m_pCurr;
	strncpy((char*)m_pCurr,szFieldName,len);
	*((char*)m_pCurr+len) = '\0';
	m_pCurr = (char*)m_pCurr + len + 1;
	//拷贝列信息
	FIELD_INFO tmpField;
	tmpField.Type = cFieldType;
	tmpField.Width = nWidth;
	tmpField.Scale = byScale;
	m_pFields[m_CurrField].FieldInfo = (FIELD_INFO*)m_pCurr;
	memcpy(m_pCurr,(const void*)&tmpField,sizeof(FIELD_INFO));
	m_pCurr = (char*)m_pCurr + sizeof(FIELD_INFO) + 1;
	m_CurrField++;
	//设置结果集头部信息
	m_pDataSetHead->ColCount++;
	m_pDataSetHead->DatasetLength += len+sizeof(FIELD_INFO)+ 2;
	return 1;
}

int32_t CPacker::AddValue(const char* szValue)
{
	if(m_pDataSetHead->RowCount == 0)
	{
		m_pDataSetHead->RowCount = 1;
	}
	if(m_CurrVal > m_pDataSetHead->ColCount)
	{
		m_CurrVal = 0;
		m_pDataSetHead->RowCount++;
	}	
	int width = m_pFields[m_CurrVal].FieldInfo->Width;
	strncpy((char*)m_pCurr,szValue,width);
	*((char*)m_pCurr+width) = '\0';
	m_pCurr = (char*)m_pCurr + width + 1;
	m_pDataSetHead->DatasetLength += width +1;
	m_CurrVal ++;
	return 1;
}

int32_t CPacker::EndPack()
{
	m_BuffLen = (char*)m_pCurr - (char*)m_pBuff;
	return 1;
}

int CFields::parse(void* lpBuf,int nFieldCount,int nLength)
{
	int count=0;
	m_nFieldCount = nFieldCount;
	if(m_nCapacity < nFieldCount)
	{
		m_lpFNameBuf = (FName*)malloc(2*nFieldCount*sizeof(FName));
		m_Field = (CField*)malloc(nFieldCount*sizeof(CField));
		m_nCapacity = (10+nFieldCount);
	}
	for(int i=0;i < 2*nFieldCount;i++)
	{
		m_lpFNameBuf[i].FieldName = NULL;
		m_lpFNameBuf[i].No = -1;
	}
	char* p = (char*)lpBuf;
	char* pend = (char*)lpBuf + nLength;
	for(count=0;count <nFieldCount;count++)
	{
		FName* tmpName = &m_lpFNameBuf[elf_hash(p,nFieldCount)];
		while(tmpName->No != -1)
			tmpName++;
		tmpName->FieldName = p;
		tmpName->No = count;
		while(*p != '\0') 
		{
			p++;
		}
		p++;
		m_Field[count].SetValue(m_lpFNameBuf[count].FieldName,(FIELD_INFO*)p,count);

		p = p + sizeof(FIELD_INFO) + 1;
	}
	return p-(char*)lpBuf;
}

CField* CFields::GetField(int colcount)
{
	return &m_Field[colcount];
}
CField* CFields::GetField(const char* fieldname)
{
	int no = elf_hash(fieldname,m_nFieldCount);
	while(strcmp(m_lpFNameBuf[no].FieldName,fieldname) != 0)
		no++; 
	return GetField(m_lpFNameBuf[no].No);
}

int CDataSet::Parse(void* lpbuf,int rowcount,int colcount)
{
	m_DataItem = (CDataItem*)malloc(sizeof(CDataItem)*rowcount*colcount);
	char* p = (char*)lpbuf;
	for(int i=0;i<rowcount;i++)
	{
		for(int j=0;j<colcount;j++)
		{
			int len = m_Fields.GetField(i*colcount+j)->GetWidth();
			m_DataItem[i*colcount+j].SetValue(p,len);
			p += len + 1;
		}
	}
	return 0;
}

char* CDataSet::GetField(const char* filename)
{
	int colno = m_Fields.GetField(filename)->GetNo();
	return m_DataItem[colno].GetValue();
}

int CUnpacker:: Open(void* lpBuf)
{
	DATASET_HEAD* lpDataSetHead = (DATASET_HEAD*)lpBuf;
	char* curr = (char*)lpBuf;
	int FieldSize = m_lpDataset.m_Fields.parse(curr+sizeof(DATASET_HEAD)+ 1,lpDataSetHead->ColCount,lpDataSetHead->DatasetLength);
	curr = curr + sizeof(DATASET_HEAD) + 1 + FieldSize;
	m_lpDataset.Parse(curr,lpDataSetHead->RowCount,lpDataSetHead->ColCount);
	return 0;
}

char* CUnpacker::GetField(const char* szFieldName)
{
	int i;
	return m_lpDataset.GetField(szFieldName);
}