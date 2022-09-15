/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   콘솔 셸에 관련된 소스 파일
 */

#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"

// 커맨드 테이블 정의
SHELLCOMMANDENTRY gs_vstCommandTable[] =
{
        { "help", "Show Help", kHelp },
        { "cls", "Clear Screen", kCls },
        { "totalram", "Show Total RAM Size", kShowTotalRAMSize },
        { "strtod", "String To Decial/Hex Convert", kStringToDecimalHexTest },
        { "strtdown", "Shutdown And Reboot OS", kShutdown },
	{ "raisefault", "raise protection or page fault", kRaisefault },
        { "strtdcrt", "dummy", kdummy },
        { "strtdoy", "dummy",  kdummy },
	{ "dummy", "dummy" , kdummy },
};                                     

//==============================================================================
//  실제 셸을 구성하는 코드
//==============================================================================
/**
 *  셸의 메인 루프
 */
void kStartConsoleShell( void )
{
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;
    
	int i =0;
	int historyCommandLength = 0;
	QUEUE	pQueue;
   	char queueBuffer[200] = {0, };
	char queueDelete[20];
	int qSize = 10;
	int dataSize = 20;
	kInitializeQueue(&pQueue, queueBuffer, qSize, dataSize);
	int qIndex = 0;
	int historyIndexingRound = 0;
	int iPutIndexMinusOne = 0;
	int stop = 0;
	
    // 프롬프트 출력
    kPrintf( CONSOLESHELL_PROMPTMESSAGE );
   
    kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
    iCommandBufferIndex = 0;
	
    while( 1 )
    {
        // 키가 수신될 때까지 대기
       
        bKey = kGetCh();
	 // Backspace 키 처리
        if( bKey == KEY_BACKSPACE )
        {
            if( iCommandBufferIndex > 0 )
            {
                // 현재 커서 위치를 얻어서 한 문자 앞으로 이동한 다음 공백을 출력하고 
                // 커맨드 버퍼에서 마지막 문자 삭제
                kGetCursor( &iCursorX, &iCursorY );
		if(iCursorX>7)	{
               		kPrintStringXY( iCursorX - 1, iCursorY, " " );
               		kSetCursor( iCursorX - 1, iCursorY );
               		iCommandBufferIndex--;	
			vcCommandBuffer[ iCommandBufferIndex ] = ' ';
		}	
            }
        }
        // 엔터 키 처리
        else if( bKey == KEY_ENTER )
        {
            kPrintf( "\n" );
	    historyIndexingRound = 0;            
            
	    if( iCommandBufferIndex > 0 )
            {
                // 커맨드 버퍼에 있는 명령을 실행
                vcCommandBuffer[ iCommandBufferIndex ] = '\0';
                kExecuteCommand( vcCommandBuffer );
		
		if(kIsQueueFull(&pQueue) == TRUE)
		{
			kGetQueue(&pQueue, queueDelete);
		}
	
		kPutQueue(&pQueue, vcCommandBuffer);
		qIndex = pQueue.iPutIndex;
		iPutIndexMinusOne = qIndex;
		kSubtractqIndex(&iPutIndexMinusOne, qSize);
            }
            
            // 프롬프트 출력 및 커맨드 버퍼 초기화
            kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE );            
            kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        // 시프트 키, CAPS Lock, NUM Lock, Scroll Lock은 무시
        else if( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) )
        {
            ;
        }
	// key up
	else if (bKey == KEY_UP)
	{
		kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
		kGetCursor(&iCursorX, &iCursorY);
		for( i = iCommandBufferIndex; i>0; i--)
		{
			kPrintStringXY( iCommandBufferIndex+6, iCursorY, " ");	
			iCommandBufferIndex--;
		}
		kSetCursor( 7, iCursorY );

		if(kIsQueueFull(&pQueue))
		{
			if(historyIndexingRound==0 || qIndex != pQueue.iGetIndex)
			{
				kSubtractqIndex(&qIndex, qSize);
				historyIndexingRound = 1;
			}
		}
		else
		{
			if(qIndex != pQueue.iGetIndex)
			{
				kSubtractqIndex(&qIndex, qSize);
			}
		}

		historyCommandLength = kPrintQueue(&pQueue, vcCommandBuffer, qIndex);
		iCommandBufferIndex += historyCommandLength;
	}
	// key down
	else if (bKey == KEY_DOWN)
	{
		kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
		kGetCursor(&iCursorX, &iCursorY);
		for( i = iCommandBufferIndex; i>0; i-- )
		{
			kPrintStringXY( iCommandBufferIndex+6, iCursorY, " ");
			iCommandBufferIndex--;
		}
		kSetCursor( 7, iCursorY );
	
		if(kIsQueueFull(&pQueue))
		{
			if(qIndex == pQueue.iPutIndex && historyIndexingRound==0)
			{
				stop = 1;
			}
			else if(qIndex == iPutIndexMinusOne)
			{
				historyIndexingRound = 0;
				kAddqIndex(&qIndex, qSize);
			
				stop = 1;
			}
			else
			{
				kAddqIndex(&qIndex, qSize);
			}
		}
		else
		{
			if(qIndex == iPutIndexMinusOne)
			{
				kAddqIndex(&qIndex, qSize);
				stop = 1;
			}
			else if(qIndex == pQueue.iPutIndex)
			{
				stop = 1;
			}
			else
			{
				kAddqIndex(&qIndex, qSize);
			}
		}
		
		if(stop == 0)
		{
			historyCommandLength = kPrintQueue(&pQueue, vcCommandBuffer, qIndex);
			iCommandBufferIndex += historyCommandLength;
		}
		stop = 0;

	}

	// 'TAB' call TABfunc
	else if( bKey == KEY_TAB )
	{
		if( iCommandBufferIndex > 0 &&
			 iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
		{
			 iCommandBufferIndex = kTabfunc( vcCommandBuffer );
		}
       
	}
        else
        {
            // 버퍼에 공간이 남아있을 때만 가능
            if( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
            {
                vcCommandBuffer[ iCommandBufferIndex++ ] = bKey;
                kPrintf( "%c", bKey );
            }
        }
    }
}

/*
 *  커맨드 버퍼에 있는 커맨드를 비교하여 해당 커맨드를 처리하는 함수를 수행
 */
void kExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;
    
    // 공백으로 구분된 커맨드를 추출
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }
    
    // 커맨드 테이블을 검사해서 동일한 이름의 커맨드가 있는지 확인
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // 커맨드의 길이와 내용이 완전히 일치하는지 검사
        if( ( iCommandLength == iSpaceIndex ) &&
            ( kMemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // 리스트에서 찾을 수 없다면 에러 출력
    if( i >= iCount )
    {
        kPrintf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  파라미터 자료구조를 초기화
 */
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  공백으로 구분된 파라미터의 내용과 길이를 반환
 */
int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // 더 이상 파라미터가 없으면 나감
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }
    
    // 버퍼의 길이만큼 이동하면서 공백을 검색
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }
    
    // 파라미터를 복사하고 길이를 반환
    kMemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // 파라미터의 위치 업데이트
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
//==============================================================================
//  커맨드를 처리하는 코드
//==============================================================================
/**
 *  셸 도움말을 출력
 */
void kHelp( const char* pcCommandBuffer )
{
    int i;
    int iCount;
    int iCursorX, iCursorY;
    int iLength, iMaxCommandLength = 0;
    
    
    kPrintf( "=========================================================\n" );
    kPrintf( "                    MINT64 Shell Help                    \n" );
    kPrintf( "=========================================================\n" );
    
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );

    // 가장 긴 커맨드의 길이를 계산
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }
    
    // 도움말 출력
    for( i = 0 ; i < iCount ; i++ )
    {
        kPrintf( "%s", gs_vstCommandTable[ i ].pcCommand );
        kGetCursor( &iCursorX, &iCursorY );
        kSetCursor( iMaxCommandLength, iCursorY );
        kPrintf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );
    }
}

