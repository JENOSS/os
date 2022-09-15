/**
 *  file    Types.h
 *  date    2008/12/14
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   Ŀ�ο��� ����ϴ� ���� Ÿ���� ������ ����
 */

#ifndef __TYPES_H__
#define __TYPES_H__

////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
#define BYTE    unsigned char
#define WORD    unsigned short
#define DWORD   unsigned int
#define QWORD   unsigned long
#define BOOL    unsigned char


#define TRUE    1
#define FALSE   0
#define NULL    0


////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )

// ���� ��� �� �ؽ�Ʈ ��� ȭ���� �����ϴ� �ڷᱸ��
typedef struct kCharactorStruct
{
    BYTE bCharactor;
    BYTE bAttribute;
} CHARACTER;

#pragma pack( pop )
typedef struct uint64_t{
     DWORD first;
     DWORD second;
} uint64_t;

typedef struct memoryMapEntry{
    uint64_t base;
    uint64_t length;   
    DWORD type;
    DWORD acpi_null; 
    } memoryMapEntry;



#endif /*__TYPES_H__*/
