#include "process2.h"
#include "frame-format.h"
#include "cmd-handler.h"
#include <sys/time.h>

void* processingSensorDataThread(void* arg) 
{    
    SHARED_SENSOR_DATA *pstSharedSensorData = (SHARED_SENSOR_DATA*)arg;
    struct timeval stNow;
    struct timespec stTimeout;
    // ACU_AZ_EL_CTRL_DATA *pstAcuAzElCtrlData;
    // SENSOR_DATA stSensorData;
    // char chMsgId[2];
    // int iSendDataSize;
    // char* pchSendData;

    // iSendDataSize = getSendCmdSize(CMD_UDS_ACU_AZ_EL_CTRL_DATA) + sizeof(FRAME_HEADER) + sizeof(FRAME_TAIL);
    // pchSendData = (char *)malloc(iSendDataSize);
                
    // int iServerSock = createUdsServerSocket("/tmp/sensor_recv_data.sock", 1);
    // int iClientSock = acceptUdsClient(iServerSock);

    while (1) {
        gettimeofday(&stNow, NULL);
        stTimeout.tv_sec = stNow.tv_sec + 1; /**< 1초 타임아웃 설정 */
        stTimeout.tv_nsec = stNow.tv_usec * 1000;
        /**< 데이터 준비 상태 대기 */
        pthread_mutex_lock(&pstSharedSensorData->mutex);
        int ret = pthread_cond_timedwait(&pstSharedSensorData->cond, &pstSharedSensorData->mutex, &stTimeout);
        if (ret != ETIMEDOUT) {
            // memcpy(&stSensorData, &pstSharedSensorData->stSensorData, sizeof(SENSOR_DATA));
            // pthread_mutex_unlock(&pstSharedSensorData->mutex);

            // 연산 알고리즘 
            fprintf(stderr,"Latitude : %lf\n", pstSharedSensorData->stSensorData.stExtern1Data.dLatitude);
            fprintf(stderr,"Longitude : %lf\n", pstSharedSensorData->stSensorData.stExtern1Data.dLongitude);
            fprintf(stderr,"Altitude : %lf\n", pstSharedSensorData->stSensorData.stExtern1Data.dAltitude);

            fprintf(stderr,"Roll : %lf\n", pstSharedSensorData->stSensorData.stExtern2Data.dRoll);
            fprintf(stderr,"Pitch : %lf\n", pstSharedSensorData->stSensorData.stExtern2Data.dPitch);
            fprintf(stderr,"Yaw : %lf\n", pstSharedSensorData->stSensorData.stExtern2Data.dYaw);

            // fprintf(stderr,"Latitude : %lf\n", pstSharedSensorData->stSensorData.stExternData.dLatitude);
            // fprintf(stderr,"Longitude : %lf\n", pstSharedSensorData->stSensorData.stExternData.dLongitude);
            // fprintf(stderr,"Altitude : %lf\n", pstSharedSensorData->stSensorData.stExternData.dAltitude);
            
            fprintf(stderr,"AZ : %lf\n", pstSharedSensorData->stSensorData.stExtern3Data.dAz);
            fprintf(stderr,"EL : %lf\n", pstSharedSensorData->stSensorData.stExtern3Data.dEl);

            // fprintf(stderr,"AZ : %lf\n", pstSharedSensorData->stSensorData.stKeyboardData.dAz);
            // fprintf(stderr,"EL : %lf\n", pstSharedSensorData->stSensorData.stKeyboardData.dEl);

            //연산 결과 대입
            // chMsgId[0] = 1;
            // chMsgId[1] = 2;            
            // pstAcuAzElCtrlData = (ACU_AZ_EL_CTRL_DATA *)(pchSendData+sizeof(FRAME_HEADER));
            // pstAcuAzElCtrlData->dAz = 20.1;
            // pstAcuAzElCtrlData->dEl = 21.2;
            // makeSendPacket(CMD_UDS_ACU_AZ_EL_CTRL_DATA, chMsgId, pchSendData);
            // udsSendMsg(iClientSock, pchSendData, iSendDataSize);            
        }
    }
    // free(pchSendData);

    return NULL;
}