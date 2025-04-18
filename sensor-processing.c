#include "process2.h"
#include "frame-format.h"
#include "cmd-handler.h"
#include "uds-socket-path.h"
#include <sys/time.h>

void* sensorProcessingThread(void* arg) 
{    
    SHARED_SENSOR_DATA *pstSharedSensorData = (SHARED_SENSOR_DATA*)arg;
    struct timeval stNow;
    struct timespec stTimeout;
    PROCESSING_DATA *pstProcessingData;
    SENSOR_DATA stSensorData;
    char chMsgId[2];
    int iSendDataSize;
    char* pchSendData;
    MSG_ID stMsgId={};
    double dProcessData;

    if (pstSharedSensorData == NULL) {
        fprintf(stderr, "Shared data is NULL\n");
        pthread_exit(NULL);
    }

    iSendDataSize = getSendCmdSize(UDS3_PROCESSING_DATA) + sizeof(FRAME_HEADER) + sizeof(FRAME_TAIL);
    pchSendData = (char *)malloc(iSendDataSize);
    if (pchSendData == NULL) {
        fprintf(stderr, "malloc failed!\n");
        pthread_exit(NULL);
    }

    int iServerSock = createUdsServerSocket(SENSOR_PROCESSING_SOCK, 1);
    int iClientSock = acceptUdsClient(iServerSock);
    while (1) {
        gettimeofday(&stNow, NULL);
        stTimeout.tv_sec = stNow.tv_sec + 1; /**< 1초 타임아웃 설정 */
        stTimeout.tv_nsec = stNow.tv_usec * 1000;
        /**< 데이터 준비 상태 대기 */
        pthread_mutex_lock(&pstSharedSensorData->mutex);
        int ret = pthread_cond_timedwait(&pstSharedSensorData->cond, &pstSharedSensorData->mutex, &stTimeout);
        if (ret != ETIMEDOUT) {
            memcpy(&stSensorData, &pstSharedSensorData->stSensorData, sizeof(SENSOR_DATA));
            pthread_mutex_unlock(&pstSharedSensorData->mutex);
            dProcessData = stSensorData.stExtern1Data.dLatitude + stSensorData.stExtern1Data.dLongitude + stSensorData.stExtern1Data.dAltitude \
                            + stSensorData.stExtern2Data.dRoll + stSensorData.stExtern2Data.dPitch + stSensorData.stExtern2Data.dYaw \
                            + stSensorData.stExtern3Data.dAz + stSensorData.stExtern3Data.dEl \
                            + stSensorData.stExtern4Data.dLatitude + stSensorData.stExtern4Data.dLongitude + stSensorData.stExtern4Data.dAltitude;
            //연산 결과 대입  
            pstProcessingData = (PROCESSING_DATA *)(pchSendData+sizeof(FRAME_HEADER));
            pstProcessingData->dAz = dProcessData;
            pstProcessingData->dEl = dProcessData;
            makeSendPacket(UDS3_PROCESSING_DATA, &stMsgId, pchSendData);
            udsSendMsg(iClientSock, pchSendData, iSendDataSize);            
        }else{
            pthread_mutex_unlock(&pstSharedSensorData->mutex);
        }
    }
    // free(pchSendData);

    return NULL;
}