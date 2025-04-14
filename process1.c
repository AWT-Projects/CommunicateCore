/**
 * @file tracking-ctrl.c
 * @brief TCP 서버 소켓을 생성하여 여러 클라이언트와의 통신을 관리하는 서버 프로그램
 * 
 * @details 본 프로그램은 클라이언트들과의 동시 통신을 지원하기 위해 수신 스레드와 송신 스레드를 사용하며, 
 *          클라이언트의 연결 및 해제를 관리합니다. 서버는 지정된 포트에서 클라이언트의 연결 요청을 대기하고, 
 *          수신된 데이터를 처리한 뒤 클라이언트에게 응답을 보냅니다.
 * 
 * @author 박철우
 * @date 2015.05
 */

#include "process1.h"

#include <unistd.h>

#define PORT        8080
#define MAX_CLIENTS 5

/**
 * @brief 메인 함수: TCP 서버 소켓을 생성하고 클라이언트 연결을 처리
 * @return int 실행 결과
 * 
 * @details 서버 소켓을 생성하고 클라이언트의 연결 요청을 대기합니다. 
 *          연결된 클라이언트별로 송신 및 수신 스레드를 생성하여 데이터를 처리합니다.
 */
int main() {
    int iServerSock, iClientSock;
    struct sockaddr_in stSockClientAddr;
    socklen_t uiClientAddrLen = sizeof(stSockClientAddr);
    fd_set stReadFds;
    CLIENT_INFO stClientGroup[MAX_CLIENTS] = {0}; /**< 클라이언트 정보 배열 초기화 */

    iServerSock = createServerSocket(PORT, MAX_CLIENTS);
    fprintf(stdout, "포트 %d에서 서버 대기 중\n", PORT);
    
    while (1) {
        FD_ZERO(&stReadFds);
        FD_SET(iServerSock, &stReadFds);
        int iMaxSock = iServerSock;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            int iSock = stClientGroup[i].iClientSock;
            if (iSock > 0)
                FD_SET(iSock, &stReadFds);
            if (iSock > iMaxSock)
                iMaxSock = iSock;
        }

        int iActivitySock = select(iMaxSock + 1, &stReadFds, NULL, NULL, NULL);
        if ((iActivitySock < 0) && (errno != EINTR)) {
            perror("select 실패");
        }

        if (FD_ISSET(iServerSock, &stReadFds)) {
            if ((iClientSock = accept(iServerSock, (struct sockaddr *)&stSockClientAddr, &uiClientAddrLen)) < 0) {
                perror("accept 실패");
                exit(EXIT_FAILURE);
            }

            fprintf(stdout, "새 연결: 소켓 FD %d, IP %s, 포트 %d\n", 
                    iClientSock, 
                    inet_ntoa(stSockClientAddr.sin_addr), 
                    ntohs(stSockClientAddr.sin_port));

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (stClientGroup[i].iClientSock == 0) {
                    /**< 빈 슬롯에 클라이언트 추가 */
                    stClientGroup[i].iClientSock = iClientSock;

                    if (pthread_mutex_init(&stClientGroup[i].stSharedData.mutex, NULL) != 0) {
                        perror("pthread_mutex_init 실패");
                    }

                    stClientGroup[i].bExitFlag = false;
                    fprintf(stdout, "소켓 목록에 추가: %d\n", i);

                    /**< 수신 및 송신 스레드 생성 */
                    pthread_create(&stClientGroup[i].recvThreadId, NULL, receiveThread, &stClientGroup[i]);
                    pthread_create(&stClientGroup[i].sendThreadId, NULL, sendThread, &stClientGroup[i]);
                    break;
                }
            }
        }
    }

    return 0;
}
