#ifndef PROCESS1_H
#define PROCESS1_H

#include <tcp-sock.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/time.h>

/**
 * @brief   버퍼 크기를 정의합니다.
 * @details 데이터 송수신에 사용되는 버퍼의 크기를 바이트 단위로 나타냅니다.
 */
#define BUFFER_SIZE 1024

/**
 * @brief 클라이언트와의 데이터 공유를 위한 구조체
 * 
 * @details 클라이언트로부터 수신한 데이터를 저장하고, 데이터의 사용 상태를 관리하며, 
 *          동기화를 위해 뮤텍스와 조건 변수를 포함합니다.
 */
typedef struct {
    char chData[BUFFER_SIZE];       /**< 클라이언트로부터 수신한 데이터 */
    bool hasData;                   /**< 데이터 존재 여부 플래그 */
    pthread_mutex_t mutex;          /**< 데이터 접근 동기화를 위한 뮤텍스 */
    pthread_cond_t cond;            /**< 데이터 준비 상태를 알리는 조건 변수 */
} SHARED_DATA;

/**
 * @brief 클라이언트 정보를 저장하는 구조체
 * 
 * @details 클라이언트 소켓과 관련된 상태 정보를 저장하고, 클라이언트별 스레드와 데이터 공유 구조체를 포함합니다.
 */
typedef struct {
    int iClientSock;                /**< 클라이언트 소켓 파일 디스크립터 */
    bool bExitFlag;                 /**< 연결 종료 플래그 */
    SHARED_DATA stSharedData;       /**< 클라이언트와 공유되는 데이터 구조체 */
    pthread_t recvThreadId;         /**< 수신 스레드 ID */
    pthread_t sendThreadId;         /**< 송신 스레드 ID */
    pthread_mutex_t exitFlagMutex;  /**< 연결 종료 플래그 동기화를 위한 뮤텍스 */
} CLIENT_INFO;


void *receiveThread(void *arg);
void *sendThread(void *arg);
#endif