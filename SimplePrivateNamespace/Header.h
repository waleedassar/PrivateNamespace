#pragma once

#define uchar unsigned char
#define ushort unsigned short
#define ulong unsigned long
#define ulonglong unsigned long long
#define ULONG unsigned long
#define ULONGLONG unsigned long long


#define OBJ_CASE_INSENSITIVE    0x00000040
#define OBJ_PROTECT_CLOSE 		0x00000001
#define OBJ_INHERIT             0x00000002
#define OBJ_AUDIT_OBJECT_CLOSE  0x00000004
#define OBJ_PERMANENT           0x00000010
#define OBJ_EXCLUSIVE           0x00000020
#define OBJ_CASE_INSENSITIVE    0x00000040
#define OBJ_OPENIF              0x00000080
#define OBJ_OPENLINK            0x00000100
#define OBJ_KERNEL_HANDLE       0x00000200
#define OBJ_FORCE_ACCESS_CHECK  0x00000400
#define OBJ_VALID_ATTRIBUTES    0x000007F2
#define OBJ_CREATOR_INFO_MINE   0x00010000

struct _UNICODE_STRING
{
	unsigned short Length;
	unsigned short MaxLength;
	unsigned long Pad;
	wchar_t* Buffer;
};
typedef struct _OBJECT_ATTRIBUTES {
  ULONGLONG           Length;
  HANDLE          RootDirectory;
  _UNICODE_STRING* ObjectName;
  ULONGLONG           Attributes;
  PVOID           SecurityDescriptor;
  PVOID           SecurityQualityOfService;
} OBJECT_ATTRIBUTES;


extern "C"
{
	int ZwDeletePrivateNamespace(HANDLE NamespaceHandle); 
	int NtDeletePrivateNamespace(HANDLE NamespaceHandle); 

	int ZwCreatePrivateNamespace(HANDLE* NamespaceHandle,ACCESS_MASK DesiredAccess,_OBJECT_ATTRIBUTES* ObjectAttributes,void* BoundaryDescriptor);
	int NtCreatePrivateNamespace(HANDLE* NamespaceHandle,ACCESS_MASK DesiredAccess,_OBJECT_ATTRIBUTES* ObjectAttributes,void* BoundaryDescriptor);

	int NtOpenPrivateNamespace(HANDLE* NamespaceHandle,ACCESS_MASK DesiredAccess,_OBJECT_ATTRIBUTES* ObjectAttributes,void* BoundaryDescriptor);
	int ZwOpenPrivateNamespace(HANDLE* NamespaceHandle,ACCESS_MASK DesiredAccess,_OBJECT_ATTRIBUTES* ObjectAttributes,void* BoundaryDescriptor);

	int ZwClose(HANDLE hObject);
}


struct _BOUNDARY_DESCRIPTOR_INPUT_TLV
{
	ulong Type;
	ulong Length;
	//uchar data[Length]
};



struct _BOUNDARY_DESCRIPTOR_INPUT_BASIC
{
	ulong Revision;//at 0x0, must be 1
	ulong NumberOfEntries; //at 0x4
	ulong Size; //At 0x8
	ulong UNKZ;//at 0x0C
	//Optional members here TLVs
};