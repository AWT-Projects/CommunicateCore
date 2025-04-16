#ifndef PROCESS2_H
#define PROCESS2_H

#include <pthread.h>
#include <sys/select.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "command.h"

void* sensorServerThread(void* arg);
void* processingSensorDataThread(void* arg);

typedef struct{
    SENSOR_DATA stSensorData;
    pthread_mutex_t mutex;    /**<  */
    pthread_cond_t  cond;            /**< 데이터 준비 상태를 알리는 조건 변수 */
}SHARED_SENSOR_DATA;


#endif