/**
 *  화면을 지움 
 */
void kCls( const char* pcParameterBuffer )
{
    // 맨 윗줄은 디버깅 용으로 사용하므로 화면을 지운 후, 라인 1로 커서 이동
    kClearScreen();
    kSetCursor( 0, 1 );
}

/**
 *  총 메모리 크기를 출력
 */
void kShowTotalRAMSize( const char* pcParameterBuffer )
{
    kPrintf( "Total RAM Size = %d MB\n", kGetTotalRAMSize() );
}

/**
 *  문자열로 된 숫자를 숫자로 변환하여 화면에 출력
 */
void kStringToDecimalHexTest( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    int iCount = 0;
    long lValue;
    
    // 파라미터 초기화
    kInitializeParameter( &stList, pcParameterBuffer );
    
    while( 1 )
    {
        // 다음 파라미터를 구함, 파라미터의 길이가 0이면 파라미터가 없는 것이므로
        // 종료
        iLength = kGetNextParameter( &stList, vcParameter );
        if( iLength == 0 )
        {
            break;
        }

        // 파라미터에 대한 정보를 출력하고 16진수인지 10진수인지 판단하여 변환한 후
        // 결과를 printf로 출력
        kPrintf( "Param %d = '%s', Length = %d, ", iCount + 1, 
                 vcParameter, iLength );

        // 0x로 시작하면 16진수, 그외는 10진수로 판단
        if( kMemCmp( vcParameter, "0x", 2 ) == 0 )
        {
            lValue = kAToI( vcParameter + 2, 16 );
            kPrintf( "HEX Value = %q\n", lValue );
        }
        else
        {
            lValue = kAToI( vcParameter, 10 );
            kPrintf( "Decimal Value = %d\n", lValue );
        }
        
        iCount++;
    }
}

/**
 *  PC를 재시작(Reboot)
 */
void kShutdown( const char* pcParamegerBuffer )
{
    kPrintf( "System Shutdown Start...\n" );
    
    // 키보드 컨트롤러를 통해 PC를 재시작
    kPrintf( "Press Any Key To Reboot PC..." );
    kGetCh();
    kReboot();         
}

