/* cmd_handler.c */
#include "cmd-handler.h"
#include "command.h"
#include <stdio.h>
#include <string.h>

// 명령 테이블
static cmd_handler_t handlers[] = {
    { CMD_KEEP_ALIVE,   "KEEP ALIVE",   keepAlive },
    { CMD_IBIT,         "IBIT",         iBit },
    { CMD_RBIT,         "RBIT",         rBit },
    { CMD_CBIT,         "CBIT",         cBit },
    /* UDS */
    { CMD_UDS_GPS_DATA,         "RECV GPS",         recvGpsData },
    { CMD_UDS_IMU_DATA,         "RECV IMU",         recvImuData },
    { CMD_UDS_SP_DATA,          "RECV SP",          recvSpData },
    { CMD_UDS_EXTERN_DATA,      "RECV EXTERN",      recvExternData },
    { CMD_UDS_KEYBOARD_DATA,    "RECV KEYBOARD",    recvKeyboardData }
};

static const int iNumOfHandlers = sizeof(handlers) / sizeof(handlers[0]);

int getSendCmdSize(cmd_id_t id)
{
    switch (id) {
        case CMD_KEEP_ALIVE: return sizeof(SND_KEEP_ALIVE);
        case CMD_IBIT: return sizeof(SND_IBIT);
        case CMD_RBIT: return sizeof(SND_RBIT);
        case CMD_CBIT: return sizeof(SND_CBIT);
        /* UDS */
        case CMD_UDS_GPS_DATA: return sizeof(GPS_DATA);
        case CMD_UDS_IMU_DATA: return sizeof(IMU_DATA);
        case CMD_UDS_SP_DATA: return sizeof(SP_DATA);
        case CMD_UDS_EXTERN_DATA: return sizeof(EXTERN_DATA);
        case CMD_UDS_KEYBOARD_DATA: return sizeof(KEYBOARD_DATA);
        default: return 0;
    }
}

int getRecvCmdSize(cmd_id_t id)
{
    switch (id) {
        case CMD_KEEP_ALIVE: return sizeof(RCV_KEEP_ALIVE);
        case CMD_IBIT: return sizeof(RCV_IBIT);
        case CMD_RBIT: return sizeof(RCV_RBIT);
        case CMD_CBIT: return sizeof(RCV_CBIT);
        /* UDS */
        case CMD_UDS_GPS_DATA: return sizeof(GPS_DATA);
        case CMD_UDS_IMU_DATA: return sizeof(IMU_DATA);
        case CMD_UDS_SP_DATA: return sizeof(SP_DATA);
        case CMD_UDS_EXTERN_DATA: return sizeof(EXTERN_DATA);
        case CMD_UDS_KEYBOARD_DATA: return sizeof(KEYBOARD_DATA);
        default: return 0;
    }
}

const char* getCmdName(cmd_id_t id) 
{
    switch (id) {
        case CMD_KEEP_ALIVE: return "KEEP ALIVE";
        case CMD_IBIT: return "IBIT";
        case CMD_RBIT: return "RBIT";
        case CMD_CBIT: return "CBIT";
        /* UDS */
        case CMD_UDS_GPS_DATA: return "GPS DATA";
        case CMD_UDS_IMU_DATA: return "IMU DATA";
        case CMD_UDS_SP_DATA: return "SP DATA";
        case CMD_UDS_EXTERN_DATA: return "EXTERN DATA";
        case CMD_UDS_KEYBOARD_DATA: return "KEYBOARD DATA";
        default: return "UNKNOWN";
    }
}


// 명령 분배 처리
int dispatchCommand(int iCmdId, const char* pchPayload, char* pchResponse, logger_func_t logger) {
    for (int i = 0; i < iNumOfHandlers; ++i) {
        if (handlers[i].cmdId == iCmdId) {
            if (logger) {
                logger(handlers[i].cmdId, "Start");
            }
            int result = handlers[i].handler(pchPayload, pchResponse);
            if (logger) {
                logger(handlers[i].cmdId, (result == 0) ? "Success" : "Fail");
            }
            return result;
        }
    }
    
    if (logger){
        logger(CMD_UNKNOWN, "Unknown Command");
    }
    return -1;
}
