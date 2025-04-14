#include <stdio.h>
#include <string.h>
#include "command.h"

int handle_bit(const char* chpPayload, char* chResponse) {
    snprintf(chResponse, 128, "BIT OK");
    return 0;
}

int keepAlive(const char* pchPayload, char* pchResponse)
{
    SND_KEEP_ALIVE* pstSndKeepAlive = (SND_KEEP_ALIVE *)pchResponse;
    pstSndKeepAlive->chResult = 0x01;
    return CMD_SUCCESS;
}

int iBit(const char* pchPayload, char* pchResponse)
{
    SND_IBIT* pstSndIBit = (SND_IBIT *)pchResponse;
    pstSndIBit->chBitTotResult = 0x01;
    pstSndIBit->chPositionResult = 0x01;
    return CMD_SUCCESS;
}

int rBit(const char* pchPayload, char* pchResponse)
{
    SND_RBIT* pstSndRBit = (SND_RBIT *)pchResponse;
    pstSndRBit->chBitTotResult = 0x01;
    pstSndRBit->chPositionResult = 0x01;
    return CMD_SUCCESS;
}

int cBit(const char* pchPayload, char* pchResponse)
{
    RCV_CBIT* pstRcvCBit = (RCV_CBIT *)pchPayload;
    SND_CBIT* pstSndCBit = (SND_CBIT *)pchResponse;
    pstSndCBit->chBitTotResult = 0x01;
    pstSndCBit->chPositionResult = 0x01;
    return CMD_SUCCESS;
}