// fault raise
void kRaisefault( const char* pcParamegerBuffer )
{
	long *ptr;
	ptr=  (long*)0x1ff000;
	*ptr = 0;	
}

/*
	QUEUE Function
*/
// Queue Print
int kPrintQueue( QUEUE* pq, char* vcCommandBuffer, int index) 
{
	int count = 0;
	int i = 0;
	char printCommand[20] = {0, };
	kMemCpy(printCommand, (char*)(pq->pvQueueArray) + pq->iDataSize * index, pq->iDataSize);
	for(i=0; i<sizeof(printCommand); i++)
	{
		if(printCommand[i] == 0 || printCommand[i] == ' ')
			break;
		count++;
	}

	for(i=0; i<count; i++)
	{
		vcCommandBuffer[i] = printCommand[i];
	}
	kPrintf("%s", printCommand);
	
	return count;
}
// Queue Set
void kSetqIndex( int * qIndex, QUEUE* pq )
{
	if(pq->iPutIndex == 0)
	{
		*qIndex = pq->iMaxDataCount - 1;
	}
	else
	{
		*qIndex = pq->iPutIndex - 1;
	}
}
// Queue Subtract
void kSubtractqIndex( int* qIndex, int maxDataCount )
{
	if( *qIndex == 0 )
	{
		*qIndex = maxDataCount - 1;
	}
	else
	{
		*qIndex = *qIndex - 1;
	}
}
// Queue Add Index
void kAddqIndex( int * qIndex, int maxDataCount)
{
	if( *qIndex == maxDataCount - 1)
	{
		*qIndex = 0;
	}
	else
	{
		*qIndex = *qIndex +1;	
	}
}

/*
	TAB Function
*/
int kTabfunc( char * pcCommandBuffer )
{
	int i, iSpaceIndex;
	int iCount;
	int iCommandBufferLength, iCommandLength;
	char * RealCmdWord;
	char testWord [40];
	int wordCount = 0;
	int outputNum = 0;
	int printLen  = 0;
  
	// 공백으로 구분된 커맨드를 추출
	iCommandBufferLength = kStrLen( pcCommandBuffer );

	for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
	{
		if( pcCommandBuffer[ iSpaceIndex ] == ' ' ) 
			break;
	}   
   
	iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY ); 
	char printBuffer[iCount][40];
  	
	// 커맨드 테이블을 검사해서 동일한 이름의 커맨드가 있는지 확인
	for( i = 0 ; i < iCount ; i++ )
	{  
		RealCmdWord = gs_vstCommandTable[ i ].pcCommand;
		iCommandLength = kStrLen( RealCmdWord );

		// 커맨드의 길이와 내용 검사
        	if( ( iCommandLength >= iSpaceIndex ) &&
            	    ( kMemCmp( RealCmdWord, pcCommandBuffer, iSpaceIndex ) == 0 ) )
        	{
			if(wordCount == 0) {
		        kMemCpy(testWord, RealCmdWord, iCommandLength);
		        kMemCpy(printBuffer[printLen], RealCmdWord, iCommandLength);
			testWord[iCommandLength] = '\0';
			printBuffer[wordCount][iCommandLength] = '\0';		
			wordCount++;
		
			}
			else {
				int testlen = kStrLen(testWord);
				if( testlen > iCommandLength)
					testlen = iCommandLength;
				
				for(outputNum=0; outputNum<testlen; outputNum++){
					if( kMemCmp(RealCmdWord, testWord, outputNum) != 0){
						outputNum--;
						break;		
					}
				}
				
				if( outputNum==testlen &&
				    kMemCmp(RealCmdWord, testWord, outputNum) != 0)	
					outputNum--;
				
				kMemCpy(testWord, RealCmdWord, outputNum);
				testWord[outputNum] = '\0';
				
				kMemCpy(printBuffer[wordCount], RealCmdWord, iCommandLength);
				printBuffer[wordCount][iCommandLength] = '\0';
				wordCount++;
	
    			}
            
       		}	
	}

	if(wordCount == 0)
	{		
		return iCommandBufferLength;
	}

	int testLen = kStrLen(testWord);

	if( kMemCmp(testWord, pcCommandBuffer, testLen) == 0 ) 
	{
          	kPrintf("\n");
		for(int k=0; k<wordCount; k++)
			kPrintf("%s ", printBuffer[k]);
		
		kPrintf( "\n%s", CONSOLESHELL_PROMPTMESSAGE );            
		kPrintf("%s", testWord);

		return testLen;
	}
	else {
   		int iCursorX, iCursorY;
   	
		kGetCursor( &iCursorX, &iCursorY );
               	kPrintStringXY( iCursorX - iSpaceIndex, iCursorY, " " );
               	kSetCursor( iCursorX - iSpaceIndex, iCursorY );
 
                kMemSet( pcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
		kMemCpy( pcCommandBuffer, testWord, testLen );
		kPrintf("%s", testWord);

		return testLen;
	}	

}

void kdummy( const char* pcParamegerBuffer )
{
	kPrintf("dummy \n");
}
