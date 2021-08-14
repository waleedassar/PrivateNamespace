// SimplePrivateNamespace.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "windows.h"
#include "stdio.h"

#include "Header.h"



PSID GetCurrentUserSID(ulong* pSidSize)
{


	SetLastError(0);

	if(pSidSize) *pSidSize = 0;

	DWORD cchDomainName = 0x100;

	WCHAR * wszDomainName = (wchar_t*)LocalAlloc(LMEM_ZEROINIT,cchDomainName);
	if(!wszDomainName) return 0;
	


	ulong SidSize = 0x10000;
	_SID* pSid = (_SID*)VirtualAlloc(0,SidSize,MEM_COMMIT,PAGE_READWRITE);
	if(pSid)
	{
		SID_NAME_USE UseX;

		if( LookupAccountName(0,L"walied",pSid,&SidSize,wszDomainName,&cchDomainName,&UseX) )
		{
			LocalFree(wszDomainName);
			if(pSidSize) *pSidSize = SidSize;
			return pSid;
		}
		else
		{
			printf("Error getting current user's sid.\r\n");
			VirtualFree(pSid,0,MEM_RELEASE);
		}
	}
	LocalFree(wszDomainName);
	return 0;
}



//Minimum working example
int _tmain_min(int argc, _TCHAR* argv[])
{
	_BOUNDARY_DESCRIPTOR_INPUT_BASIC BasicX = {0};
	BasicX.Revision = 1;
	BasicX.Size = 0x10;
	BasicX.NumberOfEntries = 0;

	HANDLE hName = 0;

	int ret = NtCreatePrivateNamespace(&hName,GENERIC_READ,0,&BasicX);
	wprintf(L"NtCreatePrivateNamespace, ret: %X, hName: %X\r\n",ret,hName);
	Sleep(-1);
	return 0;
}



uchar* FillBasic(_BOUNDARY_DESCRIPTOR_INPUT_BASIC* pBoundBasic)
{
	if(!pBoundBasic) return 0;

	pBoundBasic->Revision=1;
	pBoundBasic->NumberOfEntries=0;
	pBoundBasic->Size= sizeof(_BOUNDARY_DESCRIPTOR_INPUT_BASIC); //to be updated
	pBoundBasic->UNKZ = 0;

	return (((uchar*)pBoundBasic) + sizeof(_BOUNDARY_DESCRIPTOR_INPUT_BASIC));
}

uchar* AddNameTLV(_BOUNDARY_DESCRIPTOR_INPUT_BASIC* pBoundBasic,
				_BOUNDARY_DESCRIPTOR_INPUT_TLV* pTLV,
				wchar_t* NameSpaceName)
{
	if(!pBoundBasic) return 0;
	if(!pTLV) return 0;
	if(!NameSpaceName) return 0;
	//----------------
	ulonglong DiffX = 0;
	if( ((ulonglong)pTLV) & 0x7)
	{
		//printf("Realigning\r\n");
		
		ulonglong xTLV = 
		((((ulonglong)pTLV) + 7) & 0xFFFFFFFFFFFFFFF8);
		
		DiffX = xTLV - ((ulonglong)pTLV);

		pTLV = (_BOUNDARY_DESCRIPTOR_INPUT_TLV*)xTLV;
	}

	pTLV->Type = 1;

	ulong NameLen = wcslen(NameSpaceName) * 2;

	pTLV->Length = sizeof(_BOUNDARY_DESCRIPTOR_INPUT_TLV) + NameLen;

	uchar* pTLV_data = ((uchar*)pTLV) + sizeof(_BOUNDARY_DESCRIPTOR_INPUT_TLV);

	memcpy(pTLV_data,NameSpaceName,NameLen);

	pBoundBasic->NumberOfEntries++;

	pBoundBasic->Size += pTLV->Length;
	pBoundBasic->Size += DiffX;

	return pTLV_data+NameLen;
}

uchar* AddSidTLV(_BOUNDARY_DESCRIPTOR_INPUT_BASIC* pBoundBasic,
				_BOUNDARY_DESCRIPTOR_INPUT_TLV* pTLV,
				PSID pSid)
{
	if(!pBoundBasic) return 0;
	if(!pTLV) return 0;
	if(!pSid) return 0;
	//----------------
	ulonglong DiffX = 0;
	if( ((ulonglong)pTLV) & 0x7)
	{
		//printf("Realigning\r\n");
		
		ulonglong xTLV = 
		((((ulonglong)pTLV) + 7) & 0xFFFFFFFFFFFFFFF8);
		
		DiffX = xTLV - ((ulonglong)pTLV);

		pTLV = (_BOUNDARY_DESCRIPTOR_INPUT_TLV*)xTLV;
	}


	pTLV->Type = 2;



	ulong SidSize = GetLengthSid(pSid);
	if(!SidSize) return 0;

	pTLV->Length = sizeof(_BOUNDARY_DESCRIPTOR_INPUT_TLV) + SidSize;

	uchar* pTLV_data = ((uchar*)pTLV) + sizeof(_BOUNDARY_DESCRIPTOR_INPUT_TLV);

	memcpy(pTLV_data,pSid,SidSize);

	pBoundBasic->NumberOfEntries++;

	pBoundBasic->Size += pTLV->Length;
	pBoundBasic->Size += DiffX;

	return pTLV_data+SidSize;
}

