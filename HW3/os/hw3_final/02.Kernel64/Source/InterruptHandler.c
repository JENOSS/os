/**
 *  file    InterruptHandler.c
 *  date    2009/01/24
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ���ͷ�Ʈ �� ���� �ڵ鷯�� ���õ� �ҽ� ����
 */

#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"
#include "Console.h"
#include "AssemblyUtility.h"
/**
 *  �������� ����ϴ� ���� �ڵ鷯
 */
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
    char vcBuffer[ 3 ] = { 0, };

    // ���ͷ�Ʈ ���͸� ȭ�� ������ ���� 2�ڸ� ������ ���
    vcBuffer[ 0 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 1 ] = '0' + iVectorNumber % 10;
    
    kPrintf( "====================================================\n" );
    kPrintf( "                 Exception Occur~!!!!               \n" );
    kPrintf( "                    Vector: " );
    kPrintf("%s \n", vcBuffer );
    kPrintf("====================================================\n" );


     /* check errorcode print
    char a[65]={0,};
    for(int i=0;i<63;i++){
        a[i] = '0' + (qwErrorCode & 1 );
	qwErrorCode = qwErrorCode>>1;
    }
    kPrintf( "%s", a );
    */
    while( 1 ) ;
}

void kPageExceptionHandler( QWORD *fault_addr , QWORD qwErrorCode )
{
    int p_bit = qwErrorCode & 1;
   
    kPrintf( "====================================================\n" );
    if(p_bit == 0)
   	 kPrintf( "                   Page Fault ");
    else if (p_bit == 1)
    	 kPrintf( "                   Protection Falut ");
    kPrintf( " Occur~!!!!               \n" );
    kPrintf( "                   Address: 0x%x \n", fault_addr);
    kPrintf("====================================================\n" );
  
    // find the address to generate page_fault or protection_fault 
     /* if(err_addr == (QWORD *)0x1ff000)
		kPrintf("%x", err_addr);*/

    //exception processing    
    if (p_bit==0 && fault_addr==(QWORD*)0x1ff000 ) {
	// page_fault clear
	long *ptr = (long*)0x142ff8; // this address has 0x1ff000 and Page Table Entry flags
	long *temp = ptr;
    	*ptr= *temp | 0x1; // present bit add
	invlpg(fault_addr);
    }
    else if (p_bit==1 && fault_addr==(QWORD*)0x1ff000 ) {
	// protection fault clear
	long *ptr = (long*)0x142ff8; // this address has 0x1ff000 and Page Table Entry flags 
        long *temp = ptr;
	*ptr = *temp | 0x2; // R/W bit add
	invlpg(fault_addr);
    }
 
}
/**
 *  �������� ����ϴ� ���ͷ�Ʈ �ڵ鷯
 */
void kCommonInterruptHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iCommonInterruptCount = 0;

    //=========================================================================
    // ���ͷ�Ʈ�� �߻������� �˸����� �޽����� ����ϴ� �κ�
    // ���ͷ�Ʈ ���͸� ȭ�� ������ ���� 2�ڸ� ������ ���
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // �߻��� Ƚ�� ���
    vcBuffer[ 8 ] = '0' + g_iCommonInterruptCount;
    g_iCommonInterruptCount = ( g_iCommonInterruptCount + 1 ) % 10;
    kPrintStringXY( 70, 0, vcBuffer );
    //=========================================================================
    
    // EOI ����
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

/**
 *  Ű���� ���ͷ�Ʈ�� �ڵ鷯
 */
void kKeyboardHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iKeyboardInterruptCount = 0;
    BYTE bTemp;

    //=========================================================================
    // ���ͷ�Ʈ�� �߻������� �˸����� �޽����� ����ϴ� �κ�
    // ���ͷ�Ʈ ���͸� ȭ�� ���� ���� 2�ڸ� ������ ���
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // �߻��� Ƚ�� ���
    vcBuffer[ 8 ] = '0' + g_iKeyboardInterruptCount;
    g_iKeyboardInterruptCount = ( g_iKeyboardInterruptCount + 1 ) % 10;
    kPrintStringXY( 0, 0, vcBuffer );
    //=========================================================================

    // Ű���� ��Ʈ�ѷ����� �����͸� �о ASCII�� ��ȯ�Ͽ� ť�� ����
    if( kIsOutputBufferFull() == TRUE )
    {
        bTemp = kGetKeyboardScanCode();
        kConvertScanCodeAndPutQueue( bTemp );
    }

    // EOI ����
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

static inline void invlpg(void* m)
{
	asm volatile ( "invlpg (%0)" : : "b"(m) : "memory" );
}
