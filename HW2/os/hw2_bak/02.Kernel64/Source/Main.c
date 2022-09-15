/**
 *  file    Main.c
 *  date    2009/01/02
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ���� �ۼ��� Ŀ���� ��Ʈ�� ����Ʈ ����
 */

#include "Types.h"

// �Լ� ����
void kprintString( int iX, int iY, const char* pcString );
void kprint_2 ( int x, int y, const char* p);
BOOL isWrite( DWORD* pdwCurrentAddress);
BOOL isRead( DWORD* pdwCurrentAddress);
/**
 *  �Ʒ� �Լ��� C ��� Ŀ���� ���� �κ���
 */
void Main( void )
{
    kprintString( 0, 10, "Switch To IA-32e Mode Success~!!" );
    kprintString( 0, 11, "IA-32e C Language Kernel Start..............[Pass]" );
    kprint_2 (0, 12, "This message is printed through the video memory relocated to 0xAB8000"); 
    
   if( isRead( (DWORD*) 0x1fe000) )
	kprint_2(0,13, "Read from 0x1fe000 [Ok]");
   if( isWrite( (DWORD*) 0x1fe000) )
	kprint_2(0,14, "Write to 0x1fe000 [Ok]");
   if(  isRead( (DWORD*) 0x1ff000) )
	kprint_2(0,15, "Read from 0x1ff000 [0k]");
   // isWrite( (DWORD*) 0x1ff000); //boomb
    
}

/**
 *  ���ڿ��� X, Y ��ġ�� ���
 */
void kprintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y ��ǥ�� �̿��ؼ� ���ڿ��� ����� ��巹���� ���
    pstScreen += ( iY * 80 ) + iX;

    // NULL�� ���� ������ ���ڿ� ���
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}
	
void kprint_2 ( int x, int y, const char* p)
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xAB8000;
    int i;
    
    // X, Y ��ǥ�� �̿��ؼ� ���ڿ��� ����� ��巹���� ���
    pstScreen += ( y * 80 ) + x;

    // NULL�� ���� ������ ���ڿ� ���
    for( i = 0 ; p[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = p[ i ];
    }
}

BOOL isWrite(DWORD* pdwCurrentAddress)
{
	*pdwCurrentAddress = 20;
	
	if( *pdwCurrentAddress == 20 )
		return TRUE;
	return FALSE;
}

BOOL isRead(DWORD* pdwCurrentAddress)
{
	if( *pdwCurrentAddress == 10){
		
	}
	else if( *pdwCurrentAddress != 10){
		return TRUE;
	}
	// anyway~! it read! pdwCurrentAddress
	// 
}
