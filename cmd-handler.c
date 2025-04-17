/* cmd_handler.c */
#include "cmd-handler.h"
#include "command.h"
#include <stdio.h>
#include <string.h>

// 명령 테이블
static cmd_handler_t handlers[] = {
    { CMD_KEEP_ALIVE,           "KEEP ALIVE",           keepAlive },
    { CMD_IBIT,                 "IBIT",                 iBit },
    { CMD_RBIT,                 "RBIT",                 rBit },
    { CMD_CBIT,                 "CBIT",                 cBit },
    /* UDS */
    { UDS2_EXTERN1_DATA,        "EXTERNAL1 DATA",       recvExtern1Data },
    { UDS2_EXTERN2_DATA,        "EXTERNAL2 DATA",       recvExtern2Data },
    { UDS2_EXTERN3_DATA,        "EXTERNAL3 DATA",       recvExtern3Data },
    { UDS2_EXTERN4_DATA,        "EXTERNAL4 DATA",       recvExtern4Data },
    { UDS3_PROCESSING_DATA,     "PROCESSING DATA",      recvProcessingData },
    { UDS4_CTRL_DATA,           "CONTROLL DATA",        recvControllData },
    { UDS4_COMMAND,             "COMMAND",              recvCommandData }
};

static const int iNumOfHandlers = sizeof(handlers) / sizeof(handlers[0]);

int getSendCmdSize(cmd_id_t id)
{
    switch (id) {
        case CMD_KEEP_ALIVE:        return sizeof(SND_KEEP_ALIVE);
        case CMD_IBIT:              return sizeof(SND_IBIT);
        case CMD_RBIT:              return sizeof(SND_RBIT);
        case CMD_CBIT:              return sizeof(SND_CBIT);
        /* UDS */
        case UDS2_EXTERN1_DATA:     return sizeof(EXTERNAL1_DATA);
        case UDS2_EXTERN2_DATA:     return sizeof(EXTERNAL2_DATA);
        case UDS2_EXTERN3_DATA:     return sizeof(EXTERNAL3_DATA);
        case UDS2_EXTERN4_DATA:     return sizeof(EXTERNAL4_DATA);
        case UDS3_PROCESSING_DATA:  return sizeof(PROCESSING_DATA);
        case UDS4_CTRL_DATA:        return sizeof(CONTROLL_DATA);
        case UDS4_COMMAND:          return sizeof(COMMAND_DATA);
        default:                    return 0;
    }
}

int getRecvCmdSize(cmd_id_t id)
{
    switch (id) {
        case CMD_KEEP_ALIVE:        return sizeof(RCV_KEEP_ALIVE);
        case CMD_IBIT:              return sizeof(RCV_IBIT);
        case CMD_RBIT:              return sizeof(RCV_RBIT);
        case CMD_CBIT:              return sizeof(RCV_CBIT);
        /* UDS */
        case UDS2_EXTERN1_DATA:     return sizeof(EXTERNAL1_DATA);
        case UDS2_EXTERN2_DATA:     return sizeof(EXTERNAL2_DATA);
        case UDS2_EXTERN3_DATA:     return sizeof(EXTERNAL3_DATA);
        case UDS2_EXTERN4_DATA:     return sizeof(EXTERNAL4_DATA);
        case UDS3_PROCESSING_DATA:  return sizeof(PROCESSING_DATA);
        case UDS4_CTRL_DATA:        return sizeof(CONTROLL_DATA);
        case UDS4_COMMAND:          return sizeof(COMMAND_DATA);
        default:                    return 0;
    }
}

const char* getCmdName(cmd_id_t id) 
{
    switch (id) {
        case CMD_KEEP_ALIVE:        return "KEEP ALIVE";
        case CMD_IBIT:              return "IBIT";
        case CMD_RBIT:              return "RBIT";
        case CMD_CBIT:              return "CBIT";
        /* UDS */
        case UDS2_EXTERN1_DATA:     return "EXTERNAL1 DATA";
        case UDS2_EXTERN2_DATA:     return "EXTERNAL2 DATA";
        case UDS2_EXTERN3_DATA:     return "EXTERNAL3 DATA";
        case UDS2_EXTERN4_DATA:     return "EXTERNAL4 DATA";
        case UDS3_PROCESSING_DATA:  return "PROCESSING DATA";
        case UDS4_CTRL_DATA:        return "CONTROLL DATA";
        case UDS4_COMMAND:          return "COMMAND";
        default:                    return "UNKNOWN";
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
