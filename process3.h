#ifndef PROCESS3_H
#define PROCESS3_H

#include <uds.h>
#include <pthread.h>
#include <sys/select.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "command.h"

#define DATA_SIZE 128

void* recvProcessingDataThread(void* arg);
void* recvCommandDataThread(void* arg);

typedef struct __attribute__((__packed__)){
	int iCtrlFd[2];
    int iCmdFd[2];
    int iSendFd[2];	
}PIPE_INFO;

#endif