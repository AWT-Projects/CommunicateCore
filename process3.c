#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include "process3.h"

int main() {
    pthread_t recvSensorProcessingId, recvCommandDataId;
    char chRecvData[DATA_SIZE];
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

    // 쓰레드 생성
    pthread_create(&recvSensorProcessingId, NULL, recvProcessingDataThread, (void*)&stPipeInfo);
    pthread_create(&recvCommandDataId, NULL, recvCommandDataThread, (void*)&stPipeInfo);

    int maxfd = (stPipeInfo.iCmdFd[0] > stPipeInfo.iCtrlFd[0] ? stPipeInfo.iCmdFd[0] : stPipeInfo.iCtrlFd[0]) + 1;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(stPipeInfo.iCmdFd[0], &readfds);
        FD_SET(stPipeInfo.iCtrlFd[0], &readfds);

        int ret = select(maxfd, &readfds, NULL, NULL, NULL);
        if (ret > 0) {
            if (FD_ISSET(stPipeInfo.iCmdFd[0], &readfds)) {
                int n = read(stPipeInfo.iCmdFd[0], chRecvData, sizeof(chRecvData) - 1);
                if (n > 0) {
                    chRecvData[n] = '\0';
                }
            }
            if (FD_ISSET(stPipeInfo.iCtrlFd[0], &readfds)) {
                int n = read(stPipeInfo.iCtrlFd[0], chRecvData, sizeof(chRecvData) - 1);
                if (n > 0) {
                    chRecvData[n] = '\0';
                }
            }
        }
    }

    // 종료 대기 (사실 이 예제에서는 무한 루프라 도달하지 않음)
    pthread_join(recvSensorProcessingId, NULL);
    pthread_join(recvCommandDataId, NULL);
    close(stPipeInfo.iCtrlFd[0]);
    close(stPipeInfo.iCtrlFd[1]);
    close(stPipeInfo.iCmdFd[0]);
    close(stPipeInfo.iCmdFd[1]);
    return 0;
}