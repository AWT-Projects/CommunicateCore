
#include <sys/socket.h>
#include <sys/un.h>


#include "frame-format.h"
#include "cmd-handler.h"
#include "command.h"
#include <uds.h>
#include "process2.h"

#define MAX_CLIENTS 5
typedef struct {
    int iFd;
    int iSensorId;
} SENSOR_CLIENT;

void* sensorServerThread(void* arg) 
{
    SHARED_SENSOR_DATA *pstSharedSensorData = (SHARED_SENSOR_DATA*)arg;
    int iServerSock = createUdsServerSocket("/tmp/sensor_recv_data.sock", MAX_CLIENTS);
    fd_set masterSet, readSet;
    int iMaxFd;
    char chSensorData[128];
    FD_ZERO(&masterSet);
    FD_SET(iServerSock, &masterSet);
    iMaxFd = iServerSock;
    SENSOR_CLIENT stClients[MAX_CLIENTS] = {0};
    unsigned int  uiPacketFormatStatus;
    FRAME_HEADER *pstRcvHeader;
    char *pchPayload;
    printf("서버가 시작되었습니다. 클라이언트 연결을 기다립니다...\n");
    while (1) {
        readSet = masterSet;
        if (select(iMaxFd + 1, &readSet, NULL, NULL, NULL) < 0) {
            perror("select()");
            continue;
        }
        for (int iCurrFd = 0; iCurrFd <= iMaxFd; iCurrFd++) {
            if (!FD_ISSET(iCurrFd, &readSet)) 
                continue;

            if (iCurrFd == iServerSock) {
                int iClientSock = acceptUdsClient(iServerSock);
                if (iClientSock >= 0) {
                    FD_SET(iClientSock, &masterSet);
                    if (iClientSock > iMaxFd) 
                        iMaxFd = iClientSock;
                    printf("[SensorServer] 새로운 연결: fd=%d\n", iClientSock);
                }
            } else {
                int iRecvSize = recv(iCurrFd, chSensorData, sizeof(chSensorData) - 1, 0);
                if (iRecvSize <= 0) {
                    printf("[SensorServer] 연결 종료: fd=%d\n", iCurrFd);
                    close(iCurrFd);
                    FD_CLR(iCurrFd, &masterSet);
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (stClients[i].iFd == iCurrFd) {
                            stClients[i].iFd = -1;
                            break;
                        }
                    }
                    continue;
                }
                uiPacketFormatStatus = checkPacketFormat(chSensorData, iRecvSize);
                // 데이터 포멧이 안맞은 경우 처리 필요
                pstRcvHeader = (FRAME_HEADER *)chSensorData;
                pchPayload = (char *)(chSensorData + sizeof(FRAME_HEADER));
                pthread_mutex_lock(&pstSharedSensorData->mutex);
                dispatchCommand(pstRcvHeader->nCmd, pchPayload, (char *)&pstSharedSensorData->stSensorData, logging);

                // 프레임 포멧에서 src가 SP, Exter, Keyboard인 경우만 cond_signal 보내기
                pthread_cond_signal(&pstSharedSensorData->cond); 

                pthread_mutex_unlock(&pstSharedSensorData->mutex);
            }
        }
    }

    close(iServerSock);
    return NULL;
}