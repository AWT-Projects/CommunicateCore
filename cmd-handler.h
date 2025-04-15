#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H

#include "logger.h"

// 명령 ID 정의
typedef enum {
    CMD_KEEP_ALIVE = 0,
    CMD_IBIT,
    CMD_RBIT,
    CMD_CBIT,

    CMD_UDS_GPS_DATA = 1000,
    CMD_UDS_IMU_DATA,
    CMD_UDS_SP_DATA,
    CMD_UDS_EXTERN_DATA,
    CMD_UDS_KEYBOARD_DATA,
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