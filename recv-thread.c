#include "process1.h"
#include "frame-format.h"
#include "cmd-handler.h"
/**
 * @brief 클라이언트로부터 데이터를 수신하는 스레드 함수
 * @param arg CLIENT_INFO 구조체 포인터
 * @return NULL
 * 
 * @details 클라이언트 소켓으로부터 데이터를 읽어 SHARED_DATA 구조체에 저장합니다. 
 *          데이터를 수신하면 조건 변수를 통해 송신 스레드에 알립니다.
 */

void *receiveThread(void *arg) 
{
    CLIENT_INFO *pstClientInfo = (CLIENT_INFO *)arg;
    FRAME_HEADER *pstRcvHeader;
    FRAME_TAIL *pstRcvTail;
    FRAME_HEADER *pstSndHeader;
    FRAME_TAIL *pstSndTail;
    struct sockaddr_in stSockClientAddr;
    socklen_t uiClientAddrLen = sizeof(stSockClientAddr);
    char achBuffer[BUFFER_SIZE];
    int iRecvSize;
    int iTotalRecvSize;
    int iResponseSize;
    int iRetVal;
    char *pchResponse;
    char chMsgId[2];
    char *pchPayload;
    char *pchCmdResult;
    unsigned int  uiPacketFormatStatus;

    fprintf(stderr,"### %s()%d Thread START ###\n", __func__,__LINE__);
    if (getpeername(pstClientInfo->iClientSock, (struct sockaddr *)&stSockClientAddr, &uiClientAddrLen) == -1) {
        perror("getpeername 실패");
        pthread_exit(NULL);
    }

    while (1) {
        memset(achBuffer, 0x0, BUFFER_SIZE);
        iRecvSize = recvMsgTimeout(pstClientInfo->iClientSock, achBuffer, BUFFER_SIZE, 500);
        if(iRecvSize == 0 ){
            /**< 클라이언트 연결 종료 처리 필요*/
            handleClientDisconnection(pstClientInfo->iClientSock);
            pstClientInfo->iClientSock = 0;
            break;
        }else if(iRecvSize < 0){
            fprintf(stdout, "select error %d\n", iRecvSize);
        } else {
            /**< 데이터 수신 성공 */            
            iTotalRecvSize = getTotalRecvSize(achBuffer, iRecvSize);
            if(iRecvSize < iTotalRecvSize){
                fprintf(stderr,"### %s():%d ###\n", __func__,__LINE__);
                /**< todo 데이터가 큰 경우 */
            } else if(iRecvSize > iTotalRecvSize){
                fprintf(stderr,"### %s():%d ###\n", __func__,__LINE__);
                /**< todo 삭제할 데이터 또는 이상한 데이터가 같이 들어온 경우 */
            }else{
                uiPacketFormatStatus = checkPacketFormat(achBuffer, iRecvSize);
                // 데이터 포멧이 안맞은 경우 처리 필요

                pstRcvHeader = (FRAME_HEADER *)achBuffer;
                pchPayload = (char *)(achBuffer + sizeof(FRAME_HEADER));
                pstRcvTail = (FRAME_TAIL *)(achBuffer + sizeof(FRAME_HEADER) + pstRcvHeader->iLength);
                
                iResponseSize = getSendCmdSize((cmd_id_t)pstRcvHeader->nCmd) + sizeof(FRAME_HEADER) + sizeof(FRAME_TAIL);
                pchResponse = (char *)malloc(iResponseSize);
                memset(pchResponse, 0x0, iResponseSize);
                
                pstSndHeader = (FRAME_HEADER *)pchResponse;
                pchCmdResult = (char *)(pchResponse + sizeof(FRAME_HEADER));
                pstSndTail = (FRAME_TAIL *)(pchResponse + sizeof(FRAME_HEADER)+ getSendCmdSize((cmd_id_t)pstRcvHeader->nCmd));
                chMsgId[0] = pstRcvHeader->chDstId;
                chMsgId[1] = pstRcvHeader->chSrcId;
                makeSendPacket(pstRcvHeader->nCmd, chMsgId, pchResponse);
                iRetVal = dispatchCommand(pstRcvHeader->nCmd, pchPayload, pchCmdResult, logging);
                                
                pthread_mutex_lock(&pstClientInfo->stSharedData.mutex);                
                /**< 데이터 존재 플래그 설정 */
                pstClientInfo->stSharedData.hasData = true; 
                memset(pstClientInfo->stSharedData.chData, 0x0, BUFFER_SIZE);
                memcpy(pstClientInfo->stSharedData.chData, pchResponse, iResponseSize);
                fprintf(stderr,"\nRAW DATA\n");
                for(int i=0; i<iResponseSize; i++){
                    fprintf(stderr,"%02x ", (unsigned char)pchResponse[i]);
                }
                free(pchResponse);
                /**< 조건 변수 신호 전송 */
                pthread_cond_signal(&pstClientInfo->stSharedData.cond); 
                pthread_mutex_unlock(&pstClientInfo->stSharedData.mutex);
            }
        }
    }

    fprintf(stdout, "%s():%d 클라이언트 연결 해제, 소켓 IP: %s, 포트: %d\n", 
            __func__, __LINE__, 
            inet_ntoa(stSockClientAddr.sin_addr), 
            ntohs(stSockClientAddr.sin_port));

    pthread_mutex_lock(&pstClientInfo->exitFlagMutex);
    pstClientInfo->bExitFlag = true;
    pthread_mutex_unlock(&pstClientInfo->exitFlagMutex);
    pthread_exit(NULL);
}