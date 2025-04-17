#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H

#include "logger.h"

// 명령 ID 정의
typedef enum {
    // process1에서 TCP/IP 와 UDS1 에 사용되는 ID (값의 범위 : 0 ~ 99)
    CMD_KEEP_ALIVE = 0,
    CMD_IBIT,
    CMD_RBIT,
    CMD_CBIT,

    // process2에서 UDS2에 사용되는 ID (값의 범위 : 100 ~ 199)
    UDS2_EXTERN1_DATA = 100,
    UDS2_EXTERN2_DATA,
    UDS2_EXTERN3_DATA,
    UDS2_EXTERN4_DATA,

    // process2에서 UDS3에 사용되는 ID (값의 범위 : 200 ~ 299)
    UDS3_PROCESSING_DATA = 200,

    // process3에서 UDS4에 사용되는 ID (값의 범위 : 300 ~ 399)
    UDS4_CTRL_DATA = 300,
    UDS4_COMMAND,
        
    CMD_UNKNOWN
} cmd_id_t;

// 핸들러 함수 타입
typedef int (*command_func_t)(const char* pchPayload, char* pchResponse);

// 로그 함수 타입
typedef void (*logger_func_t)(int iCmdId, const char* pchStatus);

const char* getCmdName(cmd_id_t id);
int getSendCmdSize(cmd_id_t id);
int getRecvCmdSize(cmd_id_t id);

// 명령 처리 함수 (로그 포함)
int dispatchCommand(int iCmdId, const char* pchPayload, char* pchResponse, logger_func_t logger);


// 명령 핸들러 구조체
typedef struct {
    cmd_id_t cmdId;
    const char* chpCmdName;
    command_func_t handler;
} cmd_handler_t;
#endif // ICD_HANDLER_H