uchar* AddObjectIdTLV(_BOUNDARY_DESCRIPTOR_INPUT_BASIC* pBoundBasic,
				_BOUNDARY_DESCRIPTOR_INPUT_TLV* pTLV,
				PSID pSid)
{
	if(!pBoundBasic) return 0;
	if(!pTLV) return 0;
	if(!pSid) return 0;
	//----------------
	ulonglong DiffX = 0;
	if( ((ulonglong)pTLV) & 0x7)
	{
		//printf("Realigning\r\n");
		
		ulonglong xTLV = 
		((((ulonglong)pTLV) + 7) & 0xFFFFFFFFFFFFFFF8);
		
		DiffX = xTLV - ((ulonglong)pTLV);

		pTLV = (_BOUNDARY_DESCRIPTOR_INPUT_TLV*)xTLV;
	}

	pTLV->Type = 3;



	ulong SidSize = GetLengthSid(pSid);
	if(!SidSize) return 0;

	pTLV->Length = sizeof(_BOUNDARY_DESCRIPTOR_INPUT_TLV) + SidSize;

	uchar* pTLV_data = ((uchar*)pTLV) + sizeof(_BOUNDARY_DESCRIPTOR_INPUT_TLV);

	memcpy(pTLV_data,pSid,SidSize);

	pBoundBasic->NumberOfEntries++;

	pBoundBasic->Size += pTLV->Length;
	pBoundBasic->Size += DiffX;

	return pTLV_data+SidSize;
}


int _tmain(int argc, _TCHAR* argv[])
{	

	//Object Directories for namespaces can't be named objects.
	_OBJECT_ATTRIBUTES ObjAttrX = {0};
	ObjAttrX.Length = sizeof(_OBJECT_ATTRIBUTES);


	_BOUNDARY_DESCRIPTOR_INPUT_BASIC* inBoundaryDescriptor = 
	(_BOUNDARY_DESCRIPTOR_INPUT_BASIC*)VirtualAlloc(0,0x10000,MEM_COMMIT,PAGE_READWRITE);



	uchar* pRunner = FillBasic(inBoundaryDescriptor);
	if(!pRunner) return -1;
	//Insert name
	wchar_t* NamespaceName = L"waliedxx";

	wchar_t NameXXX[100]=L"walied";



	pRunner = AddNameTLV(inBoundaryDescriptor,
		(_BOUNDARY_DESCRIPTOR_INPUT_TLV*)pRunner,
		NamespaceName);
	if(!pRunner) return -2;

	//Insert _SID_INFO
	ulong SidSize = 0;
	PSID pSid = GetCurrentUserSID(&SidSize);

	if(!pSid) return -3;
	if(!SidSize) return -4;


	pRunner = AddSidTLV(inBoundaryDescriptor,
		(_BOUNDARY_DESCRIPTOR_INPUT_TLV*)pRunner,
		pSid);

	if(!pRunner) return -5;
	//Insert ObjectId
	pRunner = AddObjectIdTLV(inBoundaryDescriptor,
		(_BOUNDARY_DESCRIPTOR_INPUT_TLV*)pRunner,
		pSid);

	if(!pRunner) return -5;
	//-------- END ----------
	
	HANDLE hName = 0;
	int ret = NtCreatePrivateNamespace(&hName,GENERIC_ALL,
									&ObjAttrX,inBoundaryDescriptor);
	wprintf(L"NtCreatePrivateNamespace, ret: %X, hName: %X\r\n",ret,hName);


	if(ret >= 0)
	{
		HANDLE hNameZ = 0;
		int retValue = NtOpenPrivateNamespace(&hNameZ,
												GENERIC_ALL,
												&ObjAttrX,
												inBoundaryDescriptor);

		wprintf(L"NtOpenPrivateNamespace, ret: %X, hName: %X\r\n",retValue,hNameZ);


		if(retValue >= 0)
		{
			ZwClose(hNameZ);
		}
	}


	if(pSid) VirtualFree(pSid,0,MEM_RELEASE);


	if(inBoundaryDescriptor)
	{
		VirtualFree(inBoundaryDescriptor,0,MEM_RELEASE);
	}

	ret = ZwDeletePrivateNamespace(hName);
	wprintf(L"ZwDeletePrivateNamespace, ret: %X\r\n",ret);


	ZwClose(hName);
	
	Sleep(-1);
	return 0;
}
