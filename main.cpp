#include "pack.h"

int main()
{
	CPacker *lptest = new CPacker();
	CUnpacker *unpacker = new CUnpacker();
	lptest->BeginPack();
	lptest->AddField("id",'S',10,0);
	lptest->AddField("name",'S',10,0);
	lptest->AddField("sex",'S',10,0);
	lptest->AddField("literature",'S',10,0);
	lptest->AddField("science",'S',10,0);
	lptest->AddValue("10575");
	lptest->AddValue("xuxinpeng");
	lptest->AddValue("male");
	lptest->AddValue("98");
	lptest->AddValue("100");
	unpacker->Open(lptest->GetContent());
	printf("[%s]",unpacker->GetField("literature"));
	printf("[%s]",unpacker->GetField("sex"));
	printf("[%s]",unpacker->GetField("name"));
	printf("[%s]",unpacker->GetField("id"));
	printf("[%s]",unpacker->GetField("name"));
	return 1;
}