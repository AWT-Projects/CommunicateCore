#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/select.h>
#include "tcp-frame-format.h"
#include "uds-cmd-handler.h"
#include "uds-command.h"
#include "uds.h"
#define MAX_CLIENTS 5
typedef struct {
    int iFd;
    int iSensorId;
} SENSOR_CLIENT;


// void handleNewClient(int clientSockFd, SensorClient *pstSensorClient) 
// {
//     for (int i = 0; i < MAX_CLIENTS; i++) {
//         if (!pstClientGroup[i].bActiveFlag) {
//             pstClientGroup[i].stThreadArgs.iClientSockFd = clientSockFd;
//             pstClientGroup[i].stThreadArgs.bExitFlag = false;
//             if (pthread_mutex_init(&pstClientGroup[i].stThreadArgs.uRunningMutex, NULL) != 0) {
//                 perror("pthread_mutex_init 실패");                
//             }
//             pstClientGroup[i].bActiveFlag = true;            
//             pthread_create(&pstClientGroup[i].recvThread, NULL, receiveThread, &pstClientGroup[i].stThreadArgs);
//             pthread_create(&pstClientGroup[i].sendThread, NULL, sendThread, &pstClientGroup[i].stThreadArgs);
//             printf("클라이언트 %d가 연결되었습니다.\n", clientSockFd);
//             return;
//         }
//     }
//     printf("최대 클라이언트 수 초과: 연결 거부\n");
//     close(clientSockFd);
// }

// void cleanupClient(ClientInfo *pstClient) 
// {
//     if (pstClient->bActiveFlag) {
//         close(pstClient->stThreadArgs.iClientSockFd);
//         pthread_join(pstClient->recvThread, NULL);
//         pthread_join(pstClient->sendThread, NULL);
//         pstClient->bActiveFlag = false;
//         printf("클라이언트 연결이 해제되었습니다.\n");
//     }
// }

// // ACU 상태
// typedef struct {
//     float azimuth;
//     float elevation;
//     int mode; // 0: 수동, 1: 자동
//     pthread_mutex_t lock;
// } ACUStatus;

// ACUStatus acuStatus = {0.0f, 0.0f, 0, PTHREAD_MUTEX_INITIALIZER};

// // 명령 처리 함수
// void process_command(const char* cmd) {
//     if (strncmp(cmd, "SET", 3) == 0) {
//         float az, el;
//         if (sscanf(cmd, "SET %f %f", &az, &el) == 2) {
//             pthread_mutex_lock(&acuStatus.lock);
//             acuStatus.azimuth = az;
//             acuStatus.elevation = el;
//             acuStatus.mode = 0; // 수동모드로 전환
//             pthread_mutex_unlock(&acuStatus.lock);
//             printf("[CMD] 수동 제어: AZ=%.2f, EL=%.2f\n", az, el);
//         }
//     } else if (strncmp(cmd, "AUTO", 4) == 0) {
//         pthread_mutex_lock(&acuStatus.lock);
//         acuStatus.mode = 1;
//         pthread_mutex_unlock(&acuStatus.lock);
//         printf("[CMD] 자동 제어 모드로 전환\n");
//     } else {
//         printf("[CMD] 알 수 없는 명령: %s\n", cmd);
//     }
// }

// // 센서 데이터 연산 함수
// void process_sensor_data(float sensor_az, float sensor_el) {
//     pthread_mutex_lock(&acuStatus.lock);
//     if (acuStatus.mode == 1) { // 자동 모드일 때만 적용
//         acuStatus.azimuth = sensor_az;
//         acuStatus.elevation = sensor_el;
//         printf("[SENSOR] 자동 제어 적용: AZ=%.2f, EL=%.2f\n", sensor_az, sensor_el);
//     } else {
//         printf("[SENSOR] 수동 모드, 무시됨\n");
//     }
//     pthread_mutex_unlock(&acuStatus.lock);
// }

// // Thread 1: 명령 수신
// void* commandReceiverThread(void* arg) {
//     int sock;
//     struct sockaddr_un addr;
//     char cmdBuf[128];

//     sock = socket(AF_UNIX, SOCK_STREAM, 0);
//     addr.sun_family = AF_UNIX;
//     strcpy(addr.sun_path, "/tmp/acu_cmd.sock");

//     while (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
//         perror("Waiting for proc1 (cmd)");
//         sleep(1);
//     }

//     while (1) {
//         int len = recv(sock, cmdBuf, sizeof(cmdBuf) - 1, 0);
//         if (len > 0) {
//             cmdBuf[len] = '\0';
//             printf("[Thread1] 명령 수신: %s\n", cmdBuf);
//             process_command(cmdBuf);
//         }
//     }

//     close(sock);
//     return NULL;
// }

// // Thread 2: 방위각/고각 전송
// void* statusSenderThread(void* arg) {
//     int sock;
//     struct sockaddr_un addr;

//     sock = socket(AF_UNIX, SOCK_STREAM, 0);
//     addr.sun_family = AF_UNIX;
//     strcpy(addr.sun_path, "/tmp/acu_status.sock");

//     while (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
//         perror("Waiting for proc6 (status)");
//         sleep(1);
//     }

//     while (1) {
//         pthread_mutex_lock(&acuStatus.lock);
//         float az = acuStatus.azimuth;
//         float el = acuStatus.elevation;
//         int mode = acuStatus.mode;
//         pthread_mutex_unlock(&acuStatus.lock);

//         char msg[64];
//         snprintf(msg, sizeof(msg), "AZ:%.2f EL:%.2f MODE:%s", az, el, mode == 0 ? "MANUAL" : "AUTO");
//         send(sock, msg, strlen(msg), 0);

//         printf("[Thread2] 상태 전송: %s\n", msg);
//         sleep(1);
//     }

//     close(sock);
//     return NULL;
// }

// Thread 3: 센서 데이터 수신
void* sensorServerThread(void* arg) {    
    int iServerSock = createUdsServerSock("/tmp/sensor_recv_data.sock", MAX_CLIENTS);
    fd_set masterSet, readSet;
    int iMaxFd;
    char chSensorData[128];
    FD_ZERO(&masterSet);
    FD_SET(iServerSock, &masterSet);
    iMaxFd = iServerSock;
    SENSOR_CLIENT stClients[MAX_CLIENTS] = {0};
    unsigned int  uiPacketFormatStatus;
    FRAME_HEADER *pstRcvHeader;
    FRAME_TAIL *pstRcvTail;
    char *pchPayload;
    char chSensorStructData[sizeof(SENSOR_DATA)];
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
                pstRcvTail = (FRAME_TAIL *)(chSensorData + sizeof(FRAME_HEADER) + pstRcvHeader->iLength);
                
                dispatchCommand(pstRcvHeader->nCmd, pchPayload, chSensorStructData, logging);

            }
        }
    }

    close(iServerSock);
    return NULL;
}



// main
int main() {
    pthread_t sensorThreadId;
    // , tid2, tid3;

    // pthread_create(&tid1, NULL, commandReceiverThread, NULL);
    // pthread_create(&tid2, NULL, statusSenderThread, NULL);
    pthread_create(&sensorThreadId, NULL, sensorServerThread, NULL);

    // pthread_join(tid1, NULL);
    // pthread_join(tid2, NULL);
    pthread_join(sensorThreadId, NULL);

    return 0;
}
