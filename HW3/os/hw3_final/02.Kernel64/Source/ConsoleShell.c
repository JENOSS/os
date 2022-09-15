/**
 *  file    ConsoleShell.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   �ܼ� �п� ���õ� �ҽ� ����
 */

#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"

// Ŀ�ǵ� ���̺� ����
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
//  ���� ���� �����ϴ� �ڵ�
//==============================================================================
/**
 *  ���� ���� ����
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
	
    // ������Ʈ ���
    kPrintf( CONSOLESHELL_PROMPTMESSAGE );
   
    kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
    iCommandBufferIndex = 0;
	
    while( 1 )
    {
        // Ű�� ���ŵ� ������ ���
       
        bKey = kGetCh();
	 // Backspace Ű ó��
        if( bKey == KEY_BACKSPACE )
        {
            if( iCommandBufferIndex > 0 )
            {
                // ���� Ŀ�� ��ġ�� �� �� ���� ������ �̵��� ���� ������ ����ϰ� 
                // Ŀ�ǵ� ���ۿ��� ������ ���� ����
                kGetCursor( &iCursorX, &iCursorY );
		if(iCursorX>7)	{
               		kPrintStringXY( iCursorX - 1, iCursorY, " " );
               		kSetCursor( iCursorX - 1, iCursorY );
               		iCommandBufferIndex--;	
			vcCommandBuffer[ iCommandBufferIndex ] = ' ';
		}	
            }
        }
        // ���� Ű ó��
        else if( bKey == KEY_ENTER )
        {
            kPrintf( "\n" );
	    historyIndexingRound = 0;            
            
	    if( iCommandBufferIndex > 0 )
            {
                // Ŀ�ǵ� ���ۿ� �ִ� ����� ����
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
            
            // ������Ʈ ��� �� Ŀ�ǵ� ���� �ʱ�ȭ
            kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE );            
            kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        // ����Ʈ Ű, CAPS Lock, NUM Lock, Scroll Lock�� ����
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
            // ���ۿ� ������ �������� ���� ����
            if( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
            {
                vcCommandBuffer[ iCommandBufferIndex++ ] = bKey;
                kPrintf( "%c", bKey );
            }
        }
    }
}

/*
 *  Ŀ�ǵ� ���ۿ� �ִ� Ŀ�ǵ带 ���Ͽ� �ش� Ŀ�ǵ带 ó���ϴ� �Լ��� ����
 */
void kExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;
    
    // �������� ���е� Ŀ�ǵ带 ����
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if( pcCommandBuffer[ iSpaceIndex ] == ' ' )
        {
            break;
        }
    }
    
    // Ŀ�ǵ� ���̺��� �˻��ؼ� ������ �̸��� Ŀ�ǵ尡 �ִ��� Ȯ��
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        // Ŀ�ǵ��� ���̿� ������ ������ ��ġ�ϴ��� �˻�
        if( ( iCommandLength == iSpaceIndex ) &&
            ( kMemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer,
                       iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    // ����Ʈ���� ã�� �� ���ٸ� ���� ���
    if( i >= iCount )
    {
        kPrintf( "'%s' is not found.\n", pcCommandBuffer );
    }
}

/**
 *  �Ķ���� �ڷᱸ���� �ʱ�ȭ
 */
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  �������� ���е� �Ķ������ ����� ���̸� ��ȯ
 */
int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    // �� �̻� �Ķ���Ͱ� ������ ����
    if( pstList->iLength <= pstList->iCurrentPosition )
    {
        return 0;
    }
    
    // ������ ���̸�ŭ �̵��ϸ鼭 ������ �˻�
    for( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if( pstList->pcBuffer[ i ] == ' ' )
        {
            break;
        }
    }
    
    // �Ķ���͸� �����ϰ� ���̸� ��ȯ
    kMemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength = i - pstList->iCurrentPosition;
    pcParameter[ iLength ] = '\0';

    // �Ķ������ ��ġ ������Ʈ
    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
//==============================================================================
//  Ŀ�ǵ带 ó���ϴ� �ڵ�
//==============================================================================
/**
 *  �� ������ ���
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

    // ���� �� Ŀ�ǵ��� ���̸� ���
    for( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if( iLength > iMaxCommandLength )
        {
            iMaxCommandLength = iLength;
        }
    }
    
    // ���� ���
    for( i = 0 ; i < iCount ; i++ )
    {
        kPrintf( "%s", gs_vstCommandTable[ i ].pcCommand );
        kGetCursor( &iCursorX, &iCursorY );
        kSetCursor( iMaxCommandLength, iCursorY );
        kPrintf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );
    }
}

/**
 *  ȭ���� ���� 
 */
void kCls( const char* pcParameterBuffer )
{
    // �� ������ ����� ������ ����ϹǷ� ȭ���� ���� ��, ���� 1�� Ŀ�� �̵�
    kClearScreen();
    kSetCursor( 0, 1 );
}

/**
 *  �� �޸� ũ�⸦ ���
 */
void kShowTotalRAMSize( const char* pcParameterBuffer )
{
    kPrintf( "Total RAM Size = %d MB\n", kGetTotalRAMSize() );
}

/**
 *  ���ڿ��� �� ���ڸ� ���ڷ� ��ȯ�Ͽ� ȭ�鿡 ���
 */
void kStringToDecimalHexTest( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    int iCount = 0;
    long lValue;
    
    // �Ķ���� �ʱ�ȭ
    kInitializeParameter( &stList, pcParameterBuffer );
    
    while( 1 )
    {
        // ���� �Ķ���͸� ����, �Ķ������ ���̰� 0�̸� �Ķ���Ͱ� ���� ���̹Ƿ�
        // ����
        iLength = kGetNextParameter( &stList, vcParameter );
        if( iLength == 0 )
        {
            break;
        }

        // �Ķ���Ϳ� ���� ������ ����ϰ� 16�������� 10�������� �Ǵ��Ͽ� ��ȯ�� ��
        // ����� printf�� ���
        kPrintf( "Param %d = '%s', Length = %d, ", iCount + 1, 
                 vcParameter, iLength );

        // 0x�� �����ϸ� 16����, �׿ܴ� 10������ �Ǵ�
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
 *  PC�� �����(Reboot)
 */
void kShutdown( const char* pcParamegerBuffer )
{
    kPrintf( "System Shutdown Start...\n" );
    
    // Ű���� ��Ʈ�ѷ��� ���� PC�� �����
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
  
	// �������� ���е� Ŀ�ǵ带 ����
	iCommandBufferLength = kStrLen( pcCommandBuffer );

	for( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
	{
		if( pcCommandBuffer[ iSpaceIndex ] == ' ' ) 
			break;
	}   
   
	iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY ); 
	char printBuffer[iCount][40];
  	
	// Ŀ�ǵ� ���̺��� �˻��ؼ� ������ �̸��� Ŀ�ǵ尡 �ִ��� Ȯ��
	for( i = 0 ; i < iCount ; i++ )
	{  
		RealCmdWord = gs_vstCommandTable[ i ].pcCommand;
		iCommandLength = kStrLen( RealCmdWord );

		// Ŀ�ǵ��� ���̿� ���� �˻�
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
