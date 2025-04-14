#include "process1.h"

/**
 * @brief 클라이언트로 데이터를 송신하는 스레드 함수
 * @param arg CLIENT_INFO 구조체 포인터
 * @return NULL
 * 
 * @details SHARED_DATA 구조체에 저장된 데이터를 클라이언트 소켓으로 전송합니다. 
 *          수신 스레드에서 데이터가 준비되면 조건 변수를 통해 알림을 받습니다.
 */
void *sendThread(void *arg) {
    CLIENT_INFO *pstClientInfo = (CLIENT_INFO *)arg;
    struct sockaddr_in stSockClientAddr;
    socklen_t uiClientAddrLen = sizeof(stSockClientAddr);
    char achBuffer[BUFFER_SIZE];
    bool bSendFlag = false;
    bool bExitFlag = false;
    struct timeval stNow;
    struct timespec stTimeout;

    if (getpeername(pstClientInfo->iClientSock, (struct sockaddr *)&stSockClientAddr, &uiClientAddrLen) == -1) {
        perror("getpeername 실패");
        pthread_exit(NULL);
    }

    while (1) {
        gettimeofday(&stNow, NULL);
        stTimeout.tv_sec = stNow.tv_sec + 1; /**< 1초 타임아웃 설정 */
        stTimeout.tv_nsec = stNow.tv_usec * 1000;

        pthread_mutex_lock(&pstClientInfo->exitFlagMutex);
        bExitFlag = pstClientInfo->bExitFlag;
        pthread_mutex_unlock(&pstClientInfo->exitFlagMutex);

        if (bExitFlag) {
            break;
        }

        /**< 데이터 준비 상태 대기 */
        int ret = pthread_cond_timedwait(&pstClientInfo->stSharedData.cond, &pstClientInfo->stSharedData.mutex, &stTimeout);
        if (ret == ETIMEDOUT) {
            fprintf(stderr, ".");
        } else {
            memset(achBuffer, 0x0, BUFFER_SIZE);
            pstClientInfo->stSharedData.hasData = false; /**< 데이터 사용 완료 플래그 초기화 */
            memcpy(achBuffer, pstClientInfo->stSharedData.chData, strlen(pstClientInfo->stSharedData.chData));
            pthread_mutex_unlock(&pstClientInfo->stSharedData.mutex);
            bSendFlag = true;
        }

        if (bSendFlag) {
            /**< 데이터 송신 */
            sendMessage(pstClientInfo->iClientSock, achBuffer, strlen(achBuffer));
        }
        bSendFlag = false;
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
