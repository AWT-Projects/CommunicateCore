#include "process3.h"
#include "frame-format.h"
#include "cmd-handler.h"
#include "uds-socket-path.h"
#include <sys/time.h>

void* recvProcessingDataThread(void* arg) 
{    
    struct timeval stNow;
    struct timespec stTimeout;
    PROCESSING_DATA *pstProcessingData;
    int iRecvDataSize;
    char achRecvData[DATA_SIZE];
    MSG_ID stMsgId={0x0, 0x01};
    PIPE_INFO* pstPipeInfo = (PIPE_INFO *)arg;    

    int iClientSock = createUdsClientSocket(SENSOR_PROCESSING_SOCK);
    while (1) {
         iRecvDataSize = udsRecvMsgTimeout(iClientSock, achRecvData, sizeof(achRecvData), 100);
         if(iRecvDataSize > 0){
            //fram 검사 후 정상이면 바로 전달
            write(pstPipeInfo->iCtrlFd[1], achRecvData, iRecvDataSize);
         }else if(iRecvDataSize == 0){
            //Timeout
         }else{

         }
    }
    return NULL;
}