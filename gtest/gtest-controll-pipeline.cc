#include <gtest/gtest.h>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <pthread.h>
#include "../uds-socket-path.h"
#include "../process3.h"
#include "uds.h"
#include "../frame-format.h"
#include "../cmd-handler.h"
#include "../command.h"

// 센서 처리 UDS 서버: recvProcessingDataThread와 통신
void* sensorProcessingServerStub(void* arg) {
    int iServerSock = createUdsServerSocket(SENSOR_PROCESSING_SOCK, 1);
    int iClientSock = acceptUdsClient(iServerSock);
    char pchSendData[128] = {0};
    for (int i = 0; i < 3; ++i) {
        memset(pchSendData, 0x0, sizeof(pchSendData));
        sprintf(pchSendData,"sensorProcess%03d", i);
        udsSendMsg(iClientSock, pchSendData, strlen(pchSendData));
        usleep(100 * 1000);
    }

    close(iClientSock);
    close(iServerSock);
    return NULL;
}

// 명령 UDS 서버: recvCommandDataThread와 통신
void* commandServerStub(void* arg) {
    int iServerSock = createUdsServerSocket(LOCAL_COMMAND_SOCK, 1);
    int iClientSock = acceptUdsClient(iServerSock);
    char pchSendData[128] = {0};
    for (int i = 0; i < 3; ++i) {
        memset(pchSendData, 0x0, sizeof(pchSendData));
        sprintf(pchSendData,"command%03d", i);
        udsSendMsg(iClientSock, pchSendData, strlen(pchSendData));
        usleep(100 * 1000);
    }

    close(iClientSock);
    close(iServerSock);
    return NULL;
}

TEST(IntegrationTest, CommandAndProcessingThreadsWithPipeValidation) {
    pthread_t recvSensorProcessingId, recvCommandDataId;
    char chRecvData[DATA_SIZE];
    char chSensorProcessingData[DATA_SIZE];
    char chCommandDataData[DATA_SIZE];
    int iRecvSensorProcessingCount = 0;
    int iRecvCommandDataCount = 0;
    fd_set readfds;
    PIPE_INFO stPipeInfo;

    if (pipe(stPipeInfo.iCmdFd) < 0) {
        perror("pipe2");
        exit(EXIT_FAILURE);
    }

    if (pipe(stPipeInfo.iCtrlFd) < 0) {
        perror("pipe3");
        exit(EXIT_FAILURE);
    }

    // 서버 쓰레드 실행
    pthread_t processingServerThread, commandServerThread;
    pthread_create(&processingServerThread, nullptr, sensorProcessingServerStub, nullptr);
    pthread_create(&commandServerThread, nullptr, commandServerStub, nullptr);
    
    usleep(200);
    // 클라이언트 쓰레드 실행
    // 쓰레드 생성
    pthread_create(&recvSensorProcessingId, NULL, recvProcessingDataThread, (void*)&stPipeInfo);
    pthread_create(&recvCommandDataId, NULL, recvCommandDataThread, (void*)&stPipeInfo);

    int maxfd = (stPipeInfo.iCmdFd[0] > stPipeInfo.iCtrlFd[0] ? stPipeInfo.iCmdFd[0] : stPipeInfo.iCtrlFd[0]) + 1;

    // 파이프를 통해 6개의 메시지 수신 (센서 3 + 명령 3)
    int cmdCount = 0, procCount = 0;

    for ( ; iRecvCommandDataCount+iRecvSensorProcessingCount < 6; ) {
        FD_ZERO(&readfds);
        FD_SET(stPipeInfo.iCmdFd[0], &readfds);
        FD_SET(stPipeInfo.iCtrlFd[0], &readfds);
        int ret = select(maxfd, &readfds, NULL, NULL, NULL);
        if (ret > 0) {
            if (FD_ISSET(stPipeInfo.iCmdFd[0], &readfds)) {
                memset(chRecvData, 0x0, DATA_SIZE);                
                int n = read(stPipeInfo.iCmdFd[0], chRecvData, sizeof(chRecvData) - 1);
                if (n > 0) {
                    memset(chCommandDataData, 0x0, DATA_SIZE);
                    sprintf(chCommandDataData,"command%03d", iRecvCommandDataCount++);
                    EXPECT_STREQ(chRecvData, chCommandDataData);
                }
            }
            if (FD_ISSET(stPipeInfo.iCtrlFd[0], &readfds)) {
                memset(chRecvData, 0x0, DATA_SIZE);
                int n = read(stPipeInfo.iCtrlFd[0], chRecvData, sizeof(chRecvData) - 1);
                if (n > 0) {
                    memset(chSensorProcessingData, 0x0, DATA_SIZE);
                    sprintf(chSensorProcessingData,"sensorProcess%03d", iRecvSensorProcessingCount++);
                    EXPECT_STREQ(chRecvData, chSensorProcessingData);
                }
            }
        }
    }

    EXPECT_EQ(iRecvCommandDataCount, 3);
    EXPECT_EQ(iRecvSensorProcessingCount, 3);

    // 종료
    pthread_cancel(recvCommandDataId);
    pthread_cancel(recvSensorProcessingId);
    pthread_join(recvCommandDataId, NULL);
    pthread_join(recvSensorProcessingId, NULL);
    pthread_join(commandServerThread, NULL);
    pthread_join(processingServerThread, NULL);

    close(stPipeInfo.iCtrlFd[0]);
    close(stPipeInfo.iCtrlFd[1]);
    close(stPipeInfo.iCmdFd[0]);
    close(stPipeInfo.iCmdFd[1]);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}