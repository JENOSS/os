/**
 *  file    InterruptHandler.c
 *  date    2009/01/24
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   인터럽트 및 예외 핸들러에 관련된 소스 파일
 */

#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"
#include "Console.h"
#include "AssemblyUtility.h"
/**
 *  공통으로 사용하는 예외 핸들러
 */
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
    char vcBuffer[ 3 ] = { 0, };

    // 인터럽트 벡터를 화면 오른쪽 위에 2자리 정수로 출력
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
 *  공통으로 사용하는 인터럽트 핸들러
 */
void kCommonInterruptHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iCommonInterruptCount = 0;

    //=========================================================================
    // 인터럽트가 발생했음을 알리려고 메시지를 출력하는 부분
    // 인터럽트 벡터를 화면 오른쪽 위에 2자리 정수로 출력
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // 발생한 횟수 출력
    vcBuffer[ 8 ] = '0' + g_iCommonInterruptCount;
    g_iCommonInterruptCount = ( g_iCommonInterruptCount + 1 ) % 10;
    kPrintStringXY( 70, 0, vcBuffer );
    //=========================================================================
    
    // EOI 전송
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

/**
 *  키보드 인터럽트의 핸들러
 */
void kKeyboardHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iKeyboardInterruptCount = 0;
    BYTE bTemp;

    //=========================================================================
    // 인터럽트가 발생했음을 알리려고 메시지를 출력하는 부분
    // 인터럽트 벡터를 화면 왼쪽 위에 2자리 정수로 출력
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // 발생한 횟수 출력
    vcBuffer[ 8 ] = '0' + g_iKeyboardInterruptCount;
    g_iKeyboardInterruptCount = ( g_iKeyboardInterruptCount + 1 ) % 10;
    kPrintStringXY( 0, 0, vcBuffer );
    //=========================================================================

    // 키보드 컨트롤러에서 데이터를 읽어서 ASCII로 변환하여 큐에 삽입
    if( kIsOutputBufferFull() == TRUE )
    {
        bTemp = kGetKeyboardScanCode();
        kConvertScanCodeAndPutQueue( bTemp );
    }

    // EOI 전송
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

static inline void invlpg(void* m)
{
	asm volatile ( "invlpg (%0)" : : "b"(m) : "memory" );
}
